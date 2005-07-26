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
		// Here is where the magic goes. 'quick' holds an reference to data, as long as you don't try to modify it. If You modify do, a local copy is allocated - so the referenced string is still safe and left unchanged.
		quick += ref;
		printf("Local modified copy: %s", quick.a_str());

		return StringPassRef(quick); // StringPassRef safely passes our local buffer outside of the function... Once again, _no copy is being made_ !
	}

	@endcode

	The STL equivalent would be:
	@code
	std::string fun(const std::string& ref, std::string quick) {
		...
	}
	@endcode

	We have a copy of the return value, @a quick always makes a copy here, @ref is passing true references only if we are using std::string. In case of fun("bla", "ble") - we make _three_ redundant copies at every function call! When using StringRef and StringPassRef you don't make them at all!

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


	public:

		class StringPassRef: public StringRef {
		public:
			StringPassRef() {
			}
			StringPassRef(const StringRef& str) {
				this->swapBuffers(const_cast<StringRef&>(str));
			}
		};

		StringRefCP() {}

		StringRefCP(const StringPassRef& pass) {
			this->swapBuffers(const_cast<StringPassRef&>(pass));
		}

		StringRefCP(const StringRef& str) {
			assignCheapReference(str);
		}
		StringRefCP(const char* ch, unsigned size = lengthUnknown) {
			assignCheapReference(ch, size);
		}
		StringRefCP(const wchar_t* ch, unsigned size = lengthUnknown) {
			assignCheapReference(ch, size);
		}

