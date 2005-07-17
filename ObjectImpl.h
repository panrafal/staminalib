/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa� Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once

#ifndef __STAMINA_OBJECTIMPL__
#define __STAMINA_OBJECTIMPL__


#include "Object.h"
#include "CriticalSection.h"



namespace Stamina {

	/** Basic object implementation */
	template <class OI /**Object interface*/> class Object: public OI {

	};


	/** iLockableObj implementation template
	@param OI - object's interface
	@param OIMP - Object implementation (iObject)
	@param TCS - locker class to use (needs functions lock() and unlock())
	*/
	template <class OI, class OIMP = Object<OI> , class TCS = Stamina::CriticalSection> class LockableObject: public OIMP {
	public:
		void __stdcall lock() {
			_cs.lock();
		}
		/** Odblokowuje dost�p do obiektu */
		void __stdcall unlock() {
			_cs.unlock();
		}
		TCS & CS() {return _cs;}
	private:
		TCS _cs;
	};


	typedef LockerTmpl<iLockableObject> ObjLocker;

	/** iSharedObj implementation
	@param IO - object's interface
	@param LO - Lockable object implementation
	*/
	template <class OI, class LO = LockableObject<OI> /*Locker Obj*/> class SharedObject: public LO {
	public:
		SharedObject() {
			this->_useCount=1; ///< trzyma sam siebie...
		}
		~SharedObject() {
			// je�eli ani razu nie wywo�amy hold/release znaczy �e obiekt by� utworzony i usuni�ty bez u�ycia SharedPtr i ma _useCount == 1
			S_ASSERT(this->_useCount == 0 || this->_useCount == 1);
		}
		bool __stdcall hold() {
			LockerTmpl<LO>(this);
			if (this->_useCount == 0)
				return false;
			this->_useCount++;
			return true;
		}
		void __stdcall release() {
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
		/** Zwraca true je�eli z obiektu mo�na korzysta�
		*/
		bool __stdcall isValid() {
			LockerTmpl<LO>(this);
			return this->_useCount != 0;
		}
		bool __stdcall isDestroyed() {
			LockerTmpl<LO>(this);
			return this->_useCount == 0;
		}
		virtual void __stdcall destroy() {
			LockerTmpl<LO>(this);
			S_ASSERT(this->_useCount == 0);
			delete this;
		}
		unsigned int __stdcall getUseCount() {
			LockerTmpl<LO>(this);
			return this->_useCount;
		}
	private:
		/** Dop�ki obiekt nie jest niszczony ma _useCount >= 1 */
		volatile unsigned int _useCount;
	};


};	

#endif