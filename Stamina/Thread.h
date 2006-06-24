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
				int queueRet = ::QueueUserAPC(func, this->getHandle(), (ULONG_PTR)param) != 0;
				S_ASSERT(queueRet);
				return queueRet;
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