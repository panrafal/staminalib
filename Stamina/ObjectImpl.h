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
	@param LS - lock selector class to use (needs function selectLock()), or Lock object descendant (will be used for all types of locks...)
	*/
	template <class OI, class LS = Stamina::CriticalSection, class OIMP = Object<OI> > class LockableObject: public OIMP {
	public:

		virtual Lock& selectLock(enLockType type) const {
			__if_exists(LS::selectLock) {
				return const_cast<LockableObject*>(this)->_lock.selectLock(type);
			}
			__if_not_exists(LS::selectLock) {
				return const_cast<LockableObject*>(this)->_lock;
			}
		}

		LS & lockSelector() {return _lock;}
	private:
		LS _lock;
	};


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
			ObjLocker(this, lockSharedMutex);
			if (this->_useCount == 0)
				return false;
			this->_useCount++;
			return true;
		}
		void release() {
			{
				ObjLocker(this, lockSharedMutex);
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
			ObjLocker(this, lockSharedMutex);
			return this->_useCount != 0;
		}
		bool isDestroyed() {
			ObjLocker(this, lockSharedMutex);
			return this->_useCount == 0;
		}
		virtual void destroy() {
			ObjLocker(this, lockSharedMutex);
			S_ASSERT(this->_useCount == 0);
			delete this;
		}
		unsigned int getUseCount() {
			ObjLocker(this, lockSharedMutex);
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