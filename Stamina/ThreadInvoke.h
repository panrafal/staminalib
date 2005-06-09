/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

/**@file


*/
#pragma once

#include "Event.h"
#include "Thread.h"
#include "Assert.h"

namespace Stamina {


	template <class F> class __ThreadInvoke {
	public:

		static void invoke(iQueueFunction& queue, F & f, bool force) {
			S_ASSERT(&f);
			__ThreadInvoke* tf = new __ThreadInvoke(f);
			queue.queueFunction((PAPCFUNC)callback, tf, force);
		}

	private:
		__ThreadInvoke(F& func):_func(func) {}

		static void __stdcall callback(__ThreadInvoke * tf) {
			S_ASSERT(tf);
			tf->_func();
			delete tf;
		}

		F _func;
	};


	template <class F> 
	inline void threadInvoke(iQueueFunction& queue, F & f, bool force=true) {
		__ThreadInvoke<F>::invoke(queue, f, force);
	}

	template <class F> class __ThreadInvokeWait {
	public:

		static void invoke(iQueueFunction& queue, F & f, bool force=true, bool alertable=false, bool waitAfter=true) {
			S_ASSERT(&f);
			Event event;
			__ThreadInvokeWait* tfw = new __ThreadInvokeWait(f, event, waitAfter);
			queue.queueFunction((PAPCFUNC)callback, tfw, force);
			event.wait(INFINITE, alertable);
		}

	protected:
		__ThreadInvokeWait(F & func, Event& event, bool waitAfter): _waitAfter(waitAfter), _wait(event) {
			_func = waitAfter ? &func : new F (func);
		}
		~__ThreadInvokeWait() {
			if (_waitAfter == false)
				delete _func;
		}

		static void __stdcall callback(__ThreadInvokeWait * tfw) {
			S_ASSERT(tfw);
			if (tfw->_waitAfter == false)
				tfw->_wait.set();
			F& func = *tfw->_func;
			func();
			if (tfw->_waitAfter == true)
				tfw->_wait.set();
			delete tfw;
		}

		F * _func;
		Event& _wait;
		bool _waitAfter;
	};

	template <class F> 
	inline void threadInvokeWait(iQueueFunction& queue, F & f, bool force=true, bool alertable=false, bool waitAfter=true) {
		__ThreadInvokeWait<F>::invoke(queue, f, force, alertable, waitAfter);
	}


	template <typename Ret, class F> class __ThreadInvokeReturn:public __ThreadInvokeWait<F> {
	public:
		
		static Ret invoke(iQueueFunction& queue, F & f, bool force=true, bool alertable=false) {
			S_ASSERT(&f);
			__ThreadInvokeReturn tfr (f);
			tfr._wait = CreateEvent(0, 0, 0, 0);
			S_ASSERT(tfr._wait);
			queue.queueFunction((PAPCFUNC)callback, &tfr, force);
			WaitForSingleObjectEx(tfr._wait, INFINITE, alertable);
			return tfr._retVal;
		}

	private:
		__ThreadInvokeReturn(F & func):__ThreadInvokeWait<F>(func) {}

		static void __stdcall callback(__ThreadInvokeReturn * tfr) {
			S_ASSERT(tfr);
			tfr->_retVal = tfr->_func();
			SetEvent(tfr->_wait);
		}

		Ret _retVal;
	};

	template <typename Ret, class F> 
	inline Ret threadInvokeReturn(iQueueFunction& queue, F & f, bool force=true, bool alertable=false) {
		return __ThreadInvokeReturn<Ret, F>::invoke(queue, f, force, alertable);
	}


#ifdef __STAMINA_OBJECT__

	template <class F> class __ThreadInvokeObject {
	public:

		static void invoke(iQueueFunction& queue, iSharedObject* obj, F & f, bool force) {
			S_ASSERT(&f);
			__ThreadInvokeObject* tf = new __ThreadInvokeObject(obj, f);
			queue.queueFunction((PAPCFUNC)callback, tf, force);
		}

	private:
		__ThreadInvokeObject(iSharedObject* obj, F& func):_func(func), _obj(obj) {}

		static void __stdcall callback(__ThreadInvokeObject * tf) {
			S_ASSERT(tf);
			tf->_func();
			delete tf;
		}

		F _func;
		SharedPtr<iSharedObject> _obj;
	};


	template <class F> 
	inline void threadInvokeObject(iQueueFunction& queue, iSharedObject* obj, F & f, bool force=true) {
		__ThreadInvokeObject<F>::invoke(queue, obj, f, force);
	}

#endif

};