/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#ifndef __STAMINA_STRINGBUFFER__
#define __STAMINA_STRINGBUFFER__

#pragma once

#include "Assert.h"
#include "Memory.h"

namespace Stamina {

	/** Character buffer template.
	Implements basic character buffer operations without regard to type, size or codepage.

	Supports:
	 - cheap referencing - operations on external buffer of unknown size
	 - data discard - ability to discard buffer's data without freeing the memory
	 - up to 0x0FFFFFFF (268.435.455) bytes of length
	 - always makes room for \0 char at the string's end. All buffer and data sizes don't count the terminating null character and neither You should do that!

	*/
	template <typename CHAR>
	class StringBuffer {

	protected: // na pocz¹tku, ¿eby by³y dobrze widoczne w debuggerze

		CHAR* _buffer;
		unsigned int _size;
		unsigned int _length;

	public:

//		friend class String;

		const static unsigned int pooledBufferSize = 64;
		const static unsigned int maxBufferSize = 0xFFFFFFFF;
		const static unsigned int lengthUnknown = 0xFFFFFFFF;
		const static unsigned int lengthDiscarded = 0xFFFFFFFE;
		const static unsigned int wholeData = 0xFFFFFFFF;

		class PassBuffer: public StringBuffer<CHAR> {
		public:
			PassBuffer(const StringBuffer<CHAR>& b):StringBuffer<CHAR>() {
				this->swap(const_cast<StringBuffer<CHAR>&>(b));
			}
		};

		class BufferRef: public StringBuffer<CHAR> {
		public:
			BufferRef(const StringBuffer<CHAR>& b):StringBuffer<CHAR>() {
				this->assignCheapReference(b);
			}
		};


	public:
		inline StringBuffer(): _size(0), _buffer(0), _length(0) {
		}

		inline StringBuffer(const StringBuffer& b): _size(0), _buffer(0), _length(0) {
			this->assign(b);
		}

		inline StringBuffer(const PassBuffer& pass): _size(0), _buffer(0), _length(0) {
			this->swap(const_cast<PassBuffer&>(pass));
		}


		inline StringBuffer(unsigned int initialSize): _size(0), _buffer(0), _length(0) {
			resize(initialSize, 0);
		}

		inline StringBuffer(const CHAR* data, unsigned int dataSize = lengthUnknown): _size(0), _buffer(0), _length(0) {
			assignCheapReference(data, dataSize);
		}


		inline ~StringBuffer() {
			freeBuffer();
		}

		inline void swap(StringBuffer<CHAR>& b) {
			CHAR* buffer = this->_buffer;
			int length = this->_length;
			int size = this->_size;
			this->_buffer = b._buffer;
			this->_length = b._length;
			this->_size = b._size;

			b._buffer = buffer;
			b._length = length;
			b._size = size;
			
		}

		/** Creates "cheap reference" - provided buffer will replace the one currently in use, until modification occurs.
		*/
		inline void assignCheapReference(const CHAR* data, unsigned int length = lengthUnknown) {
			//S_ASSERT(data);
			this->reset();
			this->_buffer = (CHAR*)data;
			this->_length = length;
		}

		inline void assignCheapReference(const StringBuffer<CHAR>& b) {
			this->assignCheapReference( b.getBuffer(), b.getLength() );
		}

		/** Makes a copy of data */
		inline void assign(const CHAR* data, unsigned int size) {
//			S_ASSERT(data);
			S_ASSERT(size <= maxBufferSize);
			this->makeRoom(size, 0);
			if (data) {
				copy(_buffer, data, size);
			}
			markValid(size);
		}

		inline void assign(const StringBuffer<CHAR>& b) {
			this->assign( b.getBuffer(), b.getLength() );
		}

		int compare(const StringBuffer<CHAR>& b) {
			int r;
			int l = min((this->getLength()), (b.getLength()));

			r = memcmp(this->getString(), b.getString(), l);
			if (r == 0) {
				if ((this->getLength()) > (b.getLength())) 
					return 1;
				else if ((this->getLength()) < (b.getLength())) 
					return -1;
			}
			return r;
		}

