/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once


namespace Stamina {


	class Lock {
	public:
		virtual void lock() = 0;
		virtual void unlock() = 0;
		virtual bool canAccess() = 0;
		virtual bool tryLock() {
			if (canAccess()) {
				this->lock();
				return true;
			}
			return false;
		}
		//virtual void access() {lock(); unlock();}
		virtual int getLockCount() = 0;
	};

	/* Scoped locking of iLockable */
	template <class TLO> class LockerTmpl {
	public:
		__inline LockerTmpl(TLO* lo):_lo(lo) {
			lock();
		}
		__inline LockerTmpl(TLO& lo):_lo(&lo) {
			lock();
		}
		__inline ~LockerTmpl(){
			unlock();
		}
		__inline void lock() {
			_lo->lock();
		}
		__inline void unlock() {
			_lo->unlock();
		}
	protected:
		TLO * _lo;
	};
	typedef LockerTmpl<Lock> Locker;
	typedef Locker LockerCS;


};