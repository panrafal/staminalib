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

#ifndef __STAMINA_IARRAY__
#define __STAMINA_IARRAY__

#pragma once

#include "Object.h"
#include "Exception.h"

namespace Stamina {

	class iArrayBase: public iSharedObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(iArrayBase, iSharedObject, Version(1,0,0,0));

		class ExceptionOutOfRange: public Exception {
		public:
			STAMINA_OBJECT_CLASS(iArrayBase::ExceptionOutOfRange, Exception);
		private:
		};


		const static unsigned int wholeData = 0x7FFFFFFF;

		/** Returns ObjectClassInfo structure for elements stored in this array, but only if the type supports staticClassInfo(); */
		virtual const ObjectClassInfo& getTypeClass() const =0;

		/** Returns item count */
		virtual unsigned int size() const =0;

		/** Reserves memory for newSize elements. 
		Use it before adding many items.
		*/
		virtual void reserve(unsigned int newSize)=0;

		/** How many elements can fit in the array without resizing */
		virtual unsigned int capacity() const =0;

		/** Erases count items from the array
		@param pos Position. Use negative values to erase values from the end...
		@param count Number od elements to erase. It's trimmed to available count of elements...
		Throws iArrayBase::ExceptionOutOfRange
		*/
		virtual void erase(int pos, unsigned int count = wholeData)=0;

		void clear() {
			this->erase(0);
		}

	};

	template <typename TYPE>
	class iArray: public iArrayBase {
	public:

		typedef SharedPtr< iArray<TYPE> > oArray;

		void assign(const oArray& b) {
			this->clear();
			this->reserve(b->size());
			for (int i=0; i < b->size(); ++i) {
				this->insert(b->at(i));
			}
		}

		/** Inserts an item into the array.
		@param pos You can provide negative position, which starts from the end of the array. Position is always trimmed to the array's size.
		*/
		virtual TYPE& insert(const TYPE& v, int pos = wholeData) = 0;

		TYPE& append(const TYPE& v) {
			return this->insert(v, wholeData);
		}

		TYPE& prepend(const TYPE& v) {
			return this->insert(v, 0);
		}

		/** Returns the element on desired position.
		@param pos Position. Use negative values to query values from the end...
		Throws iArrayBase::ExceptionOutOfRange
		*/
		virtual TYPE& at(int pos) = 0;


		const TYPE& at(int pos) const {
			return (const_cast< iArray<TYPE>* >(this))->at(pos);
		}

		TYPE& operator[] (int pos) {
			return at(pos);
		}

		const TYPE& operator[] (int pos) const {
			return at(pos);
		}

	};

	/** Obiekt tablicy. */
	template <typename TYPE>
	class oArray:public ::Stamina::SharedPtr< iArray<TYPE> > {
	public:
		oArray(const iArray<TYPE> & obj):SharedPtr< iArray<TYPE> >(obj) {}
		oArray(iArray<TYPE> * obj = 0):SharedPtr< iArray<TYPE> >(obj) {}
/*
		TYPE& operator[] (unsigned int pos) {
			S_ASSERT(this->isValid());
			return get()->at(pos);
		}
		const TYPE& operator[] (unsigned int pos) const {
			S_ASSERT(this->isValid());
			return get()->at(pos);
		}
*/
	private:

	};


	STAMINA_REGISTER_CLASS_VERSION(iArrayBase);

};


#endif