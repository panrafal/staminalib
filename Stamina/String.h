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

The Initial Developer of the Original Code is "STAMINA" - Rafa≥ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa≥ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

*/

#include "Stamina.h"
#include "Object.h"

#ifndef __STAMINA_STRING__
#define __STAMINA_STRING__

#include <string>
//#include "WideChar.h"
#include "Buffer.h"
#include "StringType.h"

namespace Stamina {

	class iString: public iObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(iString, iObject, Version(1,0,0,0));

		virtual unsigned int getCodePage() const =0;

	};

	/** Universal String class. Transparently supports both MultiByte(template specified CodePage) and WideChar (UTF-16) encodings at the same time.
	Default codepage is CP_ACP (default system codepage).

	All positions refer to character positions - ie. in polish word "≥Ûdü" a letter 'd' is in UTF-8 stored at byte 4, however it's real character position is 2

	@warning StringRefT<> is a base class that provides all String functions. However it's constructors do not make copies of data - referencing it instead. StringRef should be used for quick passing of arguments to functions (without copying data). Use class StringT<> in typical situations and in function returns!

	Check the following example:

	@code

	String fun(const StringRef& ref, StringRef quick) {
		// _both_ 'ref' and 'quick' are passed without copying data! It's almost as fast as using direct pointers to data, however leaves you with plenty of functionality to play with. Keep in mind, that conversion buffers are also referenced!
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
	template <class TYPE = stACP>
	class StringRefT: public iString {
	public:

		const static unsigned int lengthUnknown = 0xFFFFFFFF;
		const static unsigned int wholeData = 0xFFFFFFFF;
		const static unsigned int npos = 0xFFFFFFFF;
		const static unsigned int notFound = 0xFFFFFFFF;

		typedef StringRefT<TYPE> StringRef;

		STAMINA_OBJECT_CLASS(StringRefT<TYPE>, iString);

	protected:

		enum Flags {
			flagWide = 1,
			flagTypeLock = 2,
			flagSinglebyte = 1,
		};
		enum Masks {
			maskWide = 4,
			maskMultibyte = 8,
		};

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
		@warning Use with QUADRUPLE caution!
		*/
		class PassStringRef: public StringRef {
		public:
			PassStringRef(const StringRef& str):StringRef() {
				this->swapBuffers(const_cast<StringRef&>(str));
			}
		};

		inline StringRefT() {
			S_ASSERT( TYPE::isWide == false );
			_length = lengthUnknown;
			_flags = 0;
		}

		inline StringRefT(const PassStringRef& pass) {
			_length = lengthUnknown;
			_flags = 0;
			this->swapBuffers(const_cast<PassStringRef&>(pass));
		}

		inline StringRefT(const StringRef& str) {
			_flags = 0;
			copyType(str);
			assignCheapReference(str);
		}

		StringRefT(const iString& str);

		inline StringRefT(const char* ch, unsigned size = lengthUnknown) {
			_flags = 0;
			assignCheapReference(ch, size);
		}
		inline StringRefT(const wchar_t* ch, unsigned size = lengthUnknown) {
			_flags = 0;
			assignCheapReference(ch, size);
		}

		inline StringRefT(char ch) {
			_flags = 0;
			assignCheapReference(&ch, 1);
		}
		inline StringRefT(wchar_t ch) {
			_flags = 0;
			assignCheapReference(&ch, 1);
		}

#ifdef _STRING_
		/*
		template <typename CHAR, typename TRAITS, typename ALLOCATOR>
		StringRefT(const std::basic_string<CHAR, TRAITS, ALLOCATOR>& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
		*/
		inline StringRefT(const std::string& ch) {
			_flags = 0;
			assignCheapReference(ch.c_str(), ch.size());
		}
		inline StringRefT(const std::wstring& ch) {
			_flags = 0;
			assignCheapReference(ch.c_str(), ch.size());
		}
