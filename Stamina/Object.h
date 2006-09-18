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

#ifndef __STAMINA_OBJECT__
#define __STAMINA_OBJECT__

#if defined(_DEBUG) && !defined(STAMINA_DEBUG)
#define STAMINA_DEBUG
#endif

#ifndef _WINDOWS_
	#define WIN32_LEAN_AND_MEAN
	#define _WIN32_WINNT 0x0500
	#include <windows.h>
#endif




#include "Assert.h"
#include "Version.h"
#include "Memory.h"
#include "ObjectClassInfo.h"
#include "ObjectPtr.h"

namespace Stamina {

#ifdef STAMINA_DEBUG
	extern volatile long debugObjectsCount;
#endif

#define STAMINA_OBJECT_CLONEABLE()\
	const static bool isCloneable = true;\
	virtual iObject* cloneObject() const {\
		iObject* obj = new ObjectClass();\
		obj->cloneMembers(this);\
		return obj;\
	}




	class String; // forward declaration
	class iStringFormatter;

	/** Basic object interface */
	class iObject {
	public:

#ifdef STAMINA_DEBUG

		iObject() {
			InterlockedIncrement(&debugObjectsCount);
				}
		virtual ~iObject() {
			InterlockedDecrement(&debugObjectsCount);
				}
#else 
		iObject() {
		}

		virtual ~iObject() {};
#endif


		/** Returns object's class information */
		virtual const ObjectClassInfo& getClass() const {
			return staticClassInfo();
		}

		virtual String toString(iStringFormatter* format=0) const;

		virtual iObject* cloneObject() const {
			S_DEBUG_ERROR(L"not cloneable!");
			return 0;
		}

	public:

		virtual void cloneMembers(const iObject* b) {
		}

	public:

		/** Static class information */
		static const ObjectClassInfo& staticClassInfo() {
			static const ObjectClassInfo oci = ObjectClassInfo("iObject", sizeof(iObject), ObjectClassInfo::none, Version(1,0,0,0));
			return oci;
		}

/*		bool isFromCurrentLibInstance() {
			return this->getClass().getLibInstance() == LibInstance::get();
		}
		bool isSameLibInstance(const iObject& obj) {
			return this->getClass().getLibInstance() == obj.getClass().getLibInstance();
		}*/

		template <class TO> TO* castStaticObject() {
			return static_cast<TO*>(this);
		}

		template <class TO> TO* castStaticObject() const {
			return static_cast<TO*>(this);
		}

		template <class TO> TO* castObject() {
			if (this->getClass() >= TO::staticClassInfo()) {
				return reinterpret_cast<TO*>(this);
			} else {
				return 0;
			}
		}

		template <class TO> TO* tryCastObject() throw (...);

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

		void *operator new( size_t size) {
			return Memory::malloc(size);
		}

		void *operator new( size_t size, void* ptr) {
			return ptr;
		}

		void operator delete( void * buff ) {
			Memory::free(buff);
		}

		void operator delete( void * buff, void* ) {
			return;
		}


	private:

		virtual void zzPlaceHolder_iObject1() {}
		virtual void zzPlaceHolder_iObject2() {}
		virtual void zzPlaceHolder_iObject3() {}
		virtual void zzPlaceHolder_iObject4() {}
		virtual void zzPlaceHolder_iObject5() {}

	};


}; // kompletne podstawy....


#include "String.h"
#include "Lock.h"
#include "LockSelector.h"

#ifdef STAMINA_DEBUG
	#include <list>
	#include <vector>
	#include "CriticalSection.h"
#endif




// dokañczamy....
namespace Stamina {

#ifdef STAMINA_DEBUG
	typedef std::list<class iObject*> tDebugObjects;
	extern tDebugObjects* debugObjects;
	//extern CriticalSection* debugObjectsCS;
	extern Lock* debugObjectsCS;
	extern volatile long debugObjectsCount;
#endif

	/** Interface of lockable objects */
	class iLockableObject: public iObject {
	public:
	    /** Blokuje dostêp do obiektu */
		virtual void lock(enLockType type) const =0;

		/** Odblokowuje dostêp do obiektu */
		virtual void unlock(enLockType type) const =0;

		/** Zwraca obiekt blokuj¹cy o podanym typie, o ile obiekt blokuj¹cy jest zgodny ze Stamina::Lock */
		virtual Lock& selectLock(enLockType type) const {
			return Lock_blank::instance;
		}

		virtual ~iLockableObject() {};

		STAMINA_OBJECT_CLASS_VERSION(iLockableObject, iObject, Version(1,0,0,0));

	private:

		virtual void zzPlaceHolder_iLoObject1() {}
		virtual void zzPlaceHolder_iLoObject2() {}
		virtual void zzPlaceHolder_iLoObject3() {}
		virtual void zzPlaceHolder_iLoObject4() {}
		virtual void zzPlaceHolder_iLoObject5() {}

	};


	/* Scoped locking of iLockable */
	class ObjLocker {
	public:
		__inline ObjLocker(const iLockableObject* lo, enLockType type) {
			_type = type;
			_obj = lo;
			_obj->lock(_type);
		}
		__inline ObjLocker(const iLockableObject& lo, enLockType type) {
			_type = type;
			_obj = &lo;
			_obj->lock(_type);
		}
		__inline ~ObjLocker(){
			_obj->unlock(_type);
		}
	protected:
		const iLockableObject* _obj;
		enLockType _type;
	};


	class iSharedObject: public iLockableObject {
	public:
		virtual bool hold() =0;
		virtual void release() =0;
		/** Returns true when it's safe to use the object */
		virtual bool isValid() =0;
		virtual bool isDestroyed() =0;

		/** Returns number of object instances in use. There are some special meanings.
		0 means that object is being destroyed
		1 means that object was created, but never used (by calling hold())
		1+ means that object is used (by calling hold()) number-1 times

		If getUseCount() returns 2 (used one time), releasing will trigger the destroy proces...

		To make an assertion if object will be destroyed after calling release() use:
		@code
		S_ASSERT(obj->isLastInstance() == 1);
		@endcode
		*/
		virtual unsigned int getUseCount() =0;

		bool isLastInstance() {
			return this->getUseCount() == 2;
		}

		STAMINA_OBJECT_CLASS_VERSION(iSharedObject, iLockableObject, Version(1,0,0,0));


#ifdef STAMINA_DEBUG

		iSharedObject() {
			if (debugObjectsCS) {
				Locker locker(*debugObjectsCS);
				if (debugObjects) {
					debugObjects->push_back(this);
				}
			}
		}
		virtual ~iSharedObject() {
			if (debugObjectsCS) {
				Locker locker (*debugObjectsCS);
				if (debugObjects) {
					debugObjects->remove(this);
				}
			}
		}
#else 
		iSharedObject() {
		}

		virtual ~iSharedObject() {};
#endif

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
