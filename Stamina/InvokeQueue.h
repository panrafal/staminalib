/*

The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License from
/LICENSE.HTML in this package or at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is "Stamina.lib" library code, released Feb 1, 2006.

The Initial Developer of the Original Code is "STAMINA" - Rafa³ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa³ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

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