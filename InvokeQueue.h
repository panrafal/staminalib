/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once

#include "CriticalSection.h"
#include <queue>
#include "Event.h"
#include "Task.h"

namespace Stamina {



	class InvokeQueue: public iQueueFunction {
	public:

		InvokeQueue():_isQueued(0, true) {
			
		}

		//void (__stdcall *fQueuedFunction)(void*);
		typedef PAPCFUNC fQueuedFunction;

		bool queueFunction(fQueuedFunction func, void* param, bool force) {
			LockerCS lock(_cs);
			QueueItem qi = {func, param};
			_queue.push(qi);
			_isQueued.set();
			return true;
		}

		void runQueue();

		inline bool isQueued() {
			LockerCS lock(_cs);
			return _queue.empty() == false;
		}

		inline const Event& getQueueEvent() {
			return _isQueued;

		}


	protected:
		struct QueueItem {
			fQueuedFunction func;
			void * param;
		};
		std::queue<QueueItem> _queue;
		CriticalSection _cs;
		Event _isQueued;
	};


	class InvokeQueueTask: public Task, public InvokeQueue {
	public:
		InvokeQueueTask(const std::string& name, bool defaultRunner = true):Task(name, defaultRunner) {
		}
		InvokeQueueTask(const std::string& name,  const oThreadRunner& threadRunner):Task(name, threadRunner) {
		}

		bool isCurrent() {
			return this->getThread().isCurrent();
		}

	protected:

		void threadProcedure () {
			Stamina::log(logFunc, "Stamina", "InvokeQueueTask::threadProcedure", "name='%s'", _name.c_str());
			while (_isQueued.wait(_shutdownEvent, INFINITE) - WAIT_OBJECT_0 == 0) {
				this->runQueue();
			}
			Task::threadProcedure();
		}
	};


};