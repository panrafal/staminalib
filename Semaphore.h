/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once

#include "Assert.h"
#include "Event.h"
#include "Lock.h"

namespace Stamina {

	class Semaphore: public WaitableHandle, public Lock {
	public:
		inline Semaphore(LPSECURITY_ATTRIBUTES attributes=0, int count=1, int maximum=0x7FFFFFFF, LPCTSTR name=0) {
			_handle = CreateSemaphore(attributes, count, maximum, name);
			S_ASSERT(_handle);
		}
		inline Semaphore(DWORD desiredAccess, bool inheritHandle, LPCTSTR name) {
			_handle = OpenSemaphore(desiredAccess, inheritHandle, name);
			S_ASSERT(_handle);
		}


		inline int obtain(int timeout = INFINITE, bool alertable = false) {
			return this->wait(timeout, alertable);
		}

		inline int release(int releaseBy = 1) {
			LONG count;
			if (ReleaseSemaphore(this->_handle, releaseBy, &count)) {
				return count;
			} else {
				return -1;
			}
		}

		/** Checks if semaphore is available */
		inline bool isOpened() {
			if (this->obtain(0) == WAIT_OBJECT_0) {
				this->release();
				return true;
			}
			return false;
		}

		virtual void lock() {
			this->obtain();
		}
		virtual void unlock() {
			this->release();
		}
		virtual bool canAccess() {
			return this->isOpened();
		}
		virtual int getLockCount() {
			return -1;
		}


	};

	/** Contextual semaphore... Opens upon creation */
	class SemaphoreCtx {
	public:
		inline SemaphoreCtx(Semaphore& s):_obj(s) {
			_obj.obtain();
		}
		inline ~SemaphoreCtx() {
			_obj.release();
		}
	private:
		Semaphore& _obj; 
	};

};