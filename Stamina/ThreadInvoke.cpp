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