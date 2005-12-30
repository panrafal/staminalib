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

	class Lock_blank:public Lock {
	public:
		static Lock_blank instance;
		__inline void lock() {} 
		__inline void unlock() {} 
		int getLockCount() {return 0;}
		__inline bool canAccess() {return true;}
	};

	__declspec(selectany) Lock_blank Lock_blank::instance;

	/* Scoped locking */
	template <class TLO> class LockerTmpl {
	public:
		__inline LockerTmpl(const TLO* lo):_lo(const_cast<TLO*>(lo)) {
			_lo->lock();
		}
		__inline LockerTmpl(const TLO& lo):_lo(&const_cast<TLO&>(lo)) {
			_lo->lock();
		}
		__inline ~LockerTmpl(){
			_lo->unlock();
		}
	protected:
		TLO * _lo;
	};
	typedef LockerTmpl<Lock> Locker;
	typedef Locker LockerCS;




	/** FastLocker works the same as LockerTmpl. However it doesn't use virtual functions, but call them directly on specified class (which could enable some compiler optimizations).
	*/
	template <class TLO> class FastLocker {
	public:
		__inline FastLocker(TLO* lo):_lo(lo) {
			_lo->TLO::lock();
		}
		__inline FastLocker(TLO& lo):_lo(&lo) {
			_lo->TLO::lock();
		}
		__inline ~FastLocker(){
			_lo->TLO::unlock();
		}
	protected:
		TLO * _lo;
	};


};