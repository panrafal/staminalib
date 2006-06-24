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

#include "stdafx.h"
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <algorithm>
#include "Thread.h"
#include "ThreadRun.h"
#include "Assert.h"
#include "Helpers.h"
#include "Logger.h"

namespace Stamina {


	
	//
	// Usage: SetThreadName (-1, "MainThread");
	//
	typedef struct tagTHREADNAME_INFO
	{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
	} THREADNAME_INFO;

	void Thread::setName( LPCSTR newName, DWORD threadId)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = newName;
		info.dwThreadID = threadId;
		info.dwFlags = 0;

		__try
		{
			RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
		}
		__except(EXCEPTION_CONTINUE_EXECUTION)
		{
		}
	}



	// ------------------------------------


	uintptr_t __stdcall ThreadRunnerStore::threadStore(RunParams* rp) {
		S_ASSERT(rp);
		Thread* thread = new Thread();
		ThreadItem ti;
		ti.thread = thread;
		ti.startName = rp->name;
		if (ti.startName.empty()) {
			ti.startName = Stamina::inttostr(thread->getId());
		}
		{
			ObjLocker lock(rp->store);
			rp->store->_list.push_front( ti );
		}
		uintptr_t ret = rp->func(rp->args);
		{
			ObjLocker lock(rp->store, lockWrite);
			ThreadList::iterator found = std::find(rp->store->_list.begin(), rp->store->_list.end(), ti );
			if (found != rp->store->_list.end()) {
				rp->store->_list.erase( found );
				delete thread;
			}
		}
		rp->store->release();
		delete rp;
		return ret;
	}


	HANDLE ThreadRunnerStore::beginThread(const char* name, void * sec, unsigned stack,	fThreadProc cb, void * args, unsigned flag, unsigned * addr) {
		this->hold();
		RunParams* rp;
		{
			ObjLocker(this, lockWrite);
			rp = new RunParams;
		rp->func = cb;
		rp->args = args;
		rp->store = this;
		if (name) {rp->name = name;}
		}
		HANDLE handle = (HANDLE) this->_beginThread(name, sec, stack, (fThreadProc)threadStore, rp, flag, addr);
		return handle;
	}

	int ThreadRunnerStore::waitForThreads(int timeout, int globalTimeout, bool terminateOnTimeout) {
		int timeouts = 0;
		while (1) {
			this->lock(lockWrite);
				if (this->_list.empty()) {
					this->unlock();
					break;
				}
				ThreadItem ti = this->_list.front();
				Thread thread = *ti.thread;
				this->_list.erase(this->_list.begin());
				delete ti.thread;
			this->unlock(lockWrite);
			DWORD ticks = GetTickCount();
			Stamina::log(logLog, "Stamina", "ThreadRunnerStore::waitForThreads", "Waiting for '%s' thread", ti.startName.c_str());
			if (thread.wait(min(timeout, globalTimeout)) == WAIT_TIMEOUT) {
				Stamina::log(logError, "Stamina", "ThreadRunnerStore::waitForThreads", "Thread '%s' timed out! %s", ti.startName.c_str(), terminateOnTimeout ? "terminating" : "");
				timeouts ++;
				if (terminateOnTimeout) {
					thread.terminate(WAIT_TIMEOUT);
				}
			}
			if (GetTickCount() > ticks) {
				globalTimeout -= GetTickCount() - ticks;
			}
			if (globalTimeout < 0) globalTimeout = 0;
		}
		return timeouts;
	}


};