#endif
#ifdef STDSTRING_H
		inline StringRefT(const CStdStringA& ch) {
			_flags = 0;
			assignCheapReference(ch.c_str(), ch.size());
		}
		inline StringRefT(const CStdStringW& ch) {
			_flags = 0;
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
			this->swapBuffers(const_cast<PassStringRef&>(pass));
			//this->swapBuffers(pass._str);
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

		// 

		operator std::string () const {
			return this->a_string();
		}

		operator std::wstring () const {
			return this->w_string();
		}

		inline std::string a_string() const {
			prepareType<char>();
			return std::string(this->getData<char>(), this->getDataSize<char>());
		}

		inline std::string w_string() const {
			prepareType<wchar_t>();
			return std::wstring(this->getData<wchar_t>(), this->getDataSize<wchar_t>());
		}

		//

		virtual String toString(iStringFormatter* format=0) const;

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

		inline unsigned int size() const {
			return getLength();
		}


		// ------ Ansi Unicode buffers


		template <typename CHAR>
		inline const CHAR* str() const {
			prepareType<CHAR>();
			return getData<CHAR>();
		}

		template <typename CHAR>
		const Buffer<CHAR>& getDataBuffer() const;

		template <typename CHAR>
		const Buffer<CHAR>& getCDataBuffer() const {
			return getDataBuffer<CHAR>();
		}

		template <> inline const Buffer<char>& getDataBuffer<char>() const {
			return _a;
		}

		template <> inline const Buffer<wchar_t>& getDataBuffer<wchar_t>() const {
			return _w;
		}

		/*
		template <class CODEPAGE>
		inline StringT<CODEPAGE> getString() const {
			return StringT<CODEPAGE>(getData<wchar_t>(), getDataSize<wchar_t>());
		}
		*/

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

		template <typename CHAR>
		int getBufferSize() {
			return this->getDataBuffer<CHAR>().getBufferSize();
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
			if (size != -1 || start != 0) {
				unsigned int length = getDataBuffer<CHAR>().getLength();
				if (size > length || size + start > length) size = getDataBuffer<CHAR>().getLength() - start;
				return getDataBuffer<CHAR>().getString() + getDataPos( size + start );
			} else {
				return getDataBuffer<CHAR>().getString() + getDataBuffer<CHAR>().getLength();
			}
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
			if (isSinglebyte<CHAR>() == false) {
				if (sizeof( CHAR ) == sizeof( char )) {
					_length = TYPE::getLength(getData<char>(), getDataEnd<char>());
				} else {
					_length = stUNICODE::getLength(getData<wchar_t>(), getDataEnd<wchar_t>());
				}
				setMaskedFlag(flagSinglebyte, getTypeMask<CHAR>(), _length == this->getDataSize<CHAR>() );
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
			if (isSinglebyte<CHAR>() == false) {
				if (sizeof( CHAR ) == sizeof( char )) {
					return TYPE::getDataPos(getData<char>(), getDataEnd<char>(), charPos);
				} else {
					return stUNICODE::getDataPos(getData<wchar_t>(), getDataEnd<wchar_t>(), charPos);
				}
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
			if (isSinglebyte<CHAR>() == false) {
				if (sizeof( CHAR ) == sizeof( char )) {
					return TYPE::getCharPos(getData<char>(), getDataEnd<char>(), dataPos);
				} else {
					return stUNICODE::getCharPos(getData<wchar_t>(), getDataEnd<wchar_t>(), dataPos);
				}
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
			if (matchConstTypes(str) || matchConstCompare(str, ignoreCase)) {
				return stUNICODE::equal(getData<wchar_t>(), getDataEnd<wchar_t>(), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(), ignoreCase);
			} else {
				return TYPE::equal(getData<char>(), getDataEnd<char>(), str.getData<char>(), str.getDataEnd<char>(), ignoreCase);
			}
			/*TODO*/
		}

		inline int compare(const StringRef& str, bool ignoreCase = false, unsigned int count = lengthUnknown) const {
			if (matchConstTypes(str) || matchConstCompare(str, ignoreCase)) {
				return stUNICODE::compare(getData<wchar_t>(), getDataEnd<wchar_t>(count), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(count), ignoreCase);
			} else {
				return TYPE::compare(getData<char>(), getDataEnd<char>(count), str.getData<char>(), str.getDataEnd<char>(count), ignoreCase);
			}
		}

		// ------ search

		inline unsigned int find(const StringRef& find, int start = 0, bool ignoreCase = false, int skip = 0, unsigned int count = lengthUnknown) const {
			if (matchConstTypes(find) || matchConstCompare(find, ignoreCase)) {
				stUNICODE::ConstIterator it (getData<wchar_t>() + getDataPos<wchar_t>(start), start);
				return stUNICODE::find(it, getDataEnd<wchar_t>(count, start), find.getData<wchar_t>(), find.getDataEnd<wchar_t>(), ignoreCase, skip).getFoundPosition(getDataEnd<wchar_t>(count, start));
			} else {
				TYPE::ConstIterator it (getData<char>() + getDataPos<char>(start), start);
				return TYPE::find(it, getDataEnd<char>(count, start), find.getData<char>(), find.getDataEnd<char>(), ignoreCase, skip).getFoundPosition(getDataEnd<char>(count, start));
			}
		}

		inline unsigned int findLast(const StringRef& find, int start = 0, unsigned int count = lengthUnknown, bool ignoreCase = false) const {
			return this->find(find, start, ignoreCase, -1, count);
		}

		inline unsigned int findChars(const StringRef& find, int start = 0, bool ignoreCase = false, int skip = 0, unsigned int count = lengthUnknown) const {
			if (matchConstTypes(find) || matchConstCompare(find, ignoreCase)) {
				stUNICODE::ConstIterator it (getData<wchar_t>() + getDataPos<wchar_t>(start), start);
				return stUNICODE::findChars(it, getDataEnd<wchar_t>(count, start), find.getData<wchar_t>(), find.getDataEnd<wchar_t>(), ignoreCase, skip).getFoundPosition(getDataEnd<wchar_t>(count, start));
			} else {
				TYPE::ConstIterator it (getData<char>() + getDataPos<char>(start), start);
				return TYPE::findChars(it, getDataEnd<char>(count, start), find.getData<char>(), find.getDataEnd<char>(), ignoreCase, skip).getFoundPosition(getDataEnd<char>(count, start));
			}
		}

		inline unsigned int findLastChars(const StringRef& find, int start = 0, unsigned int count = lengthUnknown, bool ignoreCase = false) const {
			return this->findChars(find, start, ignoreCase, -1, count);
		}

		// ------ basic modification

		inline bool equalBuffers(const StringRef& str) {
			return ( this->getDataBuffer<char>().equalBuffers(str.getDataBuffer<char>())
				|| this->getDataBuffer<wchar_t>().equalBuffers(str.getDataBuffer<wchar_t>()));
		}
		inline bool equalActiveBuffers(const StringRef& str) {
			if (isWide()) {
				return this->getDataBuffer<wchar_t>().equalBuffers(str.getDataBuffer<wchar_t>());
			} else {
                return this->getDataBuffer<char>().equalBuffers(str.getDataBuffer<char>());
			}
		}

		inline void assign(const StringRef& str) {
			if (this->equalBuffers(str)) return;
			clear();
			matchTypes(str);
			this->_length = str.getKnownLength() ;
			if (isWide()) {
				_w.assign(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
				setMaskedFlag( flagSinglebyte, maskWide, str.isSinglebyte<wchar_t>() );
			} else {
				_a.assign(str.getData<char>(), str.getDataSize<char>());
				setMaskedFlag( flagSinglebyte, maskMultibyte, str.isSinglebyte<char>() );
			}
			this->changed();
		}

		inline void assign(const wchar_t* data, unsigned int size = lengthUnknown) {
			StringRef ref(data, size);
			assign(ref);
		}

		inline void assignCheapReference(const StringRef& str) {
			if (this->equalBuffers(str)) return;
			clear();
			matchTypes(str);
//			if (isWide())
			this->_length = str.getKnownLength();
			if (str.getDataBuffer<wchar_t>().isValid()) {
				_w.assignCheapReference(str.getData<wchar_t>(), str.getKnownDataSize<wchar_t>());
				setMaskedFlag( flagSinglebyte, maskWide, str.isSinglebyte<wchar_t>() );
			} else {
				//_w.reset();
			}
//			else
			if (str.getDataBuffer<char>().isValid()) {
				_a.assignCheapReference(str.getData<char>(), str.getKnownDataSize<char>());
				setMaskedFlag( flagSinglebyte, maskMultibyte, str.isSinglebyte<char>() );
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
			int flags = this->_flags;
			this->_flags = str._flags;
			str._flags = flags;
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
			if (equalActiveBuffers(str)) {
				StringRef _str (str);
				_str.makeUnique();
				this->insert(pos, _str);
				return;
			}
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

			if (equalActiveBuffers(str)) {
				StringRef _str (str);
				_str.makeUnique();
				this->replace(charPos, _str, count);
				return;
			}

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
				this->setWide(false);
			}
		}

		// ------ more modification

		inline void makeLower() {
			if (getLength() == 0) return;
			if (isWide()) {
				_w.makeUnique();
				stUNICODE::makeLower(_w.getBuffer(), _w.getBufferEnd());
			} else {
				_a.makeUnique();
				TYPE::makeLower(_a.getBuffer(), _a.getBufferEnd());
			}
			this->changed();
		}

		inline void makeUpper() {
			if (getLength() == 0) return;
			if (isWide()) {
				_w.makeUnique();
				stUNICODE::makeUpper(_w.getBuffer(), _w.getBufferEnd());
			} else {
				_a.makeUnique();
				TYPE::makeUpper(_a.getBuffer(), _a.getBufferEnd());
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
			matchCompare(replace, ignoreCase);
			unsigned c = 0;

			if (equalActiveBuffers(find) || equalActiveBuffers(replace)) {
				StringRef _find (find);
				_find.makeUnique();
				StringRef _replace (replace);
				_replace.makeUnique();
				return this->replace(_find, _replace, start, ignoreCase, skip, limit, count);
			}

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

		/** Replaces characters found in @a from with according characters in @a to (or with last character @a to, or removes them) 

		@warning You CAN'T use character pairs in from/to values if @a to representation takes more bytes per character than @a from (ie. all non-ANSI characters in UTF-8)! Force @a this, @a from or @a to to use Wide type instead.
		
		It's always safe to erase (when @a to is empty) multi byte characters.
		It's always safe to replace characters into smaller types (ie. special language-specific characters into ANSI)
		*/
		inline void replaceChars(const StringRef& from, const StringRef& to, bool ignoreCase = false, bool keepCase = false, bool swapMatch = false, unsigned int limit = -1) {
			if (getLength() == 0) return;
			resetKnownLength();
			matchTypes(from);
			matchCompare(from, ignoreCase);

			if (equalActiveBuffers(from), equalActiveBuffers(to)) {
				StringRef _from (from);
				_from.makeUnique();
				StringRef _to (to);
				_to.makeUnique();
				this->replaceChars(_from, _to, ignoreCase, keepCase, swapMatch, limit);
				return;
			}

			if (isWide()) {
				from.prepareType<wchar_t>();
				to.prepareType<wchar_t>();
				_w.makeUnique();
				_w.setLength(
					stUNICODE::replaceChars(_w.getBuffer(), _w.getBufferEnd(), from.getData<wchar_t>(), from.getDataEnd<wchar_t>(), to.getData<wchar_t>(), to.getDataEnd<wchar_t>(), ignoreCase, keepCase, swapMatch, limit).getDataPosition(_w.getBuffer()) );
				_w.markValid();
			} else {
				from.prepareType<wchar_t>();
				to.prepareType<wchar_t>();
				_a.makeUnique();
				_a.setLength(
					TYPE::replaceChars(_a.getBuffer(), _a.getBufferEnd(), from.getData<char>(), from.getDataEnd<char>(), to.getData<char>(), to.getDataEnd<char>(), ignoreCase, keepCase, swapMatch, limit).getDataPosition(_a.getBuffer()) );
				_a.markValid();
			}
			this->changed();
		}

		// ------ type handling

		inline unsigned int getCodePage() const {
			return TYPE::codepage;
		}

		/** Returns true if String is using Wide (UTF-16) characters */
		inline bool isWide() const {
			return _flags & flagWide;
		}

		template<typename CHAR> inline bool isActive() const {
			return isWide() == (sizeof(CHAR) == sizeof(wchar_t));
		}

		template<typename CHAR> inline bool isSinglebyte() const {
			return getMaskedFlag(flagSinglebyte, getTypeMask<CHAR>());
		}

		template<> inline bool isSinglebyte<char>() const {
			return TYPE::constWidth ? true : getMaskedFlag(flagSinglebyte, getTypeMask<CHAR>());
		}
		
		template<> inline bool isSinglebyte<wchar_t>() const {
			return stUNICODE::constWidth ? true : getMaskedFlag(flagSinglebyte, getTypeMask<CHAR>());
		}

		/** Returns true if String is locked to current character type */
		inline bool isTypeLocked() const {
			return (_flags & flagTypeLock) != 0;
		}

		/** Sets lock on character type. The string won't change it's type (ie. if it's Wide now - it'll be it still, even after assignment of MultiByte characters. */
		inline void setTypeLock(bool lock) {
			if (lock) {
				_flags |= flagTypeLock;
			} else {
				_flags &= ~flagTypeLock;
			}
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
			setWide(wide);
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
				matchBytes(maskWide, maskWide);
				return;
			}
			if (str.isWide()) { 
				if (isTypeLocked()) { // wymuszamy u nas MB - przystosowujemy str
 					str.prepareType(false);
					matchBytes(maskMultibyte, maskMultibyte);
					return;
				} else {
					this->forceType(true);
					matchBytes(maskWide, maskWide);
					return;
				}
			}
			if (this->isWide()) {
				str.prepareType(true);
				matchBytes(maskWide, maskWide);
			} else {
				matchBytes(maskMultibyte, maskMultibyte);
			}
		}

		inline bool matchConstCompare(const StringRef& str, bool noCase) const {
			if (noCase == false || TYPE::fullCharProperties == true) return false;
			if (this->isWide() == false && str.isWide() == false) {
				this->prepareType(true);
				str.prepareType(true);
			}
			return true;
		}

		inline void matchCompare(const StringRef& str, bool noCase) {
			if (noCase == false || TYPE::fullCharProperties == true) return;
			if (this->isWide() == false && isTypeLocked() == false) { // wszystkie inne przypadki za≥atwia matchTypes
				this->forceType(true);
				str.prepareType(true);
			}
		}

		inline void matchBytes(Masks a, Masks b) {
			if (getMaskedFlag(flagSinglebyte, b) == false) setMaskedFlag(flagSinglebyte, a, false);
		}

	protected:

		void convertBuffers(bool toUnicode) {
			if (toUnicode) {
				if (_a.isValid() == false) {
					_w.discard();
					return;
				}
				_w.makeRoom( TYPE::convertToWideCharLength(_a.getString(), _a.getLength()), 0);
				_w.setLength( TYPE::convertToWideChar(_a.getString(), _a.getLength(), _w.getBuffer(), _w.getBufferSize()));
				_w.markValid();
				setMaskedFlag(flagSinglebyte, maskWide, false);
			} else {
				if (_w.isValid() == false) {
					_a.discard();
					return;
				}
				_a.makeRoom( TYPE::convertToCharLength(_w.getString(), _w.getLength()), 0);
				_a.setLength( TYPE::convertToChar(_w.getString(), _w.getLength(), _a.getBuffer(), _a.getBufferSize()));
				_a.markValid();
				setMaskedFlag(flagSinglebyte, maskMultibyte, false);
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
			setMaskedFlag(flagSinglebyte, maskWide, false);
			setMaskedFlag(flagSinglebyte, maskMultibyte, false);
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

		// ------ flags

		inline void setWide(bool wide) {
			if (wide) {
				_flags |= flagWide;
			} else {
				_flags &= ~flagWide;
			}
		}

		inline void setMaskedFlag(Flags flag, Masks mask, bool value) {
			if (value) {
				_flags |= flag << mask;
			} else {
				_flags &= ~(flag << mask);
			}
		}

		inline bool getMaskedFlag(Flags flag, Masks mask) const {
			return (_flags & (flag << mask)) == (flag << mask);
		}

		template <typename CHAR> inline Masks getTypeMask()  const;

		template <>	inline Masks getTypeMask<char>() const {
			return maskMultibyte;
		}

		template <>	inline Masks getTypeMask<wchar_t>() const {
			return maskWide;
		}

		// ------ buffer handling

		template <typename CHAR>
		Buffer<CHAR>& getDataBuffer();

		template <>inline Buffer<char>& getDataBuffer<char>() {
			return _a;
		}

		template <> inline Buffer<wchar_t>& getDataBuffer<wchar_t>() {
			return _w;
		}


	protected:

		Buffer<char> _a;
		Buffer<wchar_t> _w;

		unsigned int _length;
		int _flags;

	};


	template <class TYPE>
	class StringT: public StringRefT<TYPE> {
	public:

		STAMINA_OBJECT_CLASS(StringT<TYPE>, StringRefT<TYPE>);

		StringT() {}

		StringT(const StringRefT<TYPE>& str):StringRefT<TYPE>() {
			assign(str);
		}

		StringT(const StringT& str):StringRefT<TYPE>() {
			assign(str);
		}

		inline StringT(const PassStringRef& pass):StringRefT<TYPE>(pass) {
		}

		inline StringT& operator = (const StringRef& b) {
			assign(b);
			return *this;
		}

		inline StringT& operator = (const StringT& b) {
			assign(b);
			return *this;
		}

		inline StringT& operator = (const PassStringRef& pass) {
			StringRefT<TYPE>::operator = (pass);
			return *this;
		}
	};


	class String: public StringRefT<stACP> {
	public:

		STAMINA_OBJECT_CLASS(String, StringRef);

		String() {}

		String(const StringRef& str):StringRefT<stACP>() {
			assign(str);
		}

		String(const String& str):StringRefT<stACP>() { // copy constructor
			assign(str);
		}

		inline String(const PassStringRef& pass):StringRef(pass) {
		}

		inline String& operator = (const String& b) {
			assign(b);
			return *this;
		}

		inline String& operator = (const StringRef& b) {
			assign(b);
			return *this;
		}

		inline String& operator = (const PassStringRef& pass) {
			StringRef::operator = (pass);
			return *this;
		}

	};


/*
	template<typename CP = cpACP>
	class StringCPRef: public StringT<CP>::StringRef {
	public:
		StringCPRef(typename const StringT<CP>::StringRef& str): StringT<CP>::StringRef(str) {
		}
		StringCPRef() {}
	};
*/

	typedef StringRefT<stACP> StringRef;
	typedef StringRefT<stACP>::PassStringRef PassStringRef;
	typedef StringT<stUTF8> StringUTF;
	typedef StringRefT<stUTF8> StringUTFRef;

	inline PassStringRef strRet(StringRef str) {
		return str;
	}

	template<class TYPE>
	inline String StringRefT<TYPE>::toString(iStringFormatter* format=0) const {
		this->prepareType<wchar_t>();
		return String::PassStringRef( String::StringRef( this->str<wchar_t>(), this->getKnownDataSize<wchar_t>() ) );
	}

	template<class TYPE>
	inline StringRefT<TYPE>::StringRefT(const iString& str) {
		_flags = 0;
		String tmp = String::PassStringRef( str.toString() );
		tmp.prepareType<wchar_t>();
		assignCheapReference(tmp.str<wchar_t>(), tmp.getKnownDataSize<wchar_t>());
	}


	/*
	template<typename CP = cpACP>
	class StringAutoRef: public String<CP>::StringRef {
	public:
		StringAutoRef(const String<CP>::StringRef& str) {
		}
	};
	*/

	template<class CharType, class Traits, class TYPE>
	inline std::basic_ostream<CharType, Traits>& operator << (std::basic_ostream<CharType, Traits>& stream,const StringRefT<TYPE>& str) {
		str.prepareType<CharType>();
		stream.write(str.getData<CharType>(), str.getDataSize<CharType>());
		return stream;
	}

	inline String operator+ (const char* a, const StringRef& b) {
		String s = a;
		s += b;
		return PassStringRef( s );
	}
	
	inline String operator+ (const StringRef& a, const char* b) {
		return PassStringRef( a + StringRef(b) );
	}
	
	template<typename _Elem, typename _Traits, typename _Ax>
	inline String operator+ (const std::basic_string<_Elem, _Traits, _Ax> a, const StringRef& b) {
		String s = a;
		return PassStringRef( s + b );
	}

	template<typename _Elem, typename _Traits, typename _Ax>
	inline String operator+ (const StringRef& a, const std::basic_string<_Elem, _Traits, _Ax> b) {
		return PassStringRef( a + StringRef(b) );
	}


	inline String iObject::toString(iStringFormatter* format) const {
		return strRet( this->getClass().getName() );
	}




}

#endif