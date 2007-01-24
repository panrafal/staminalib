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

#include "Stamina.h"
#include "Assert.h"

namespace Stamina {

	class WaitableHandle {
	public:

		inline WaitableHandle(const WaitableHandle& object) {
			_handle = 0;
			this->copyHandle(object);
		}
		
		inline WaitableHandle& operator = (const WaitableHandle& object) {
			_handle = 0;
			this->copyHandle(object);
			return *this;
		}

		inline WaitableHandle (HANDLE handle = 0, bool copy = true) {
			_handle = 0;
			if (handle && copy) 
				this->setHandleDuplicate(handle);
			else
				this->setHandle(handle);
		}

		inline int wait(unsigned int milliseconds, bool alertable = false) const {
			S_ASSERT(getHandle());
			return WaitForSingleObjectEx(getHandle(), milliseconds, alertable);
		}

		inline int wait(const WaitableHandle& e1, unsigned int milliseconds, bool waitAll = false, bool alertable = false) const {
			S_ASSERT(getHandle());
			HANDLE list [2] = {getHandle(), e1.getHandle()};
			return WaitForMultipleObjectsEx(2, list, waitAll, milliseconds,alertable);
		}

		int waitForTimeout(unsigned int milliseconds, bool alertable = true) const 
{
			S_ASSERT(getHandle());
			int r;
			do  {
				DWORD start = GetTickCount();
				if (milliseconds < 0)
					milliseconds = 0;
				r = WaitForSingleObjectEx(getHandle(), milliseconds, alertable);
				if (r == WAIT_IO_COMPLETION) {
					DWORD stop = GetTickCount();
					if (stop < start) { // wrap around protection
						milliseconds -= (0xFFFFFFFF - start) + stop;
					} else {
						if (stop - start > milliseconds) {
							milliseconds = 0;
						} else {
							milliseconds -= stop - start;
						}
					}
				} else {
					break;
				}
			} while (milliseconds > 0);
			return r;
		}

		~WaitableHandle() {
			this->closeHandle();
		}


		inline static HANDLE duplicateHandle(HANDLE handle, HANDLE srcProcess = GetCurrentProcess(), HANDLE destProcess = GetCurrentProcess(), int options = DUPLICATE_SAME_ACCESS) {
			if (!handle) return 0;
			HANDLE newHandle = 0;
			bool handleCloned = DuplicateHandle(srcProcess, 
				handle, destProcess, &newHandle, 0, 0, options) != 0;
			S_ASSERT(handleCloned);
			S_ASSERT(newHandle);
			return newHandle;
		}

		inline HANDLE getHandleDuplicate(HANDLE destProcess = GetCurrentProcess()) const {
			return duplicateHandle(this->getHandle());
		}

		inline HANDLE getHandle() {
			return _handle;
		}

	protected:

		inline void setHandle(HANDLE handle) {
			_handle = handle;
		}

		inline HANDLE getHandle() const {
			return _handle;
		}
		inline void closeHandle() {
			HANDLE handle = this->getHandle();
			if (handle) {
				CloseHandle(handle);
				this->setHandle(0);
			}
		}
		inline void resetHandle() {
			this->setHandle(0);
		}
		inline void copyHandle(const WaitableHandle& b) {
			setHandleDuplicate(b.getHandle());
		}
		inline void setHandleDuplicate(HANDLE b) {
			this->closeHandle();
			this->setHandle( duplicateHandle(b) );
		}


		HANDLE _handle;
	};

	class Event: public WaitableHandle {
	public:
		inline Event(LPSECURITY_ATTRIBUTES attributes=0, bool manualReset=false, bool initialState=false, LPCTSTR name=0) {
			_handle = CreateEvent(attributes, manualReset, initialState, name);
			S_ASSERT(_handle);
		}
		inline Event(DWORD desiredAccess, bool inheritHandle, LPCTSTR name) {
			_handle = OpenEvent(desiredAccess, inheritHandle, name);
			S_ASSERT(_handle);
		}

		inline void pulse() {
			S_ASSERT_RUN( PulseEvent(_handle) );
		}
		inline void set() {
			S_ASSERT_RUN( SetEvent(_handle) );
		}
		inline void reset() {
			S_ASSERT_RUN( ResetEvent(_handle) );
		}



		/*TODO:Finish*/


	};

};