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

#ifndef __STAMINA_OBJECT__
#define __STAMINA_OBJECT__

#if defined(_DEBUG) && !defined(STAMINA_DEBUG)
#define STAMINA_DEBUG
#endif


#include "Assert.h"

#include "LibInstance.h"

#include "ObjectPtr.h"

#ifdef STAMINA_DEBUG
	#include <list>
	#include "CriticalSection.h"
#endif

namespace Stamina {

#ifdef STAMINA_DEBUG
	extern std::list<class iObject*>* debugObjects;
	extern CriticalSection* debugObjectsCS;
#endif



	class ObjectClassInfo {
	public:
		ObjectClassInfo(const char* name, short size, ObjectClassInfo* base):_name(name),_size(size),_base(base),_uid(0),_libInstance(LibInstance::get()) {
		}
		inline const char * getName() const {
			return _name;
		}
		inline short getSize() const {
			return _size;
		}
		inline ObjectClassInfo* getBaseInfo() const {
			return _base;
		}
		unsigned int getUID();

		inline LibInstance& getLibInstance() const {
			return _libInstance;
		}

		inline bool operator == (ObjectClassInfo& b) {
			return this->getUID() == b.getUID();
		}
		/** Returns true if this object inherits from @a b */
		bool operator >= (ObjectClassInfo& b) {
			if (*this == b) {
				return true;
			} else if (this->getBaseInfo() == 0) {
				return false;
			} else {
				return *this->getBaseInfo() >= b;
			}
		}
		/** Returns true if this object is a base class for @a b */
		inline bool operator <= (ObjectClassInfo& b) {
			return b >= *this;
		}

		/** Looks for @a b class information */
		ObjectClassInfo* getParentInfo (ObjectClassInfo& b) {
			if (*this == b) {
				return this;
			} else if (this->getBaseInfo() == 0) {
				return 0;
			} else {
				return this->getBaseInfo()->getParentInfo(b);
			}
		}

		template <class TYPE> ObjectClassInfo* getParentInfo () {
			return getParentInfo(TYPE::staticClassInfo());
		}

	private:
		unsigned int _uid;
		const char * const _name;
		const short _size;
		ObjectClassInfo* const _base;
		LibInstance& _libInstance;
	};
#define STAMINA_OBJECT_CLASS_DEFINE(TYPE, NAME, BASE) \
	static ::Stamina::ObjectClassInfo& staticClassInfo() {\
	static ::Stamina::ObjectClassInfo oci = ::Stamina::ObjectClassInfo(NAME, sizeof(TYPE), &BASE::staticClassInfo());\
		return oci;\
	}\
	::Stamina::ObjectClassInfo& getClass() const {\
		return staticClassInfo();\
	}

#define STAMINA_OBJECT_CLASS(TYPE, BASE) STAMINA_OBJECT_CLASS_DEFINE(TYPE, #TYPE, BASE)
	

	/** Basic object interface */
	class iObject {
	public:

#ifdef STAMINA_DEBUG
		iObject() {
			if (debugObjectsCS) {
				Locker locker(*debugObjectsCS);
				if (debugObjects) {
					debugObjects->push_back(this);
				}
			}
		}
		virtual ~iObject() {
			if (debugObjectsCS) {
				Locker locker (*debugObjectsCS);
				if (debugObjects) {
					debugObjects->remove(this);
				}
			}
		}
#else 
		virtual ~iObject() {};
#endif


		/** Returns object's class information */
		virtual ObjectClassInfo& getClass() const {
			return staticClassInfo();
		}
		/** Static class information */
		static ObjectClassInfo& staticClassInfo() {
			static ObjectClassInfo oci = ObjectClassInfo("iObject", sizeof(iObject), 0);
			return oci;
		}

		bool isFromCurrentLibInstance() {
			return this->getClass().getLibInstance() == LibInstance::get();
		}
		bool isSameLibInstance(const iObject& obj) {
			return this->getClass().getLibInstance() == obj.getClass().getLibInstance();
		}

		template <class TO> TO* castObject() {
			if (this->getClass() >= TO::staticClassInfo()) {
				return reinterpret_cast<TO*>(this);
			} else {
				return 0;
			}
		}
		template <class TO> TO castObject(TO toClass) {
			if (this->getClass() >= toClass->getClass()) {
				return reinterpret_cast<TO>(this);
			} else {
				return 0;
			}
		}
		template <class TO> TO castObject(TO toClass) const {
			if (this->getClass() >= toClass->getClass()) {
				return reinterpret_cast<TO>(this);
			} else {
				return 0;
			}
		}


	private:

		virtual void zzPlaceHolder_iObject1() {}
		virtual void zzPlaceHolder_iObject2() {}
		virtual void zzPlaceHolder_iObject3() {}
		virtual void zzPlaceHolder_iObject4() {}
		virtual void zzPlaceHolder_iObject5() {}

	};


	/** Interface of lockable objects */
	class iLockableObject: public iObject {
	public:
	    /** Blokuje dostêp do obiektu */
		virtual void __stdcall lock()=0;
		/** Odblokowuje dostêp do obiektu */
		virtual void __stdcall unlock()=0;

		virtual ~iLockableObject() {};

		STAMINA_OBJECT_CLASS(Stamina::iLockableObject, iObject);

	private:

		virtual void zzPlaceHolder_iLoObject1() {}
		virtual void zzPlaceHolder_iLoObject2() {}
		virtual void zzPlaceHolder_iLoObject3() {}
		virtual void zzPlaceHolder_iLoObject4() {}
		virtual void zzPlaceHolder_iLoObject5() {}

	};

	class iSharedObject: public iLockableObject {
	public:
		virtual bool __stdcall hold() =0;
		virtual void __stdcall release() =0;
		/** Returns true when it's safe to use the object */
		virtual bool __stdcall isValid() =0;
		virtual bool __stdcall isDestroyed() =0;
		virtual unsigned int __stdcall getUseCount() =0;
		virtual ~iSharedObject() {};

		STAMINA_OBJECT_CLASS(Stamina::iSharedObject, iLockableObject);

	private:

		virtual void zzPlaceHolder_iShObject1() {}
		virtual void zzPlaceHolder_iShObject2() {}
		virtual void zzPlaceHolder_iShObject3() {}
		virtual void zzPlaceHolder_iShObject4() {}
		virtual void zzPlaceHolder_iShObject5() {}


	};

};

// For compatibility with boost::intrusive_ptr
namespace boost {
	inline void intrusive_ptr_add_ref(Stamina::iSharedObject* p) {
		p->hold();
	}
	inline void intrusive_ptr_release(Stamina::iSharedObject* p) {
		p->release();
	}
};


#endif