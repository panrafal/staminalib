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

$Id: $

*/

#pragma once


namespace Stamina {

	class FastMutex/*: public Lock*/ {
	public:
		__inline FastMutex() {
			_occupied = 0;
		}
		__inline void lock() {
			while (InterlockedCompareExchange(&_occupied, 1, 0) == 1) {
				Sleep(0);
			}
		}
		__inline void unlock() {
			InterlockedCompareExchange(&_occupied, 0, 1);
		}
		__inline bool canAccess() {
			return _occupied != 1;
		}
		__inline int getLockCount() {
			return _occupied;
		}
	private:
        volatile LONG _occupied;
	};

	class SimpleMutex: public Lock {
	public:

		STAMINA_OBJECT_CLASS(SimpleMutex, Lock);

		__inline SimpleMutex() {
			_occupied = 0;
		}
		void lock() {
			while (InterlockedCompareExchange(&_occupied, 1, 0) == 1) {
				Sleep(1);
			}
		}
		void unlock() {
			InterlockedCompareExchange(&_occupied, 0, 1);
		}
		bool canAccess() {
			return _occupied != 1;
		}
		int getLockCount() {
			return _occupied;
		}
	private:
        volatile LONG _occupied;
	};

};