#ifdef _STRING_
		StringRefCP(const std::string& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
		StringRefCP(const std::wstring& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
#endif
#ifdef STDSTRING_H
		StringRefCP(const CStdStringA& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
		StringRefCP(const CStdStringW& ch) {
			assignCheapReference(ch.c_str(), ch.size());
		}
#endif

		void makeUnique() {
			if (_w.isReference()) {
				_w.makeUnique();
			}
			if (_a.isReference()) {
				_a.makeUnique();
			}
		}

		// ------ Operators

		inline StringRef& operator = (const StringRef& b) {
			this->assign(b);
			return *this;
		}

		inline StringRef& operator = (const StringPassRef& pass) {
			this->swapBuffers(const_cast<StringRef&>(str));
		}


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
			return a;
		}

		inline StringRef& operator += (const StringRef& b) {
			this->append(b);
			return *this;
		}

		inline StringRef operator - (const StringRef& b) const {
			StringRef a (*this);
			a.replace(b, "");
			return a;
		}

		inline StringRef& operator -= (const StringRef& b) const {
			this->replace(b, "");
			return *this;
		}

		inline bool operator & (const StringRef& b) const {
			return this->find(b) != notFound;
		}

		// ------ Ansi Unicode buffers

		inline const TCHAR* c_str() const {
			return str<TCHAR>();
		}

		template <typename CHAR>
		inline const CHAR* str() const {
			prepareType<CHAR>();
			return getData<CHAR>();
		}

		/** Returns the pointer to ANSI data. The buffer is always valid */
		inline const char* a_str() const {
			return str<char>();
		}

		/** Returns the pointer to UNICODE data. The buffer is always valid */
		inline const wchar_t* w_str() const {
			return str<wchar_t>();
		}

		template <typename CHAR>
		const StringBuffer<CHAR>& getDataBuffer() const;

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
			return StringRef(getData<CHAR>(), getDataSize<CHAR>());
		}


		template <typename CHAR>
		CHAR* useBuffer(unsigned int size = lengthUnknown) {
			if (sizeof(CHAR) == sizeof(wchar_t)) {
				if (isMBLocked()) {
					S_DEBUG_ERROR( "stringIsMBLocked" );
					return 0;
				}
				_w.setActive(true);
			} else {
				_w.setActive(false);
			}
			if (size < lengthUnknown) {
				getDataBuffer<CHAR>().makeRoom(size, 0);
			}
			return getDataBuffer<CHAR>().getBuffer();
		}

		template <typename CHAR>
		void releaseBuffer(unsigned int length = lengthUnknown) {
			getDataBuffer<CHAR>().setLength(length);
			getDataBuffer<CHAR>().markValid();
			changed();
		}

		// ------ character access

		template <typename CHAR>
		inline unsigned int getDataSize() const {
			return getDataBuffer<CHAR>().getLength();
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
		inline const CHAR* getDataEnd(unsigned int size = -1) const {
			if (size > getDataBuffer<CHAR>().getLength()) size = getDataBuffer<CHAR>().getLength();
			return getDataBuffer<CHAR>().getString() + size;
		}

		inline const void* getData() const {
			if (isWide())
				return (void*)_w.getString();
			else
				return (void*)_a.getString();
		}


		inline unsigned int getLength() const {
			if (empty()) return 0;
			if (isWide()) {
				return StringType<wchar_t, CP>::getLength(getData<wchar_t>(), getDataEnd<wchar_t>());
			} else {
				return StringType<char, CP>::getLength(getData<char>(), getDataEnd<char>());
			}
			/*TODO*/
		}

		inline bool empty() const {
			return getDataSize() == 0;
		}

		template <typename CHAR>
		inline unsigned int getDataPos(int charPos) const {
			return StringType<CHAR, CP>::getDataPos(getData<CHAR>(), getDataEnd<CHAR>(), charPos);
		}
		template <typename CHAR>
		inline unsigned int getCharPos(unsigned int dataPos) const {
			return StringType<CHAR, CP>::getCharPos(getData<CHAR>(), getDataEnd<CHAR>(), dataPos);
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

		inline StringRef substr(int start) const {
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

		inline StringRef substr(int start, unsigned int count) const {
			count = getDataPos(start + count);
			start = getDataPos(start);
			if ((unsigned)start > count) {
				return StringRef();
			}
			count -= start;
			StringRef str;
			str.forceType(isWide());
			if (isWide()) {
				str._w.assignCheapReference( getData<wchar_t>() + start , count );
				str._w.truncate(count);
			} else {
				str._a.assignCheapReference( getData<char>() + start , count );
				str._a.truncate(count);
			}
			return StringPassRef( str );
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
				return StringType<wchar_t, CP>::find(it, getDataEnd<wchar_t>(count), find.getData<wchar_t>(), find.getDataEnd<wchar_t>(count), ignoreCase, skip).getFoundPosition(getDataEnd<wchar_t>(count));
			} else {
				StringType<char, CP>::ConstIterator it (getData<char>() + getDataPos<char>(start), start);
				return StringType<char, CP>::find(it, getDataEnd<char>(count), find.getData<char>(), find.getDataEnd<char>(count), ignoreCase, skip).getFoundPosition(getDataEnd<char>(count));
			}
		}

		inline unsigned int findLast(const StringRef& find, int start = 0, unsigned int count = lengthUnknown, bool ignoreCase = false) const {
			return this->find(find, start, ignoreCase, -1, count);
		}

		// ------ basic modification

		inline void assign(const StringRef& str) {
			clear();
			matchTypes(str);
			if (isWide())
				_w.assign(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.assign(str.getData<char>(), str.getDataSize<char>());
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
			if (str.getDataBuffer<wchar_t>().isValid())
				_w.assignCheapReference(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
//			else
			if (str.getDataBuffer<char>().isValid())
				_a.assignCheapReference(str.getData<char>(), str.getDataSize<char>());
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
			_a.swap(str._a);
			_w.swap(str._w);
		}

		inline void append(const StringRef& str) {
			matchTypes(str);
			if (isWide())
				_w.append(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.append(str.getData<char>(), str.getDataSize<char>());
			this->changed();
		}

		inline void prepend(const StringRef& str) {
			matchTypes(str);
			if (isWide())
				_w.prepend(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.prepend(str.getData<char>(), str.getDataSize<char>());
			this->changed();
		}

		inline void insert(unsigned int pos, const StringRef& str) {
			matchTypes(str);
			if (isWide())
				_w.insertInRange(getDataPos<wchar_t>(pos), str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.insertInRange(getDataPos<char>(pos), str.getData<char>(), str.getDataSize<char>());
			this->changed();
		}

		inline void erase(unsigned int pos, unsigned int count = lengthUnknown) {
			pos = getDataPos(pos);
			if (isWide()) {
				_w.erase(pos, getDataPos<wchar_t>(pos + count) - pos);
			} else {
				_a.erase(pos, getDataPos<wchar_t>(pos + count) - pos);
			}
			this->changed();
		}

		inline void replace(unsigned int pos, const StringRef& str, unsigned int count = lengthUnknown) {
			matchTypes(str);
			if (count > this->getLength()) count = this->getLength();
			if (isWide()) {
				pos = getDataPos<wchar_t>(pos);
				_w.replace(pos, getDataPos<wchar_t>(pos + count) - pos, str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			} else {
				pos = getDataPos<char>(pos);
				_w.replace(pos, getDataPos<char>(pos + count) - pos, str.getData<char>(), str.getDataSize<char>());
			}
			this->changed();
		}


		inline void clear() {
			_w.discard();
			_a.discard();
			if (!isTypeLocked()) {
				_w.setActive(false);
			}
		}

		// ------ more modification

		inline unsigned int replace(const StringRef& find, const StringRef& replace,  int start, bool ignoreCase = false, unsigned int limit = lengthUnknown, unsigned int count = lengthUnknown) {
			matchTypes(replace);
			unsigned c = 0;
			while (limit--) {
				unsigned int found = this->find(find, start, count, ignoreCase);
				if (found == notFound) {
					break;
				}
				this->replace(found, replace, find.getLength());
				c++;
			}
			this->changed();
			return c;
		}

		/** Replaces characters found in @a from with according characters in @to (or with last character @a to, or removes them) 

		@warning You CAN'T use character pairs in from/to values if @a to representation takes more bytes per character than @a from (ie. all non-ANSI characters in UTF-8)! Force @a this, @a from or @to to use Wide type instead.
		
		It's always safe to erase (when @a to is empty) multi byte characters.
		It's always safe to replace characters into smaller types (ie. special language-specific characters into ANSI)
		*/
		inline void replaceChars(const StringRef& from, const StringRef& to, bool ignoreCase = false, bool keepCase = false, bool swapMatch = false, unsigned int limit = -1, unsigned int count = lengthUnknown) {
			matchTypes(str);
			if (isWide()) {
				_w.setLength(
					StringType<wchar_t, CP>::replaceChars(getData<wchar_t>(), getDataEnd<wchar_t>(count), from.getData<wchar_t>(), from.getDataEnd<wchar_t>(), to.getData<wchar_t>(), to.getDataEnd<wchar_t>(), ignoreCase, swapMatch, keepCase, limit) );
				_w.markValid();
			} else {
				_a.setLength(
					StringType<char, CP>::replaceChars(getData<char>(), getDataEnd<char>(count), from.getData<char>(), from.getDataEnd<char>(), to.getData<char>(), to.getDataEnd<char>(), ignoreCase, swapMatch, keepCase, limit) );
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
				if (_w.isValid() || _a.isValid() == false) return;
				noconst->_w.makeRoom( MultiByteToWideChar(CP::codePage(), 0, noconst->_a.getString(), noconst->_a.getLength(), 0, 0), 0);
				noconst->_w.setLength( MultiByteToWideChar(CP::codePage(), 0, noconst->_a.getString(), noconst->_a.getLength(), noconst->_w.getBuffer(), noconst->_w.getBufferSize()));
				noconst->_w.markValid();
			} else {
				if (_a.isValid() || _w.isValid() == false) return;
				noconst->_a.makeRoom( WideCharToMultiByte(CP::codePage(), 0, noconst->_w.getString(), noconst->_w.getLength(), 0, 0, 0, 0), 0);
				noconst->_a.setLength( WideCharToMultiByte(CP::codePage(), 0, noconst->_w.getString(), noconst->_w.getLength(), noconst->_a.getBuffer(), noconst->_a.getBufferSize(), 0, 0));
				noconst->_a.markValid();
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
			if (str.isWide() == this->isWide()) return;
			if (str.isWide()) { 
				if (isTypeLocked()) { // wymuszamy u nas MB - przystosowujemy str
					str.prepareType(false);
					return;
				} else {
					this->forceType(true);
					return;
				}
			}
			if (this->isWide()) str.prepareType(true);
		}

	protected:

		/** Marks that active buffer has changed. The conversion buffer is then discarded */
		inline void changed() {
			if (isWide()) {
				_a.discard();
			} else {
				_w.discard();
			}
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

		template <typename CHAR>
		inline unsigned int getKnownLength() const {
			return getDataBuffer<CHAR>().getKnownLength();
		}

	protected:

		StringBuffer<char> _a;
		StringBuffer<wchar_t> _w;

	};


	template <class CP>
	class StringCP: public StringRefCP<CP> {
	public:
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

		inline StringCP(const StringPassRef& pass):StringRefCP<CP>(pass) {
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
	typedef StringRefCP<cpACP>::StringPassRef StringPassRef;
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