		bool equalBuffers(const StringBuffer<CHAR>& b) {
			return  (_buffer != 0 && _buffer == b._buffer);
		}

		bool operator == (const StringBuffer<CHAR>& b) {
			return compare(b) == 0;
		}

		/** Calculates the number of bytes needed to store @a newSize of data. It only expands current buffer size. */
		inline unsigned int calculateRoom(unsigned int newSize) {
			S_ASSERT(newSize <= maxBufferSize);
			unsigned int allocSize = getBufferSize();
			if (newSize > allocSize) {
				if (allocSize < (maxBufferSize/2)) 
					allocSize *= 2;
			}
			return allocSize > newSize ? allocSize : newSize;
		}

		/** Ensures that there's enough space in the buffer, resizing the buffer if necessary. See resize() for more information.

		@param keepData - the same as in resize()
		@return Returns true if buffor was (re)allocated
		*/
		inline bool makeRoom(unsigned int newSize, unsigned int keepData = wholeData) {
			S_ASSERT(newSize < maxBufferSize);
			if (hasOwnBuffer()) {
				if (newSize > getBufferSize()) {
					this->resize(calculateRoom(newSize), keepData);
					return true;
				}
			} else { // jak nie ma bufora, zawsze alokujemy nowy
				resize(newSize, keepData);
				return true;
			}
			return false;
		}

		/** If the buffer is a reference - make a copy of it 
		@param keepData - the same as in resize()
		*/
		inline void makeUnique(unsigned int keepData = wholeData) {
			if (isReference()) {
				if (keepData > getLength()) keepData = getLength();
				resize(keepData, keepData);
			}
		}

		/** Returns number of used bytes in the buffer */
		inline unsigned int getLength() const {
			if (_length == lengthDiscarded) {
				return 0;
			}
			if (_length == lengthUnknown) {
				const_cast<StringBuffer<CHAR> * >(this)->_length = 0;
				if (!isEmpty() && isValid()) {
					const_cast<StringBuffer<CHAR>* >(this)->_length = len(_buffer);
					//CHAR* ch = _buffer;
					//CHAR* end = isReference() ? (CHAR*)-1 : _buffer + _size;
					//while (ch < end && *(ch++)) const_cast<StringBuffer<CHAR>* >(this)->_length++;
				}
			}
			return _length;
		}
		inline int getKnownLength() const {
			return (signed)_length;
		}

		/** Appends data to the end of the buffer
		*/
		inline void append(const CHAR* data, unsigned int dataSize) {
			if (dataSize == 0) return;
			S_ASSERT(dataSize <= maxBufferSize);
			S_ASSERT(dataSize + getLength() <= maxBufferSize);
			const CHAR* oldBuffer = _buffer;
			makeRoom(getLength() + dataSize);
			S_ASSERT(_buffer != 0);
			S_ASSERT(data != 0);
			S_ASSERT(getBufferSize() >= dataSize + getLength());
			if (data == oldBuffer) {
				data = _buffer;
			}
			copy(_buffer + getLength(), data, dataSize);
			markValid(getLength() + dataSize);
		}

		/** Prepends data to the buffer 
		*/
		inline void prepend(const CHAR* data, unsigned int dataSize) {
			if (dataSize == 0) return;
			unsigned currentLength = getLength();
			S_ASSERT(dataSize <= maxBufferSize);
			S_ASSERT(dataSize + currentLength <= maxBufferSize);
			const CHAR* oldBuffer = _buffer;
			if (isValid()) {
				moveRight(0, dataSize); // wywoluje makeroom
			} else {
				makeRoom(dataSize, 0);
			}
			S_ASSERT(_buffer != 0);
			S_ASSERT(data != 0);
			S_ASSERT(getBufferSize() >= dataSize + currentLength);
			if (data == oldBuffer) {
				data = _buffer + dataSize;
			}
			copy(_buffer, data, dataSize);
			markValid(currentLength + dataSize);
		}

