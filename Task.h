/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once

#include "ObjectImpl.h"
#include <string>
#include "ThreadRun.h"
#include "Logger.h"

namespace Stamina {

	class Task: public LockableObject<iLockableObject> {
	public:

		enum State {
			stateNone,
			stateCreated,
			stateRunning,
			stateShuttingDown,
			stateShutdown,
		};

	public:
		Task(const std::string& name, bool defaultRunner = true) : _shutdownEvent(0, true), _state(stateNone), _name(name) {
			if (defaultRunner) {
				init(new ThreadRunner());
			}
		}
		Task(const std::string& name,  const oThreadRunner& threadRunner) : _shutdownEvent(0, true), _state(stateNone), _name(name) {
			init(threadRunner);
		}

		void init(const oThreadRunner& threadRunner) {
			unsigned int threadId = 0;
			HANDLE thread = threadRunner->runEx((ThreadRunner::fThreadProc) threadProcedureCall, this, _name.c_str(), 0, true, &threadId);
			_thread = Thread( thread, threadId );
			CloseHandle(thread);
			_state = stateCreated;
		}


		void run() {
			ObjLocker lock(this);
			if (_state != stateCreated) return;
			_thread.resume();
			_state = stateRunning;
		}

		void shutdown() {
			if (_state > stateRunning) return;

			Stamina::log(logFunc, "Stamina", "Task::shutdown", "name=%s", _name.c_str());
			ObjLocker lock(this);
			_state = stateShuttingDown;
			_shutdownEvent.set();
		}

		void waitUntilShutdown() {
			if (_state == stateShutdown || _state < stateRunning) return;

			Stamina::log(logFunc, "Stamina", "Task::waitUntilShutdown", "name=%s START", _name.c_str());
			_thread.wait(INFINITE);
			Stamina::log(logMisc, "Stamina", "Task::waitUntilShutdown", "name=%s FINISHED", _name.c_str());
		}

		inline bool isRunning() const {
			return _state == stateRunning;
		}
		inline bool isShutdown() const {
			return _state == stateShutdown;
		}
		inline bool isShuttingDown() const {
			return _state == stateShuttingDown;
		}

		inline State getState() const {
			return _state;
		}

		inline const Event& getShutdownEvent() const {
			return _shutdownEvent;
		}

		inline const Thread& getThread() const {
			return _thread;
		}

	protected:

		virtual void threadProcedure () {
			Stamina::log(logFunc, "Stamina", "Task::threadProcedure", "name='%s' shutdown", _name.c_str());
			_state = stateShutdown;

		}


		static void __stdcall threadProcedureCall(Task* p) {
			p->threadProcedure();
		}

		State _state;
		Event _shutdownEvent;
		Thread _thread;
		std::string _name;
	};

};