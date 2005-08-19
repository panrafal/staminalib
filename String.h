/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa≥ Lindemann, Stamina
 *
 *  $Id$
 */

#ifndef __STAMINA_STRING__
#define __STAMINA_STRING__

#pragma once

#include <string>
#include "WideChar.h"
#include "StringBuffer.h"
#include "StringType.h"

namespace Stamina {

#ifdef TEST_STRING

	class iString: public iObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::iString, iObject, Version(0,1,0,0));

		virtual unsigned int getCodePage() const =0;

	};

	/** Universal String class. Transparently supports both MultiByte(template specified CorePage) and WideChar (UTF-16) encodings at the same time.
	Default codepage is CP_ACP (default system codepage).

	All positions refer to character positions - ie. in polish word "≥Ûdü" a letter 'd' is in UTF-8 stored at byte 4, however it's real character position is 2

	@warning StringRefCP<> is a base class that provides all String functions. However it's constructors do not make copies of data - referencing it instead. StringRef should be used for quick passing of arguments to functions (without copying data). Use class StringCP<> in typical situations and in function returns!

	Check the following example:

	@code

	String fun(const StringRef& ref, StringRef quick) {
		// _both_ 'ref' and 'quick' are passed without copying data! It's almost as fast as using direct pointers to data, however leaves you with plenty of function to play with. Keep in mind, that conversion buffers are also referenced!
		printf("Referenced const ref: %s", ref.a_str());
		printf("Referenced quick: %s", quick.a_str());
		// Here is where the magic goes. 'quick' holds an reference to data, as long as you don't try to modify it. If You do modify, a local copy is allocated - so the referenced string is still safe and left unchanged.
		quick += ref;
		printf("Local modified copy: %s", quick.a_str());

		return PassStringRef(quick); // PassStringRef safely passes our local buffer outside of the function... Once again, _no copy is being made_ !
	}

	@endcode

	The STL equivalent would be:
	@code
	std::string fun(const std::string& ref, std::string quick) {
		...
	}
	@endcode

	We have a copy of the return value, @a quick always makes a copy here, @ref is passing true references only if we are using std::string. In case of fun("bla", "ble") - we make _three_ redundant copies at every function call! When using StringRef and PassStringRef you don't make them at all!

	Remember not to use StringRef as a return type from functions, unless you really know what You're doing! Use String instead, with PassStringRef where appropriate.

	*/
	template <class CP = cpACP>
	class StringRefCP: public iString {
	public:

//		typedef StringIterator<CP> iterator;
//		typedef StringType<char, CP>::Iterator iteratorA;
//		typedef StringType<wchar_t, CP>::Iterator iteratorW;

		const static unsigned int lengthUnknown = 0x0FFFFFFF;
		const static unsigned int wholeData = 0xFFFFFFFF;
		const static unsigned int npos = 0xFFFFFFFF;
		const static unsigned int notFound = 0xFFFFFFFF;

		typedef StringRefCP<CP> StringRef;

		STAMINA_OBJECT_CLASS(Stamina::StringRefCP<CP>, iString);


	public:

		/** Proxy class for passing buffers without copying.

		Passing of buffers is made by swapping them. Be aware, that after assigning a String to PassStringRef, the String won't hold the same value any more (it will contain the value of PassStringRef).

		This technique can be used in two ways, always if you don't want to make a copy of the value. A typical example would be:
		@code
		String a;
		...
		a = PassStringRef(StringRef("Hello"));
		@endcode
		Assigning "Hello" directly to @a a would make a copy. As "Hello" is a const static buffer, it's more perfomance-wise to not make a copy of it.

		Be aware however, that buffers are exchanged!:
		@code
		String a = "Hello";
		String b = "Blah";
		a = PassStringRef( b );
		S_ASSERT( a == "Blah" ); // True
		S_ASSERT( a == b ); // Not true!!!
		S_ASSERT( b == "Hello" ); // True
		@endcode

		The another use of PassStringRef is returning String values:
		@code
		String fun(StringRef str) {
			if (str == "Bye")
				return PassStringRef(str);
			else
				return PassStringRef(StringRef("Hello"));
		}
		@endcode

		Again, we are not making any copies here, however you'll have to preserve caution when using this. Two examples of dangerous code:
		@code
		String ex1(String& str) {
			return PassStringRef(str);
		}
		String ex2() {
			static String example = "Example";
			return PassStringRef(example);
		}

		String a = "Hello";
		S_ASSERT( ex1(a) == "Hello" ); // true
		S_ASSERT( ex1(a) == "Hello" ); // FALSE!
		S_ASSERT( ex2() == "Example" ); // true
		S_ASSERT( ex2() == "Example" ); // FALSE!

		@endcode
		In both situations we are exchanging buffers with external Strings making them invalid!
		The best way to use it safely is to declare static Strings and function arguments as const. Then it just won't compile.

		@warning Don't use it as a variable type! It should be used only as a proxy!
		*/
		class PassStringRef {
		public:
			friend class StringRef;

			PassStringRef(StringRef& str):_str(str) {
				//str->swapBuffers(const_cast<StringRef&>(str));
			}
		private:
			StringRef& _str;
		};

		inline StringRefCP() {
			_length = lengthUnknown;
		}

		inline StringRefCP(PassStringRef& pass) {
			this->swapBuffers(pass._str);
		}

		inline StringRefCP(const StringRef& str) {
			copyType(str);
			assignCheapReference(str);
		}
		inline StringRefCP(const char* ch, unsigned size = lengthUnknown) {
			assignCheapReference(ch, size);
		}
		inline StringRefCP(const wchar_t* ch, unsigned size = lengthUnknown) {
			assignCheapReference(ch, size);
		}

#ifdef _STRING_
		/*
		template <typename CHAR, typename TRAITS, typename ALLOCATOR>
		StringRefCP(const std::basic_string<CHAR, TRAITS, ALLOCATOR>& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
		*/
		inline StringRefCP(const std::string& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
		inline StringRefCP(const std::wstring& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
#endif
#ifdef STDSTRING_H
		inline StringRefCP(const CStdStringA& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
		inline StringRefCP(const CStdStringW& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
#endif

		StringRef& makeUnique() {
			if (_w.isReference()) {
				_w.makeUnique();
			}
			if (_a.isReference()) {
				_a.makeUnique();
			}
			return *this;
		}

		// ------ Operators

		inline StringRef& operator = (const StringRef& b) {
			this->assign(b);
			return *this;
		}

		inline StringRef& operator = (const PassStringRef& pass) {
			//this->swapBuffers(const_cast<StringRef&>(str));
			this->swapBuffers(pass._str);
			return *this;
		}
/*
		inline StringRef& operator = (const char* ch) {
			assign(ch);
			return *this;
		}

		inline StringRef& operator = (const wchar_t* ch) {
			assign(ch);
			return *this;
		}

#ifdef _STRING_
		inline StringRef& operator = (const std::string& ch) {
			assign(ch.c_str(), ch.size());
			return *this;
		}
		inline StringRef& operator = (const std::wstring& ch) {
			assign(ch.c_str(), ch.size());
			return *this;
		}
#endif
#ifdef STDSTRING_H
		inline StringRef& operator = (const CStdStringA& ch) {
			assign(ch.c_str(), ch.size());
			return *this;
		}
		inline StringRef& operator = (const CStdStringW& ch) {
			assign(ch.c_str(), ch.size());
			return *this;
		}
#endif
*/


		inline bool operator == (const StringRef& b) const {
			return this->equal(b);
		}
		inline bool operator != (const StringRef& b) const {
			return this->equal(b) == false;
		}

		inline bool operator > (const StringRef& b) const {
			return this->compare(b) > 0;
		}

		inline bool operator < (const StringRef& b) const {
			return this->compare(b) < 0;
		}

		inline bool operator >= (const StringRef& b) const {
			return this->compare(b) >= 0;
		}

		inline bool operator <= (const StringRef& b) const {
			return this->compare(b) <= 0;
		}

		inline StringRef operator + (const StringRef& b) const {
			StringRef a (*this);
			a.append(b);
			return PassStringRef( a );
		}

		inline StringRef& operator += (const StringRef& b) {
			this->append(b);
			return *this;
		}

		inline StringRef operator - (const StringRef& b) const {
			StringRef a (*this);
			a.replace(b, "");
			return PassStringRef( a );
		}

		inline StringRef& operator -= (const StringRef& b) {
			this->replace(b, "");
			return *this;
		}

		inline bool operator & (const StringRef& b) const {
			return this->find(b) != notFound;
		}

		inline bool operator ^ (const StringRef& b) const {
			return this->findChars(b) != notFound;
		}

		// STL like functions

		inline const TCHAR* c_str() const {
			return str<TCHAR>();
		}

		/** Returns the pointer to ANSI data. The buffer is always valid */
		inline const char* a_str() const {
			return str<char>();
		}

		/** Returns the pointer to UNICODE data. The buffer is always valid */
		inline const wchar_t* w_str() const {
			return str<wchar_t>();
		}

		inline bool empty() const {
			return getDataSize() == 0;
		}

		inline unsigned int length() const {
			return getLength();
		}



		// ------ Ansi Unicode buffers


		template <typename CHAR>
		inline const CHAR* str() const {
			prepareType<CHAR>();
			return getData<CHAR>();
		}

		template <typename CHAR>
		const StringBuffer<CHAR>& getDataBuffer() const;

		template <typename CHAR>
		const StringBuffer<CHAR>& getCDataBuffer() const {
			return getDataBuffer<CHAR>();
		}

		template <> inline const StringBuffer<char>& getDataBuffer<char>() const {
			return _a;
		}

		template <> inline const StringBuffer<wchar_t>& getDataBuffer<wchar_t>() const {
			return _w;
		}

		/*
		template <class CODEPAGE>
		inline StringCP<CODEPAGE> getString() const {
			return StringCP<CODEPAGE>(getData<wchar_t>(), getDataSize<wchar_t>());
		}
		*/

		template <typename CHAR>
		inline StringRef getRef() const {
			return StringRef(*this);
		}


		template <typename CHAR>
		CHAR* useBuffer(unsigned int size = lengthUnknown) {
			clear();
			forceType<CHAR>();
			if (size >= lengthUnknown) 
				size = getDataBuffer<CHAR>().getBufferSize();
			getDataBuffer<CHAR>().makeRoom(size, 0); // wywola makeUnique w razie potrzeby
			resetKnownLength();
			return getDataBuffer<CHAR>().getBuffer();
		}

		template <typename CHAR>
		void releaseBuffer(unsigned int length = lengthUnknown) {
			getDataBuffer<CHAR>().setLength(length);
			getDataBuffer<CHAR>().markValid();

			// sprawdzamy czy bufor tego typu jest aktywny. Jeøeli nie konwertujemy/przenosimy dane do aktywnego...
			if (! this->isActive<CHAR>()) {
				this->convertBuffers(this->isWide());
			} else {
				this->changed();
			}

		}

		// ------ character access

		template <typename CHAR>
		inline unsigned int getDataSize() const {
			return getDataBuffer<CHAR>().getLength();
		}

		template <typename CHAR>
		inline unsigned int getKnownDataSize() const {
			return getDataBuffer<CHAR>().getKnownLength();
		}

		inline unsigned int getDataSize() const {
			if (isWide())
				return _w.getLength();
			else
				return _a.getLength();
		}

		template <typename CHAR>
		inline const CHAR* getData() const {
			return getDataBuffer<CHAR>().getString();
		}

		template <typename CHAR>
		inline const CHAR* getDataEnd(unsigned int size = -1, unsigned int start = 0) const {
			unsigned int length = getDataBuffer<CHAR>().getLength();
			if (size > length || size + start > length) size = getDataBuffer<CHAR>().getLength() - start;
			return getDataBuffer<CHAR>().getString() + size + start;
		}

		inline const void* getData() const {
			if (isWide())
				return (void*)_w.getString();
			else
				return (void*)_a.getString();
		}


		protected:
		template <typename CHAR>
		inline unsigned int getLength() {
			if (getDataBuffer<CHAR>().isVarbyte()) {
				_length = StringType<CHAR, CP>::getLength(getData<CHAR>(), getDataEnd<CHAR>());
				getDataBuffer<CHAR>().setVarbyte( _length != this->getDataSize<CHAR>() );
			} else {
				_length = this->getDataSize<CHAR>();
			}
			return _length;
		}

		public:
		inline unsigned int getLength() const {
			if (empty()) return 0;
			if (_length == lengthUnknown) {
				StringRef* noconst = const_cast<StringRef*>(this);
				if (isWide()) {
					return noconst->getLength<wchar_t>();
				} else {
					return noconst->getLength<char>();
				}
			}
			return _length;
		}

		template <typename CHAR>
		inline unsigned int getDataPos(int charPos) const {
			if (getDataBuffer<CHAR>().isVarbyte()) {
				return StringType<CHAR, CP>::getDataPos(getData<CHAR>(), getDataEnd<CHAR>(), charPos);
			} else {
				if (charPos >= 0) {
					return charPos;
				} else {
					if (-(signed)charPos > (signed)getLength())
						return 0;
					else
						return getLength() + charPos;
				}
			}
		}
		template <typename CHAR>
		inline unsigned int getCharPos(unsigned int dataPos) const {
			if (getDataBuffer<CHAR>().isVarbyte()) {
				return StringType<CHAR, CP>::getCharPos(getData<CHAR>(), getDataEnd<CHAR>(), dataPos);
			} else {
				return dataPos;
			}
		}

		/** Returns position of character in active data buffer */
		inline unsigned int getDataPos(int charPos) const {
			if (isWide()) {
				return getDataPos<wchar_t>(charPos);
			} else {
				return getDataPos<char>(charPos);
			}
		}
		/** Returns real character position in active data buffer */
		inline unsigned int getCharPos(unsigned int dataPos) const {
			if (isWide()) {
				return getCharPos<wchar_t>(dataPos);
			} else {
				return getCharPos<char>(dataPos);
			}
		}

		StringRef substr(int start) const {
			unsigned int count = getDataSize();
			start = getDataPos(start);
			if ((unsigned)start > count) {
				return StringRef();
			}
			count -= start;
			if (isWide())
				return StringRef(getData<wchar_t>() + start , count);
			else 
				return StringRef(getData<char>() + start , count);
		}

		StringRef substr(int start, unsigned int count) const {
			count = getDataPos(start + count);
			start = getDataPos(start);
			if ((unsigned)start > count) {
				return StringRef();
			}
			count -= start;
			StringRef str;
			str.copyType(*this);
			if (isWide()) {
				str._w.assignCheapReference( getData<wchar_t>() + start , count );
				str._w.truncate(count);
			} else {
				str._a.assignCheapReference( getData<char>() + start , count );
				str._a.truncate(count);
			}
			return PassStringRef( str );
		}

		// ------ compare

		inline bool equal(const StringRef& str, bool ignoreCase = false) const {
			if (matchConstTypes(str)) {
				return StringType<wchar_t, CP>::equal(getData<wchar_t>(), getDataEnd<wchar_t>(), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(), ignoreCase);
			} else {
				return StringType<char, CP>::equal(getData<char>(), getDataEnd<char>(), str.getData<char>(), str.getDataEnd<char>(), ignoreCase);
			}
			/*TODO*/
		}

		inline int compare(const StringRef& str, bool ignoreCase = false, unsigned int count = lengthUnknown) const {
			if (matchConstTypes(str)) {
				return StringType<wchar_t, CP>::compare(getData<wchar_t>(), getDataEnd<wchar_t>(count), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(count), ignoreCase);
			} else {
				return StringType<char, CP>::compare(getData<char>(), getDataEnd<char>(count), str.getData<char>(), str.getDataEnd<char>(count), ignoreCase);
			}
		}

		// ------ search

		inline unsigned int find(const StringRef& find, int start = 0, bool ignoreCase = false, int skip = 0, unsigned int count = lengthUnknown) const {
			if (matchConstTypes(find)) {
				StringType<wchar_t, CP>::ConstIterator it (getData<wchar_t>() + getDataPos<wchar_t>(start), start);
				return StringType<wchar_t, CP>::find(it, getDataEnd<wchar_t>(count, start), find.getData<wchar_t>(), find.getDataEnd<wchar_t>(), ignoreCase, skip).getFoundPosition(getDataEnd<wchar_t>(count, start));
			} else {
				StringType<char, CP>::ConstIterator it (getData<char>() + getDataPos<char>(start), start);
				return StringType<char, CP>::find(it, getDataEnd<char>(count, start), find.getData<char>(), find.getDataEnd<char>(), ignoreCase, skip).getFoundPosition(getDataEnd<char>(count, start));
			}
		}

		inline unsigned int findLast(const StringRef& find, int start = 0, unsigned int count = lengthUnknown, bool ignoreCase = false) const {
			return this->find(find, start, ignoreCase, -1, count);
		}

		inline unsigned int findChars(const StringRef& find, int start = 0, bool ignoreCase = false, int skip = 0, unsigned int count = lengthUnknown) const {
			if (matchConstTypes(find)) {
				StringType<wchar_t, CP>::ConstIterator it (getData<wchar_t>() + getDataPos<wchar_t>(start), start);
				return StringType<wchar_t, CP>::findChars(it, getDataEnd<wchar_t>(count, start), find.getData<wchar_t>(), find.getDataEnd<wchar_t>(), ignoreCase, skip).getFoundPosition(getDataEnd<wchar_t>(count, start));
			} else {
				StringType<char, CP>::ConstIterator it (getData<char>() + getDataPos<char>(start), start);
				return StringType<char, CP>::findChars(it, getDataEnd<char>(count, start), find.getData<char>(), find.getDataEnd<char>(), ignoreCase, skip).getFoundPosition(getDataEnd<char>(count, start));
			}
		}

		inline unsigned int findLastChars(const StringRef& find, int start = 0, unsigned int count = lengthUnknown, bool ignoreCase = false) const {
			return this->findChars(find, start, ignoreCase, -1, count);
		}

		// ------ basic modification

		inline void assign(const StringRef& str) {
			clear();
			matchTypes(str);
			this->_length = str.getKnownLength() ;
			if (isWide()) {
				_w.assign(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
				_w.setVarbyte( str.getDataBuffer<wchar_t>().isVarbyte() );
			} else {
				_a.assign(str.getData<char>(), str.getDataSize<char>());
				_a.setVarbyte( str.getDataBuffer<char>().isVarbyte() );
			}
			this->changed();
		}

		inline void assign(const wchar_t* data, unsigned int size = lengthUnknown) {
			StringRef ref(data, size);
			assign(ref);
		}

		inline void assignCheapReference(const StringRef& str) {
			clear();
			matchTypes(str);
//			if (isWide())
			this->_length = str.getKnownLength();
			if (str.getDataBuffer<wchar_t>().isValid()) {
				_w.assignCheapReference(str.getData<wchar_t>(), str.getKnownDataSize<wchar_t>());
				_w.setVarbyte( str.getDataBuffer<wchar_t>().isVarbyte() );
			} else {
				//_w.reset();
			}
//			else
			if (str.getDataBuffer<char>().isValid()) {
				_a.assignCheapReference(str.getData<char>(), str.getKnownDataSize<char>());
				_a.setVarbyte( str.getDataBuffer<char>().isVarbyte() );
			} else {
				//_a.reset();
			}
			// Wykorzystujemy przet≥umaczone bufory ze StringRef
			//this->changed();
		}

		template <typename CHAR>
		inline void assignCheapReference(const CHAR* data, unsigned int size = lengthUnknown) {
			StringRef ref;
			ref.forceType<CHAR>();
			ref.getDataBuffer<CHAR>().assignCheapReference(data, lengthUnknown);
			this->assignCheapReference(ref);
		}

		void swapBuffers(StringRef& str) {
			this->_length = str.getKnownLength();
			_a.swap(str._a);
			_w.swap(str._w);
		}

		inline void append(const StringRef& str) {
			matchTypes(str);
			if (isWide())
				_w.append(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.append(str.getData<char>(), str.getDataSize<char>());
			this->offsetKnownLength(str, 1);
			this->changed();
		}

		inline void prepend(const StringRef& str) {
			matchTypes(str);
			if (isWide())
				_w.prepend(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.prepend(str.getData<char>(), str.getDataSize<char>());
			this->offsetKnownLength(str, 1);
			this->changed();
		}

		inline void insert(unsigned int pos, const StringRef& str) {
			matchTypes(str);
			if (isWide())
				_w.insertInRange(getDataPos<wchar_t>(pos), str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.insertInRange(getDataPos<char>(pos), str.getData<char>(), str.getDataSize<char>());
			this->offsetKnownLength(str, 1);
			this->changed();
		}

		inline void erase(unsigned int charPos, unsigned int count = lengthUnknown) {
			unsigned int pos = getDataPos(charPos);
			if (count > getLength() || count + charPos > getLength()) count = getLength() - charPos;
			if (isWide()) {
				_w.erase(pos, getDataPos<wchar_t>(charPos + count) - pos);
			} else {
				_a.erase(pos, getDataPos<char>(charPos + count) - pos);
			}
			this->offsetKnownLength(- (signed) count);
			this->changed();
		}

		inline void replace(unsigned int charPos, const StringRef& str, unsigned int count = lengthUnknown) {
			matchTypes(str);
			if (count > this->getLength() || charPos + count > this->getLength()) count = this->getLength() - charPos;
			if (isWide()) {
				unsigned int pos = getDataPos<wchar_t>(charPos);
				_w.replace(pos, getDataPos<wchar_t>(charPos + count) - pos, str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			} else {
				unsigned int pos = getDataPos<char>(charPos);
				_a.replace(pos, getDataPos<char>(charPos + count) - pos, str.getData<char>(), str.getDataSize<char>());
			}
			this->offsetKnownLength(str, 1);
			this->offsetKnownLength(-(signed)count);
			this->changed();
		}


		inline void clear() {
			_w.discard();
			_a.discard();
			this->resetKnownLength();
			if (!isTypeLocked()) {
				_w.setActive(false);
			}
		}

		// ------ more modification

		inline void makeLower() {
			if (getLength() == 0) return;
			if (isWide()) {
				_w.makeUnique();
				StringType<wchar_t, CP>::makeLower(_w.getBuffer(), _w.getBufferEnd());
			} else {
				_a.makeUnique();
				StringType<char, CP>::makeLower(_a.getBuffer(), _a.getBufferEnd());
			}
			this->changed();
		}

		inline void makeUpper() {
			if (getLength() == 0) return;
			if (isWide()) {
				_w.makeUnique();
				StringType<wchar_t, CP>::makeUpper(_w.getBuffer(), _w.getBufferEnd());
			} else {
				_a.makeUnique();
				StringType<char, CP>::makeUpper(_a.getBuffer(), _a.getBufferEnd());
			}
			this->changed();
		}

		inline StringRef toLower() const {
			StringRef str(*this);
			str.makeLower();
			return PassStringRef(str);
		}

		inline StringRef toUpper() const {
			StringRef str(*this);
			str.makeUpper();
			return PassStringRef(str);
		}

		inline unsigned int replace(const StringRef& find, const StringRef& replace,  int start = 0, bool ignoreCase = false, int skip = 0, unsigned int limit = lengthUnknown, unsigned int count = lengthUnknown) {
			if (empty()) return 0;
			resetKnownLength();
			matchTypes(replace);
			unsigned c = 0;

			unsigned int found = this->find(find, start, ignoreCase, skip, count);
			if ((found - start) > count) return 0;
			count -= found - start;
			while (limit--) {
				if (found == notFound) {
					break;
				}
				this->replace(found, replace, find.getLength());
				found += replace.getLength();
				if (find.getLength() > count) return c;
				count -= find.getLength();
				c++;
				unsigned int last = found;
				found = this->find(find, found, ignoreCase, 0, count);
				if (found - last > count) return c;
				count -= found - last;
			}
			this->changed();
			return c;
		}

		/** Replaces characters found in @a from with according characters in @to (or with last character @a to, or removes them) 

		@warning You CAN'T use character pairs in from/to values if @a to representation takes more bytes per character than @a from (ie. all non-ANSI characters in UTF-8)! Force @a this, @a from or @to to use Wide type instead.
		
		It's always safe to erase (when @a to is empty) multi byte characters.
		It's always safe to replace characters into smaller types (ie. special language-specific characters into ANSI)
		*/
		inline void replaceChars(const StringRef& from, const StringRef& to, bool ignoreCase = false, bool keepCase = false, bool swapMatch = false, unsigned int limit = -1) {
			if (getLength() == 0) return;
			resetKnownLength();
			matchTypes(from);
			if (isWide()) {
				from.prepareType<wchar_t>();
				to.prepareType<wchar_t>();
				_w.makeUnique();
				_w.setLength(
					StringType<wchar_t, CP>::replaceChars(_w.getBuffer(), _w.getBufferEnd(), from.getData<wchar_t>(), from.getDataEnd<wchar_t>(), to.getData<wchar_t>(), to.getDataEnd<wchar_t>(), ignoreCase, keepCase, swapMatch, limit).getDataPosition(_w.getBuffer()) );
				_w.markValid();
			} else {
				from.prepareType<wchar_t>();
				to.prepareType<wchar_t>();
				_a.makeUnique();
				_a.setLength(
					StringType<char, CP>::replaceChars(_a.getBuffer(), _a.getBufferEnd(), from.getData<char>(), from.getDataEnd<char>(), to.getData<char>(), to.getDataEnd<char>(), ignoreCase, keepCase, swapMatch, limit).getDataPosition(_a.getBuffer()) );
				_a.markValid();
			}
			this->changed();
		}

		// ------ type handling

		inline unsigned int getCodePage() const {
			return CP::codePage();
		}

		/** Returns true if String is using Wide (UTF-16) characters */
		inline bool isWide() const {
			return _w.isActive();
		}

		template<typename CHAR> inline bool isActive() {
			return isWide() == (sizeof(CHAR) == sizeof(wchar_t));
		}

		/** Returns true if String is locked to current character type */
		inline bool isTypeLocked() const {
			return _w.isMajor() || _a.isMajor();
		}

		/** Sets lock on character type. The string won't change it's type (ie. if it's Wide now - it'll be it still, even after assignment of MultiByte characters. */
		inline void setTypeLock(bool lock) {
			if (isWide())
				_w.setMajor(lock);
			else
				_a.setMajor(lock);
		}

		template <typename CHAR>
		inline void forceType() {
			forceType(sizeof(CHAR) == sizeof(wchar_t));
		}

		inline void copyType(const StringRef& b, bool copyLock = false) {
			this->forceType(b.isWide());
			if (copyLock) {
				this->setTypeLock(b.isTypeLocked());
			}
		}


		/** Switches internal buffer to UNICODE or ANSI. */
		inline void forceType(bool wide) {
			if (isTypeLocked()) return;
			this->prepareType(wide);
			_w.setActive(wide);
		}

		template <typename CHAR>
		inline void prepareType() const {
			prepareType(sizeof(CHAR) == sizeof(wchar_t));
		}

		/** Prepares conversion buffer if it's not valid.
		@warning It's a const function, however it modifies internal buffers! 
		*/
		void prepareType(bool wide) const {
			if (wide == isWide()) return; // mamy juø ten typ na pewno
			StringRef* noconst = const_cast<StringRef* >(this);
			if (wide) {
				if (_w.isValid()) return;
				noconst->convertBuffers(true);
			} else {
				if (_a.isValid()) return;
				noconst->convertBuffers(false);
			}
		}


		/** Matches this and string's type choosing the widest possible option. 
		This function doesn't change active type, it only prepares conversion buffers and returns which type we should use (Wide or not).
		*/
		inline bool matchConstTypes(const StringRef& str) const {
			if (str.isWide() == this->isWide()) return isWide();
			if (str.isWide()) {
				this->prepareType(true);
			}
			if (this->isWide()) {
				str.prepareType(true);
			}
			return true;
		}

		/** Matches current and string's types choosing the widest possible option.
		If @a this uses MultiByte, and @a str uses Wide - @a this becomes Wide if it's not type-locked, or @a str prepares conversion otherwise.
		If @a this uses Wide, and @a str uses MultiByte - @a str prepares conversion buffer
		If both use the same type, nothing happens
		*/
		inline void matchTypes(const StringRef& str) {
			if (str.isWide() == this->isWide()) {
				matchBytes(_w, str._w);
				return;
			}
			if (str.isWide()) { 
				if (isTypeLocked()) { // wymuszamy u nas MB - przystosowujemy str
 					str.prepareType(false);
					matchBytes(_a, str._a);
					return;
				} else {
					this->forceType(true);
					matchBytes(_w, str._w);
					return;
				}
			}
			if (this->isWide()) {
				str.prepareType(true);
				matchBytes(_w, str._w);
			} else {
				matchBytes(_a, str._a);
			}
		}

		template <typename A, typename B>
		inline void matchBytes(StringBuffer<A>& a, const StringBuffer<B>& b) {
			if (b.isVarbyte()) a.setVarbyte(true);
		}

	protected:

		void convertBuffers(bool toUnicode) {
			if (toUnicode) {
				if (_a.isValid() == false) {
					_w.discard();
					return;
				}
				_w.makeRoom( MultiByteToWideChar(CP::codePage(), 0, _a.getString(), _a.getLength(), 0, 0), 0);
				_w.setLength( MultiByteToWideChar(CP::codePage(), 0, _a.getString(), _a.getLength(), _w.getBuffer(), _w.getBufferSize()));
				_w.markValid();
				_w.setVarbyte(true);
			} else {
				if (_w.isValid() == false) {
					_a.discard();
					return;
				}
				_a.makeRoom( WideCharToMultiByte(CP::codePage(), 0, _w.getString(), _w.getLength(), 0, 0, 0, 0), 0);
				_a.setLength( WideCharToMultiByte(CP::codePage(), 0, _w.getString(), _w.getLength(), _a.getBuffer(), _a.getBufferSize(), 0, 0));
				_a.markValid();
				_a.setVarbyte(true);
			}

		}


		/** Marks that active buffer has changed. The conversion buffer is then discarded */
		inline void changed() {
			if (isWide()) {
				_a.discard();
			} else {
				_w.discard();
			}
		}

		inline void resetKnownLength() {
			this->_length = lengthUnknown;
			_a.setVarbyte(true);
			_w.setVarbyte(true);
		}

		inline unsigned int getKnownLength() const {
			return this->_length;
		}

		inline void offsetKnownLength(const StringRef& str, int sign) {
			if (str._length == lengthUnknown) {
				_length = lengthUnknown;
				return;
			}
			offsetKnownLength(sign * str._length);
		}

		inline void offsetKnownLength(int offset) {
			if (this->_length == lengthUnknown) return;
			this->_length += offset;
		}

		// ------ buffer handling

		template <typename CHAR>
		StringBuffer<CHAR>& getDataBuffer();

		template <>inline StringBuffer<char>& getDataBuffer<char>() {
			return _a;
		}

		template <> inline StringBuffer<wchar_t>& getDataBuffer<wchar_t>() {
			return _w;
		}

		/*
		template <typename CHAR>
		inline unsigned int getKnownLength() const {
			return getDataBuffer<CHAR>().getKnownLength();
		}
		*/

	protected:

		StringBuffer<char> _a;
		StringBuffer<wchar_t> _w;

		unsigned int _length;

	};


	template <class CP>
	class StringCP: public StringRefCP<CP> {
	public:

		STAMINA_OBJECT_CLASS(Stamina::StringCP<CP>, StringRefCP<CP>);

		StringCP() {}
		/*
		String(const iString& b) {
			if (b.getCodePage() == CP) {
				assign();
			} else {
				b.prepareType(true);
				assign(b.getData<wchar_t>(), b.getDataSize<wchar_t>());
			}
		}*/

		StringCP(const StringRefCP<CP>& str) {
			assign(str);
		}

		inline StringCP(PassStringRef& pass):StringRefCP<CP>(pass) {
		}

		inline StringCP& operator = (const StringRef& b) {
			StringRefCP<CP>::operator = (b);
			return *this;
		}

		inline StringCP& operator = (const PassStringRef& pass) {
			StringRefCP<CP>::operator = (pass);
			return *this;
		}


	};



/*
	template<typename CP = cpACP>
	class StringCPRef: public StringCP<CP>::StringRef {
	public:
		StringCPRef(typename const StringCP<CP>::StringRef& str): StringCP<CP>::StringRef(str) {
		}
		StringCPRef() {}
	};
*/

	typedef StringCP<cpACP> String;
	typedef StringRefCP<cpACP> StringRef;
	typedef StringRefCP<cpACP>::PassStringRef PassStringRef;
	typedef StringCP<cpUTF8> StringUTF;
	typedef StringRefCP<cpUTF8> StringUTFRef;

	/*
	template<typename CP = cpACP>
	class StringAutoRef: public String<CP>::StringRef {
	public:
		StringAutoRef(const String<CP>::StringRef& str) {
		}
	};
	*/

	template<class CharType, class Traits>
	inline std::basic_ostream<CharType, Traits>& operator << (std::basic_ostream<CharType, Traits>& stream,const StringRef& str) {
		str.prepareType<CharType>();
		stream.write(str.getData<CharType>(), str.getDataSize<CharType>());
		return stream;
	}



#else 

	typedef std::string String;


	class StringRef {
	public:
		StringRef(const char* ch):_ch(ch) {}
#ifdef _STRING_
		StringRef(const std::string& ch):_ch(ch.c_str()) {}
#endif
#ifdef STDSTRING_H
		StringRef(const CStdString& ch):_ch(ch.c_str()) {}
#endif

		operator const char*() const {
			return _ch;
		}

		const char* c_str() const {
			return _ch;
		}

		operator const String() const {
			return String(_ch);
		}

	private:
		const char* _ch;
	};


	inline bool operator == (const String& a, const StringRef& b) {
		return stricmp(a.c_str(), b) == 0;
	}


#endif // TEST STRING

}

#endif