		/** Inserts data into any position in the buffer.
		@param pos The position where to insert the data. Can be beyond currently allocated data!

		@warning This function allows to insert data at virtually any location. It automatically expands the buffer, leaving completely random data between insert position and the end of previous data. Use with caution!
		@warning You CANNOT insert data that comes from the same buffer!
		*/
		inline void insert(unsigned int pos, const CHAR* data, unsigned int dataSize) {
			if (dataSize == 0) return;
			unsigned currentLength = getLength();
			if (pos > maxBufferSize) pos = currentLength;
			S_ASSERT(dataSize <= maxBufferSize);
			S_ASSERT(dataSize + currentLength <= maxBufferSize);
			S_ASSERT(pos + dataSize <= maxBufferSize);
			if (isValid()) {
				if (pos >= getLength()) {
					makeRoom(pos + dataSize);
				} else {
					moveRight(pos, dataSize); // wywoluje makeroom
				}
			} else {
				makeRoom(dataSize, 0);
				pos = 0;
			}
			S_ASSERT(_buffer != 0);
			S_ASSERT(data != 0);
			S_ASSERT(getBufferSize() >= pos + dataSize);
			copy(_buffer + pos, data, dataSize);
			markValid(max(currentLength + dataSize, pos + dataSize));
		}

		inline void insertInRange(unsigned int pos, const CHAR* data, unsigned int dataSize) {
			if (pos > getLength()) pos = getLength();
			insert(pos, data, dataSize);
		}

		/**
		@warning You CANNOT substitute with data that comes from the same buffer!
		*/
		inline void replace(unsigned int pos, unsigned int count, const CHAR* data, unsigned int dataSize) {
			if (dataSize == 0) {
				this->erase(pos, count);
				return;
			}
			if (pos > getLength()) {
				this->append(data, dataSize);
				return;
			}
			if (count == 0) {
				this->insert(pos, data, dataSize);
				return;
			}
			S_ASSERT(data);
			unsigned int currentLength = getLength();
			unsigned int newLength = currentLength;
			if (isReference()) {
				CHAR* from = _buffer;
				if (count > currentLength || pos + count > currentLength) count = currentLength - pos;
				makeRoom(currentLength - count + dataSize, pos);
				S_ASSERT(_buffer);
				if (pos + count < currentLength) { // je¿eli zostaje coœ po...
					S_ASSERT(getBufferSize() >= dataSize + currentLength - count);
					// kopiujemy dane po zmienianym...
					copy(_buffer + pos + dataSize, from + pos + count, currentLength - pos - count);
				}
				newLength = currentLength - count + dataSize;
			} else if (count < currentLength && pos + count < currentLength) {
				if (dataSize > count) {
					moveRight(pos + count, dataSize - count);
				} else if (dataSize < count) {
					moveLeft(pos + count, count - dataSize);
				}
				newLength = lengthUnknown; // moveLeft/Right za³atwia ju¿ markValid
			} else {
				makeRoom(pos + dataSize, pos);
				newLength = pos + dataSize;
			}
			S_ASSERT(getBufferSize() >= pos + dataSize);
			copy(_buffer + pos, data, dataSize);
			if (newLength != lengthUnknown) {
				markValid(newLength);
			}
		}

		inline void erase(unsigned int pos, unsigned int count = wholeData) {
			if (!isValid() || count == 0) return;
			if (count > getLength() || pos + count > getLength()) count = getLength() - pos;
			if (pos + count >= getLength()) {
				truncate(pos);
				return;
			}
			moveLeft(pos + count, count);
		}

		/** Truncates buffer data at the specified position. */
		inline void truncate(unsigned int pos) {
			if (!isValid()) return;
			if (pos > getLength()) pos = getLength();
			if (isReference()) {
				makeUnique(pos);
			}
			this->markValid(pos);
		}


