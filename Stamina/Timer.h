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

#if (_WIN32_WINNT < 0x0400)

#error WIN32 must be >= 0x0400!

#endif


namespace Stamina {

	class TimerBasic {
	public:
		inline TimerBasic(LPSECURITY_ATTRIBUTES lpTimerAttributes = 0, bool bManualReset = false, LPCTSTR lpTimerName = 0) {
			_handle = CreateWaitableTimer(lpTimerAttributes, bManualReset, lpTimerName);
		}
		inline TimerBasic(const TimerBasic& b) {
			DuplicateHandle(GetCurrentProcess(), b._handle, GetCurrentProcess(), &this->_handle, 0, 0, DUPLICATE_SAME_ACCESS);
		}

		inline virtual ~TimerBasic() {
			if (_handle) {
				CancelWaitableTimer(_handle);
				CloseHandle(_handle);
			}
		}

		inline void repeat(PTIMERAPCROUTINE routine, void* argument, unsigned int period, bool resume = false) {
			this->start(routine, argument, period, period);
		}

		inline void start(PTIMERAPCROUTINE routine, void* argument, unsigned int delay, unsigned int period = 0, bool resume = false) {
			__int64 due = -10000/* * delay*/;
			due *= delay;
			int setTimerRes = SetWaitableTimer(_handle, (LARGE_INTEGER*)&due, period, routine, argument, resume);
			S_ASSERT(setTimerRes != 0);
		}

		inline void stop() {
			CancelWaitableTimer(_handle);
		}


	protected:
		HANDLE _handle;
	};

	class Timer: public TimerBasic {
	public:
		Timer(LPSECURITY_ATTRIBUTES lpTimerAttributes = 0, bool bManualReset = false, LPCTSTR lpTimerName = 0):TimerBasic(lpTimerAttributes, bManualReset, lpTimerName) {
			_running = 0;
		}
		Timer(const Timer& b):TimerBasic(b) {
			_running = 0;
		}

		inline void repeat(unsigned int period, bool resume = false) {
			this->start(period, period);
		}

		inline void start(unsigned int delay, unsigned int period = 0, bool resume = false) {
			TimerBasic::start((PTIMERAPCROUTINE)Timer::timerAPCProc, this, delay, period, resume);
			_running = (period ? 2 : 1);
		}
		inline void stop() {
			_running = 0;
			TimerBasic::stop();
		}

		inline bool isRunning() {
			return _running > 0;
		}


	protected:
		char _running;

		virtual void timerProc(__int64 nanotime) = 0;


		static void __stdcall timerAPCProc(Timer* timer, int low, int high) {
			if (timer->_running == 1) timer->_running = 0;
			timer->timerProc((__int64)low | ((__int64)high << 32));
		}
	};


	class TimerDynamic:public Timer {
	public:
		TimerDynamic(bool autoDestroy, LPSECURITY_ATTRIBUTES lpTimerAttributes = 0, bool bManualReset = false, LPCTSTR lpTimerName = 0):Timer(lpTimerAttributes, bManualReset, lpTimerName), _autoDestroy(autoDestroy), _period(0) {}

		inline void repeat(unsigned int period, bool resume = false) {
			this->start(period, period);
		}

		inline void start(unsigned int delay, unsigned int period = 0, bool resume = false) {
			_period = period;
			Timer::start(delay, period, resume);
		}

		inline void stop() {
			this->_period = 0;
			Timer::stop();
			if (_autoDestroy)
				delete this;
		}

	protected:
		bool _autoDestroy : 1;
		unsigned int _period;
		void timerProc(__int64 nanotime) {
			if (_autoDestroy && _period == 0)
				stop();
		}

		
	};

	template <class F> class TimerTmpl:public TimerDynamic {
	public:
		TimerTmpl(F& func, bool autoDestroy, LPSECURITY_ATTRIBUTES lpTimerAttributes = 0, bool bManualReset = false, LPCTSTR lpTimerName = 0):TimerDynamic(autoDestroy, lpTimerAttributes, bManualReset, lpTimerName), _func(func) {}

	private:

		void timerProc(__int64 nanotime) {
			bool oldAuto = _autoDestroy;
			_autoDestroy = false;
			_func(this, nanotime);
			_autoDestroy = oldAuto;
			TimerDynamic::timerProc(nanotime);
		}

		F _func;
	};


	template <class F> 
	inline TimerTmpl<F>* timerTmplCreate(F& func, bool autoDestroy = false, LPSECURITY_ATTRIBUTES lpTimerAttributes = 0, bool bManualReset = false, LPCTSTR lpTimerName = 0) {
		return new TimerTmpl<F>(func, autoDestroy, lpTimerAttributes, bManualReset, lpTimerName);
	}


};