/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
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
		Thread* thread = new Thread();
		ThreadItem ti;
		ti.thread = thread;
		ti.startName = rp->name;
		if (ti.startName.empty()) {
			ti.startName = Stamina::inttostr(thread->getId());
		}
		{
			ObjLocker(rp->store);
			rp->store->_list.push_front( ti );
		}
		uintptr_t ret = rp->func(rp->args);
		{
			ObjLocker lock(rp->store);
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
		this->lock();
		RunParams* rp = new RunParams;
		rp->func = cb;
		rp->args = args;
		rp->store = this;
		if (name) rp->name = name;
		this->unlock();
		HANDLE handle = (HANDLE) this->_beginThread(name, sec, stack, (fThreadProc)threadStore, rp, flag, addr);
		return handle;
	}

	int ThreadRunnerStore::waitForThreads(int timeout, int globalTimeout, bool terminateOnTimeout) {
		int timeouts = 0;
		while (1) {
			this->lock();
				if (this->_list.empty()) break;
				ThreadItem ti = this->_list.front();
				Thread thread = *ti.thread;
				this->_list.erase(this->_list.begin());
				delete ti.thread;
			this->unlock();
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