		/** Moves contents of the buffer to the left (optimized). Fails if buffer is not valid.
		@param start Position of the first character to move
		@param offset Offset of movement
		@param length The length of data to move
		@param truncate Truncates buffer after moved data
		*/
		void moveLeft(unsigned int start, unsigned int offset, unsigned int length = wholeData, bool truncate = true) {
			if (!isValid() || offset == 0) return;
			CHAR* from = _buffer;
			unsigned int dataLength = getLength();
			unsigned int newLength = dataLength;
			if (length > dataLength) length = dataLength;
			if (start < offset) {
				if (length > (offset - start)) {
					length -= (offset - start); // musimy skróciæ d³ugoœæ kopiowania o tyle, o ile póŸniej je zaczynamy
				} else {
					length = 0;
				}
				start = offset;
			}
			if (start >= dataLength) {// nie ma sk¹d ich przesuwaæ
				if (truncate) 
					this->truncate(0);
				return;
			} 
			if (length + start > dataLength) {
				length = dataLength - start;
			}
			if (length == 0) {
				if (truncate) 
					this->truncate(start - offset);
				return;
			}
			if (isReference()) {
				// makeUnique bylby z³y bo nie rezerwowa³by bufora na przenoszenie
				resize(truncate ? start - offset + length : dataLength, start - offset); // kopiujemy tylko to co zostanie na pocz¹tku
			}
			CHAR* to = _buffer;

			if (truncate/* || (length + start >= this->_length)*/) {
				newLength = start - offset + length;
			} else {
				// skoro nic nie ucinamy - d³ugoœæ pozostaje bez zmian. Przy zmianie z reference mog³a siê jednak zmieniæ, wiêc przywracamy star¹.
				newLength = dataLength;
				if (getBuffer() != from) { // kopiujemy pozosta³oœci
					S_ASSERT(_size >= (start - offset + length) + dataLength - (start + length - offset));
					copy(to + start - offset + length, from + start - offset + length, dataLength - (start + length - offset));
				}
			}
			S_ASSERT(from);
			S_ASSERT(to);
			S_ASSERT(_size >= start + length);
			from += start;
			to += start - offset;
			move(to, from, length);
			markValid(newLength);
		}

		
		/** Moves contents of the buffer to the right (optimized). Fails if buffer is not valid.
		@param start Position of the first character to move
		@param offset Offset of movement
		@param length Length of data to move
		@param truncate Truncates data beyond moved data
		*/
		void moveRight(unsigned int start, unsigned int offset, unsigned int length = wholeData, bool truncate = true) {
			if (!isValid() || offset == 0) return;
			unsigned int dataLength = getLength();
			if (length > dataLength) length = dataLength;
			if (start > dataLength) return;
			if (length > dataLength - start)
				length = dataLength - start;

			CHAR* from = _buffer;
			if (isReference() && truncate) {
				makeRoom(start + offset + length, start + offset); // potrzebujmey tylko co ma byæ na pocz¹tku...
			} else {
				makeRoom(max(truncate ? 0 : dataLength, start + offset + length)); // potrzebujemy wszystko
				from = _buffer;
			}
			CHAR* to = _buffer;

			S_ASSERT(from != 0);
			S_ASSERT(to != 0);
			S_ASSERT(_size > start + length);

            from += start;
			to += start + offset;

			move(to, from, length);

			markValid(max(truncate ? 0 : dataLength, start + offset + length));

		}

		// -- more internal

		/** Resets the buffer completely (frees all allocated memory). Leaves active and major flags intact */
		inline void reset() {
			freeBuffer();
			_buffer = 0;
			_size = 0;
			_length = lengthDiscarded;
		}

		void resize(unsigned int newSize, unsigned int keepData = wholeData) {
			S_ASSERT(newSize <= maxBufferSize);
			if (keepData && newSize > 0 && this->isValid()) {
				if (keepData > getLength()) keepData = getLength();
				if (keepData > newSize) keepData = newSize;
				unsigned int size = newSize;
				CHAR* buffer = _alloc(size);
				S_ASSERT(buffer != 0);
				S_ASSERT(size >= keepData && size > 0);
				copy(buffer, _buffer, keepData);
				freeBuffer();
				this->_buffer = buffer;
				this->_size = size;
				markValid(keepData);
			} else {
				freeBuffer();
				unsigned int size = newSize;
				if (size > 0) {
					this->_buffer = _alloc(size);
					S_ASSERT(this->_buffer != 0);
					S_ASSERT(size > 0);
				}
				this->_size = size;
				this->_length = lengthDiscarded;
			}
		}

