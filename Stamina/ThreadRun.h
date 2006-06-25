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

#include <process.h>
#include <boost\shared_ptr.hpp>
#include <string>
#include <list>

#include "Thread.h"
#include "ObjectImpl.h"
#include "Semaphore.h"


namespace Stamina {

	template <class F> unsigned int __stdcall  __threadRunProc(void* func) {
		S_ASSERT(func != 0);
		F* f = (F*)func;
		(*f)();
		delete f;
		return 0;
	}

	typedef unsigned int ( __stdcall * fThreadProc)( void * );
	template <class F> inline HANDLE threadRunEx(F& func, const char* name = 0, SECURITY_ATTRIBUTES* attr = 0, bool suspended=false, unsigned int* threadId = 0) {
		F* f = new F(func);
		unsigned int id;
		HANDLE handle = (HANDLE) _beginthreadex(attr, 0, (fThreadProc)(__threadRunProc<F>), (void*)f, suspended ? CREATE_SUSPENDED : 0 , &id);
		S_ASSERT(handle != 0 && handle != -1);
		if (threadId)
			*threadId = id;
		if (name && *name) {
			Thread::setName(name, id);
		}
		return handle;
	}
	template <class F> inline void threadRun(F& func, const char* name = 0, SECURITY_ATTRIBUTES* attr = 0, bool suspended=false, unsigned int* threadId = 0) {
		CloseHandle(threadRunEx(func, name, attr, suspended, threadId));
	}

	// --------------------------

	typedef SharedPtr<class ThreadRunner> oThreadRunner;
	typedef SharedPtr<class ThreadRunnerStore> oThreadRunnerStore;

	/**
	
	*/
	class ThreadRunner: public SharedObject<iSharedObject, LockableObject<iSharedObject, Stamina::FastMutex> > {
	public:

		STAMINA_OBJECT_CLASS(ThreadRunner, iSharedObject);

		static uintptr_t defaultRunner (const char* name, void * sec, unsigned stack,	fThreadProc cb, void * args, unsigned flag, unsigned * addr) {
			unsigned int id;
			uintptr_t handle = (uintptr_t) _beginthreadex(sec, stack, cb, args, flag, &id);
			S_ASSERT(handle != 0 && handle != -1);
			if (addr) {
				*addr = id;
			}
			if (name && *name) {
				Thread::setName(name, id);
			}
			return handle;
		}


//		typedef uintptr_t (*fBeginThread)(void *, unsigned,
//			unsigned (__stdcall *) (void *), void *, unsigned, unsigned *);
		typedef uintptr_t (*fBeginThread)(const char*, void *, unsigned,
			unsigned (__stdcall *) (void *), void *, unsigned, unsigned *);

		typedef unsigned int ( __stdcall * fThreadProc)( void * );

		ThreadRunner(fBeginThread func = defaultRunner) {
			S_ASSERT(func != 0);
			this->_beginThread = func;
		}
		
		void setRunner(fBeginThread func) {
			ObjLocker l (this);
			S_ASSERT(func != 0);
			this->_beginThread = func;
		}

		template <class F> inline HANDLE runEx(F& func, const char* name = 0, SECURITY_ATTRIBUTES* attr = 0, bool suspended=false, unsigned int* threadId = 0) {
			typedef unsigned int ( __stdcall * fThreadProc)( void * );
			F* f = new F(func);
			return this->beginThread(name, attr, 0, (fThreadProc)(__threadRunProc<F>), (void*)f, suspended ? CREATE_SUSPENDED : 0, threadId);
		}
		template <class F> inline void run(F& func, const char* name = 0, SECURITY_ATTRIBUTES* attr = 0, bool suspended=false, unsigned int* threadId = 0) {
			CloseHandle(runEx(func, name, attr, suspended, threadId));
		}

		inline HANDLE runEx(fThreadProc func, void* param, const char* name = 0, SECURITY_ATTRIBUTES* attr = 0, bool suspended=false, unsigned int* threadId = 0) {
			return this->beginThread(name, attr, 0, func, param, suspended ? CREATE_SUSPENDED : 0, threadId);
		}
		inline void run(fThreadProc func, void* param, const char* name = 0, SECURITY_ATTRIBUTES* attr = 0, bool suspended=false, unsigned int* threadId = 0) {
			CloseHandle(runEx(func, param, name, attr, suspended, threadId));
		}

	protected:

		virtual HANDLE beginThread(const char* name, void * sec, unsigned stack,	fThreadProc cb, void * args, unsigned flag, unsigned * addr) {
			HANDLE handle = (HANDLE) _beginThread(name, sec, stack, cb, args, flag, addr);
			S_ASSERT(handle != 0 && handle != (HANDLE)-1);
			return handle;
		}

		fBeginThread _beginThread;
	};


	// -------------------------------------------

	class ThreadRunnerStore: public ThreadRunner {
	public:

		STAMINA_OBJECT_CLASS(ThreadRunnerStore, ThreadRunner);

		ThreadRunnerStore(fBeginThread func = defaultRunner):ThreadRunner(func) {
		}

		~ThreadRunnerStore() {
			_list.clear();
		}

		/** Waits until all threads are terminated.
		@param timeout - Timeout value for one thread
		@param globalTimeout - Timeout value for whole operation
		@param terminateOnTimeout - true if thread should be terminated on timeout.

		Threads are being checked/terminated in FIFO direction.
		*/
		int waitForThreads(int timeout, int globalTimeout, bool terminateOnTimeout);

		struct ThreadItem {
			bool operator == (const ThreadItem& b) const {
				return thread == b.thread;
			}

			Thread* thread;
			std::string startName;
        };

	protected:

		struct RunParams {
			fThreadProc func;
			void * args;
			ThreadRunnerStore* store;
			std::string name;
		};

		typedef std::list<ThreadItem> ThreadList;

		ThreadList _list;

		static uintptr_t __stdcall threadStore(RunParams* rp);


		virtual HANDLE beginThread(const char* name, void * sec, unsigned stack,	fThreadProc cb, void * args, unsigned flag, unsigned * addr);

	};


};