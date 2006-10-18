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

#ifndef __STAMINA_ARRAY__
#define __STAMINA_ARRAY__

#pragma once

#include "iArray.h"
#include "ObjectImpl.h"

namespace Stamina {


	template <typename TYPE, typename LOCK = Stamina::Lock_blank>
	class Array: public SharedObject< iArray<TYPE>, LockableObject< iArray<TYPE>, LOCK > > {
	public:

		typedef Buffer<TYPE> tBuffer;

		Array(unsigned int reserve = 0) {
			this->reserve(reserve);
		}

		/** Returns ObjectClassInfo structure for elements stored in this array, but only if the type supports staticClassInfo(); */
		virtual const ObjectClassInfo& getTypeClass() const {
			return getTypeClassInfo<TYPE>();
		}

		/** Returns item count */
		virtual unsigned int size() const {
			ObjLocker l(this, lockRead);
			return _buffer.getLength();
		}

		/** Reserves memory for newSize elements. 
		Use it before adding many items.
		*/
		virtual void reserve(unsigned int newSize) {
			ObjLocker l(this, lockWrite);
			_buffer.makeRoom(newSize);
		}

		/** How many elements can fit in the array without resizing */
		virtual unsigned int capacity() const {
			ObjLocker l(this, lockRead);
			return _buffer.getBufferSize();
		}

		virtual void erase(int pos, unsigned int count = wholeData) {
			ObjLocker l(this, lockWrite);
			if (pos == 0 && count >= _buffer.getLength()) {
				_buffer.reset();
			} else {
				_buffer.erase(this->getPos(pos), count);
			}
		}

		virtual TYPE& insert(const TYPE& v, int pos = wholeData) {
			ObjLocker l(this, lockWrite);
			return this->at(_buffer.insertInRange(this->getPosRange(pos), &v, 1));
		}

		virtual TYPE& at(int pos) {
			ObjLocker l(this, lockRead);
			return _buffer.getBuffer()[ this->getPos(pos) ];
		}

		unsigned int getPosRange(int pos) {
			if (pos < 0) {
				return max(0, _buffer.getLength() + pos);
			}
			return min((unsigned)pos, _buffer.getLength());
		}

		unsigned int getPos(int pos) {
			if (pos < 0) {
				pos = _buffer.getLength() + pos;
			}
			if (pos < 0 || (unsigned)pos >= _buffer.getLength()) {
				throw ExceptionOutOfRange();
			}
			return pos;
		}


	protected:

		tBuffer _buffer;

	};


};


#endif