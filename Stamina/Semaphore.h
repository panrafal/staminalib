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

#include "Assert.h"
#include "Event.h"
#include "Lock.h"

namespace Stamina {

	class Semaphore: public Lock, public WaitableHandle /*nie ma virtuali*/  {
	public:

		STAMINA_OBJECT_CLASS(Semaphore, Lock);

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