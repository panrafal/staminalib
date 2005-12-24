/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

/**@file


*/
#pragma once

#include "Assert.h"
#include "Event.h"

namespace Stamina {

	/**  */
	class iQueueFunction {
	public:
		virtual bool queueFunction(PAPCFUNC callback, void* param, bool force) = 0;
	};


	class Thread: public WaitableHandle, public iQueueFunction {
	public:
		inline Thread(HANDLE threadHandle = GetCurrentThread(), unsigned int threadId = 0):WaitableHandle() {
			this->copyHandle(threadHandle);
			this->_threadId = threadId ? threadId : ( threadHandle == GetCurrentThread() ? GetCurrentThreadId() : 0 );
		}
		inline Thread(const Thread& b):WaitableHandle(b) {
			this->_threadId = b._threadId;
		}
		inline Thread& operator = (const Thread& b) {
			this->copyHandle(b);
			this->_threadId = b._threadId;
			return *this;
		}
		~Thread() {
		}

		bool operator == (const Thread& b) const {
			if (this->_threadId && b._threadId) {
				return this->_threadId == b._threadId;
			} else {
				return this->_handle == b._handle;
			}
		}

		inline unsigned int getId() const {
			return _threadId;
		}

		inline bool isCurrent() const {
			S_ASSERT(this->_threadId);
			return _threadId == GetCurrentThreadId();
		}


#if(_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
		bool queueFunction(PAPCFUNC func, void* param, bool force) {
			if (!force && this->isCurrent()) {
				func((ULONG_PTR)param);
				return true;
			} else {
				return ::QueueUserAPC(func, this->getHandle(), (ULONG_PTR)param) != 0;
			}
		}
#endif


		inline void setName( LPCSTR newName) {
			setName(newName, this->getId());
		}
		static void setName( LPCSTR newName, DWORD threadId);

		inline void resume() const {
			ResumeThread(this->getHandle());
		}

		inline void suspend() const {
			SuspendThread(this->getHandle());
		}

		inline void terminate(int exitCode = 0) {
			TerminateThread(this->getHandle(), exitCode);
		}

		inline void setPriority(int priority) {
			SetThreadPriority(this->getHandle(), priority);
		}

	private:

		unsigned int _threadId;
	};

};