		/** Discards data in buffer, leaving the buffer allocated in memory so we can use it later. */
		inline void discard() {
			if (! this->isReference()) {
				this->_length = lengthDiscarded;
			} else {
				reset();
			}
		}

		// -- static helpers

		static inline void copy(CHAR* to, const CHAR* from, unsigned int count);

		static inline void move(CHAR* to, const CHAR* from, unsigned int count);

		static inline unsigned int len(const CHAR* str);


		// getters

		/** Returns the size of an allocated buffer. If nothing is allocated (ie. data is being referenced) - returns 0 

		@warning Space for \0 character is automatically allocated and not reflected in the buffer's size. So getBuffer()[getBufferSize()] = 0 is perfectly legal.
		*/
		inline unsigned int getBufferSize() const {
			return _size;
		}

		inline CHAR* getBuffer() const {
			return _buffer;
		}

		inline CHAR* getBufferEnd(unsigned size = lengthUnknown) const {
			if (size > getLength()) size = getLength();
			return _buffer + size;
		}


		inline const CHAR* getString() const {
			return isValid() ? _buffer : (CHAR*)L"";
		}

		//

		/** Returns true if data is valid (not discarded) */
		inline bool isValid() const {
			return _buffer != 0 && _length != lengthDiscarded;
		}

		inline bool isReference() const {
			return _size == 0 && _buffer != 0;
		}

		inline bool isEmpty() const {
			return _buffer == 0;
		}

		inline bool hasOwnBuffer() const {
			return _size > 0;
		}

		/** Marks data as valid (if it's not referenced) */
		inline void markValid(unsigned int length) {
			if ( this->hasOwnBuffer() ) {
				_length = length;
				if (this->_length != lengthUnknown) {
					S_ASSERT(_size >= _length && _size > 0);
					S_ASSERT(_buffer != 0);
					this->_buffer[this->_length] = 0;
				}
			}
		}

		inline void markValid() {
			markValid( this->_length == lengthDiscarded ? lengthUnknown : this->_length );
		}

		inline void setLength(unsigned int length) {
			_length = length;
		}


	private:


		inline void freeBuffer() {
			if (hasOwnBuffer()) {
				_free(_buffer, getBufferSize());
			}
			_buffer = 0;
			_size = 0;
		}

		/** Returned memory block is always @a size + 1 of size */
		static CHAR* _alloc(unsigned int &size) {
			return Memory::allocBuffer<CHAR>(size);
		}
		static void _free(CHAR* buff, unsigned int size) {
			Memory::freeBuffer<CHAR>(buff, size);
		}

	};

	inline void StringBuffer<char>::copy(char* to, const char* from, unsigned int count) {
		::memcpy(to, from, count);
	}

	inline void StringBuffer<char>::move(char* to, const char* from, unsigned int count) {
		::memmove(to, from, count);
	}

	inline unsigned int StringBuffer<char>::len(const char* str) {
		return strlen(str);
	}

	inline void StringBuffer<wchar_t>::copy(wchar_t* to, const wchar_t* from, unsigned int count) {
		::wmemcpy(to, from, count);
	}

	inline void StringBuffer<wchar_t>::move(wchar_t* to, const wchar_t* from, unsigned int count) {
		::wmemmove(to, from, count);
	}

	inline unsigned int StringBuffer<wchar_t>::len(const wchar_t* str) {
		return wcslen(str);
	}



	inline void StringBuffer<unsigned char>::copy(unsigned char* to, const unsigned char* from, unsigned int count) {
		::memcpy(to, from, count);
	}

	inline void StringBuffer<unsigned char>::move(unsigned char* to, const unsigned char* from, unsigned int count) {
		::memmove(to, from, count);
	}

	inline unsigned int StringBuffer<unsigned char>::len(const unsigned char* str) {
		return 0;
	}



	template StringBuffer<char>;
	template StringBuffer<wchar_t>;

	typedef StringBuffer<unsigned char> ByteBuffer;

};

#endif