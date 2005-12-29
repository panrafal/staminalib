/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: $
 */

#ifndef __STAMINA_ARRAY__
#define __STAMINA_ARRAY__

#pragma once

#include "iArray.h"
#include "ObjectImpl.h"

namespace Stamina {


	template <typename TYPE>
	class Array: public SharedObject< iArray<TYPE> > {
	public:

		typedef Buffer<TYPE> tBuffer;

		Array(unsigned int reserve = 0) {
			this->reserve(reserve);
		}

		/** Returns ObjectClassInfo structure for elements stored in this array, but only if the type supports staticClassInfo(); */
		virtual ObjectClassInfo* getTypeClass() const {
			return getTypeClassInfo<TYPE>();
		}

		/** Returns item count */
		virtual unsigned int size() const {
			return _buffer.getLength();
		}

		/** Reserves memory for newSize elements. 
		Use it before adding many items.
		*/
		virtual void reserve(unsigned int newSize) {
			_buffer.makeRoom(newSize);
		}

		/** How many elements can fit in the array without resizing */
		virtual unsigned int capacity() const {
			return _buffer.getBufferSize();
		}

		virtual void erase(unsigned int pos, unsigned int count = wholeData) {
			_buffer.erase(pos, count);
		}

		virtual TYPE& insert(const TYPE& v, unsigned int pos = wholeData) {
			return this->at(_buffer.insertInRange(pos, &v, 1));
		}

		virtual TYPE& at(unsigned int pos) {
			if (pos >= _buffer.getLength()) {
				throw ExceptionOutOfRange();
			}
			return _buffer.getBuffer()[pos];
		}

	protected:

		tBuffer _buffer;

	};


};


#endif