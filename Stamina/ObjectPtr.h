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

#include "Object.h"

/** @file Automated iObject pointers

These classes support staticClassInfo() function. To get static information about class inside the pointer - use ObjectClassInfo::getBaseInfo()

*/


namespace Stamina {

	/** Class for handling static objects (no reference counting!, without iSharedObj implemented) */
	template <class T> class StaticObj {
	public:

		STAMINA_OBJECT_CLASS_DEFINE(StaticObj, "StaticObj", T, Version(1,0,0,0));

		StaticObj() {
			_obj.disableRefCount();
		}

		template <typename A1> StaticObj(A1 a1):T(a1) {
			_obj.disableRefCount();
		}

		const T * get() const {
			return &this->_obj;
		}
		T * get() {
			return &this->_obj;
		}

		operator T * () const {
			return &this->_obj;
		}

		operator T * () {
			return &this->_obj;
		}


		const T * operator -> () const {
			return this->get();
		}
		T * operator -> () {
			return this->get();
		}

		const T & operator * () const {
			return *this->get();
		}
		T & operator * () {
			return *this->get();
		}

		bool operator == (const T * b) const {
			return &this->_obj == b;
		}
		bool operator != (const T * b) const {
			return &this->_obj != b;
		}
	private:
		T _obj;

	};


	/** Class for handling static objects (no reference counting!, without iSharedObj implemented) */
	template <class T> class StaticPtr {
	public:

		STAMINA_OBJECT_CLASS_DEFINE(StaticPtr, "StaticPtr", T, Version(1,0,0,0));

		StaticPtr(T & obj) {
			this->set(obj);
		}
		StaticPtr(T * obj = 0) {
			this->set(obj);
		}
		StaticPtr(const StaticPtr & b) {
			this->set(*b);
		}
		StaticPtr & operator = (const StaticPtr & b) {
			this->set(*b);
			return *this;
		}
		void set(T * obj) {
			this->_obj = obj;
		}
		void set(T & obj) {
			set(&obj);
		}
		void set(const StaticPtr & obj) {
			this->set(obj.get());
		}
		void reset() {
			this->set(0);
		}
		T * get() const {
			return this->_obj;
		}
		operator T * () const {
			return this->_obj;
		}
		T * operator -> () const {
			S_ASSERT(this->_obj != 0);
			return this->get();
		}
		T & operator * () const {
			return *this->get();
		}
		/* operator T * za³atwia poni¿sze... */
		/*
		operator bool () const {
			return this->_obj != 0;
		}
		*/
		bool operator ! () const {
			return this->_obj == 0;
		}
		bool isValid() const {
			return this->_obj != 0;
		}
		bool empty() const {
			return this->_obj == 0;
		}
		bool operator == (const T * b) const {
			return this->_obj == b;
		}
		bool operator == (T * const b) const {
			return this->_obj == b;
		}
		bool operator == (const StaticPtr & b) const {
			return this->_obj == b.get();
		}
		bool operator == (bool b) const {
			return (this->_obj != 0) == b;
		}
		bool operator != (const T * b) const {
			return this->_obj != b;
		}
		bool operator != (T * const b) const {
			return this->_obj != b;
		}
		bool operator != (const StaticPtr & b) const {
			return this->_obj != b.get();
		}
		bool operator != (bool b) const {
			return (this->_obj != 0) != b;
		}
	private:
		T * _obj;

		template <class T> friend class SharedPtr;
	};

	/** Class for handling iSharedObject (uses reference counting) */
	template <class T> class SharedPtr: public StaticPtr<T> {
	public:

		STAMINA_OBJECT_CLASS_DEFINE(SharedPtr, "SharedPtr", T, Version(1,0,0,0));

		SharedPtr(T * obj = 0):StaticPtr<T>() {
			this->set(obj);
		}
		SharedPtr(T & obj):StaticPtr<T>() {
			this->set(obj);
		}
		SharedPtr(const SharedPtr & b):StaticPtr<T>() {
			this->set(&*b);
		}
		SharedPtr& operator = (const SharedPtr & b) {
			this->set(&*b);
			return *this;
		}
		~SharedPtr() {
			this->set(0);
		}
		void set(T * obj) {
			if (this->_obj)
				this->_obj->release();
			StaticPtr<T>::set(obj);
			if (this->_obj) {
				this->_obj->hold();
			}
		}
		void set(T & obj) {
			this->set(&obj);
		}
		void set(const SharedPtr & obj) {
			this->set(obj.get());
		}
		void reset() {
			this->set(0);
		}
		T * resetGet() {
			T* tmp = this->_obj;
			S_ASSERT(tmp != 0);
			S_ASSERT(tmp->getUseCount() > 2);
			this->reset();
			return tmp;
		}
	private:
	};


};

namespace boost {

	template<class T> T * get_pointer(const Stamina::StaticPtr<T> & p)
	{
		return p.get();
	}


};
