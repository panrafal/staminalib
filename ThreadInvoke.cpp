/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#define _WIN32_WINNT 0x501
#include <windows.h>
#include "ThreadInvoke.h"
#include "Assert.h"

namespace Stamina {
/*

	template <class F> 
	void __stdcall ThreadFunc<F>::callback(ThreadFunc* tf) {
	}

	template <class F> 
	void ThreadFuncWait<F>::invoke(Thread& thread, F& f, bool force, bool alertable) {
	}
	template <class F> 
	void __stdcall ThreadFuncWait<F>::callback(ThreadFuncWait* tfw) {
		S_ASSERT(tfw);
		tfw->_func();
		SetEvent(tfw->_wait);
	}


	template <typename Ret, class F> 
	Ret ThreadFuncReturn<Ret, F>::invoke(Thread& thread, F& f, bool force, bool alertable) {
		S_ASSERT(&f);
		ThreadFuncReturn tfr (f);
		this->_wait = CreateEvent(0, 0, 0, 0);
		S_ASSERT(this->_wait);
		QueueUserAPC((APCProc)callback, thread, (ULONG_PTR)&tfr);
		WaitForSingleObjectEx(this->_wait, INFINITE, alertable);
		return retVal;
	}

	template <typename Ret, class F>
	void __stdcall ThreadFuncReturn<Ret, F>::callback(ThreadFuncReturn* tfr) {
		S_ASSERT(tfr);
		tfr->_retVal = tfr->_func();
		SetEvent(tfr->_wait);
	}
*/

};