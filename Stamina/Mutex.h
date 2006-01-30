/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: Lock.h 8 2005-06-15 14:24:57Z hao $
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
				Sleep(1);
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