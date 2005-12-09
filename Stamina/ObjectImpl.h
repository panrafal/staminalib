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

#ifndef __STAMINA_OBJECTIMPL__
#define __STAMINA_OBJECTIMPL__


#include "Object.h"
#include "CriticalSection.h"
#include "Mutex.h"



namespace Stamina {

	/** Basic object implementation */
	template <class OI /**Object interface*/> class Object: public OI {
	public:

	};


	/** iLockableObj implementation template
	@param OI - object's interface
	@param OIMP - Object implementation (iObject)
	@param TCS - locker class to use (needs functions lock() and unlock())
	*/
	template <class OI, class TCS = Stamina::CriticalSection, class OIMP = Object<OI> > class LockableObject: public OIMP {
	public:
		void lock() const {
			const_cast<LockableObject*>(this)->_cs.lock();
		}
		/** Odblokowuje dostêp do obiektu */
		void unlock() const {
			const_cast<LockableObject*>(this)->_cs.unlock();
		}
		TCS & CS() {return _cs;}
	private:
		TCS _cs;
	};


	typedef LockerTmpl<const iLockableObject> ObjLocker;

	/** iSharedObj implementation
	@param IO - object's interface
	@param LO - Lockable object implementation
	*/
	template <class OI, class LO = LockableObject<OI> /*Locker Obj*/> 
	class SharedObject: public LO {
	public:
		SharedObject() {
			this->_useCount=1; ///< trzyma sam siebie...
		}
		~SharedObject() {
			// je¿eli ani razu nie wywo³amy hold/release znaczy ¿e obiekt by³ utworzony i usuniêty bez u¿ycia SharedPtr i ma _useCount == 1
			S_ASSERT(this->_useCount == 0 || this->_useCount == 1);
		}
		bool hold() {
			LockerTmpl<LO>(this);
			if (this->_useCount == 0)
				return false;
			this->_useCount++;
			return true;
		}
		void release() {
			{
				LockerTmpl<LO>(this);
				if (this->_useCount < 1) {
					//IMDEBUG(DBG_ASSERT, "SharedObj released during destroy");
					return;
				}
				//K_ASSERT(this->_useCount >= 1);
				unsigned int newCount = --this->_useCount;
				if (newCount == 1) {
					this->_useCount = 0;
				}
			}
			if (this->_useCount == 0) {
				this->destroy();
			}
		}
		/** Zwraca true je¿eli z obiektu mo¿na korzystaæ
		*/
		bool isValid() {
			LockerTmpl<LO>(this);
			return this->_useCount != 0;
		}
		bool isDestroyed() {
			LockerTmpl<LO>(this);
			return this->_useCount == 0;
		}
		virtual void destroy() {
			LockerTmpl<LO>(this);
			S_ASSERT(this->_useCount == 0);
			delete this;
		}
		unsigned int getUseCount() {
			LockerTmpl<LO>(this);
			return this->_useCount;
		}
		/** The object won't be ever deleted automatically. Use ONLY for heap allocated objects! */
		void disableRefCount() {
			_useCount = 0;
		}

	private:
		/** Dopóki obiekt nie jest niszczony ma _useCount >= 1 */
		volatile unsigned int _useCount;
	};


};	

#endif