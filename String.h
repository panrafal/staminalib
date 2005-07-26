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
	*/
	template <class CP = cpACP>
	class StringCP: public iString {
	public:

//		typedef StringIterator<CP> iterator;
//		typedef StringType<char, CP>::Iterator iteratorA;
//		typedef StringType<wchar_t, CP>::Iterator iteratorW;

		const static unsigned int lengthUnknown = 0x0FFFFFFF;
		const static unsigned int wholeData = 0xFFFFFFFF;
		const static unsigned int npos = 0xFFFFFFFF;
		const static unsigned int notFound = 0xFFFFFFFF;

		class StringRef: public StringCP<CP> {
		public:
			StringRef() {}
			StringRef(const StringRef& str) {
				assignCheapReference(str);
			}
			StringRef(const char* ch, unsigned size = lengthUnknown) {
				assignCheapReference(ch, size);
			}
			StringRef(const wchar_t* ch, unsigned size = lengthUnknown) {
				assignCheapReference(ch, size);
			}

#ifdef _STRING_
			StringRef(const std::string& ch) {
				assignCheapReference(ch.c_str(), ch.size());
			}
			StringRef(const std::wstring& ch) {
				assignCheapReference(ch.c_str(), ch.size());
			}
#endif
#ifdef STDSTRING_H
			StringRef(const CStdStringA& ch) {
				assignCheapReference(ch.c_str(), ch.size());
			}
			StringRef(const CStdStringW& ch) {
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

		};

		typedef StringRef Ref;


	public:
		StringCP() {
		}

		/*
		String(const iString& b) {
			if (b.getCodePage() == CP) {
				assign();
			} else {
				b.prepareType(true);
				assign(b.getData<wchar_t>(), b.getDataSize<wchar_t>());
			}
		}*/

		StringCP(const StringCP<CP>& str) {
			assign(str);
		}
		StringCP(const StringRef& str) {
			assign(str);
		}

		// ------ Ansi Unicode buffers

		/** Returns the pointer to ANSI data. The buffer is always valid */
		inline const char* a_str() const {
			prepareType(false);
			return _a.getString();
		}

		/** Returns the pointer to UNICODE data. The buffer is always valid */
		inline const wchar_t* w_str() const {
			prepareType(true);
			return _w.getString();
		}

		template <typename CHAR>
		const StringBuffer<CHAR>& getDataBuffer() const;

		template <> inline const StringBuffer<char>& getDataBuffer<char>() const {
			return _a;
		}

		template <> inline const StringBuffer<wchar_t>& getDataBuffer<wchar_t>() const {
			return _w;
		}

		template <class CODEPAGE>
		inline StringCP<CODEPAGE> getString() const {
			return StringCP<CODEPAGE>(getData<wchar_t>(), getDataSize<wchar_t>());
		}

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
		inline unsigned int getDataPos(int charPos) {
			return StringType<CHAR, CP>::getDataPos(getData<CHAR>(), getDataEnd<CHAR>(), charPos);
		}
		template <typename CHAR>
		inline unsigned int getCharPos(unsigned int dataPos) {
			return StringType<CHAR, CP>::getCharPos(getData<CHAR>(), getDataEnd<CHAR>(), dataPos);
		}

		/** Returns position of character in active data buffer */
		inline unsigned int getDataPos(int charPos) {
			if (isWide()) {
				return getDataPos<wchar_t>(charPos);
			} else {
				return getDataPos<char>(charPos);
			}
		}
		/** Returns real character position in active data buffer */
		inline unsigned int getCharPos(unsigned int dataPos) {
			if (isWide()) {
				return getCharPos<wchar_t>(dataPos);
			} else {
				return getCharPos<char>(dataPos);
			}
		}

		inline StringRef substr(int start) const {
			unsigned int count = getDataSize();
			start = getDataPos(start);
			if (start > count) {
				return StringRef<CP>();
			}
			count -= start;
			if (isWide())
				return StringRef(getData<wchar_t>() + start , count);
			else 
				return StringRef(getData<char>() + start , count);
		}

		inline StringCP<CP> substr(int start, unsigned int count) const {
			start = getDataPos(start);
			count = getDataPos(start + count);
			if (start > count) {
				return StringRef<CP>();
			}
			count -= start;
			StringCP<CP> str;
			str.forceType(isWide());
			if (isWide()) {
				str._w.assignCheapReference( getData<wchar_t>() + start , count );
				str._w.truncate(count);
			} else {
				str._a.assignCheapReference( getData<char>() + start , count );
				str._a.truncate(count);
			}
			return str;
		}

		// ------ compare

		inline bool equal(const StringRef& str, bool ignoreCase = false) const {
			matchConstTypes(str);
			if (isWide()) {
				return StringType<wchar_t, CP>::equal(getData<wchar_t>(), getDataEnd<wchar_t>(), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(), ignoreCase);
			} else {
				return StringType<char, CP>::equal(getData<char>(), getDataEnd<char>(), str.getData<char>(), str.getDataEnd<char>(), ignoreCase);
			}
			/*TODO*/
		}

		inline int compare(const StringRef& str, bool ignoreCase = false, unsigned int count = lengthUnknown) const {
			matchConstTypes(str);
			if (isWide()) {
				return StringType<wchar_t, CP>::compare(getData<wchar_t>(), getDataEnd<wchar_t>(count), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(count), ignoreCase);
			} else {
				return StringType<char, CP>::compare(getData<char>(), getDataEnd<char>(count), str.getData<char>(), str.getDataEnd<char>(count), ignoreCase);
			}
		}

		// ------ search

		inline unsigned int find(const StringRef& find, int start, bool ignoreCase = false, int skip = 0, unsigned int count = lengthUnknown) const {
			matchConstTypes(str);
			start = getDataPos(start);
			if (isWide()) {
				return StringType<wchar_t, CP>::find(getData<wchar_t>() + start, getDataEnd<wchar_t>(count), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(count), ignoreCase, skip);
			} else {
				return StringType<char, CP>::find(getData<char>() + start, getDataEnd<char>(count), str.getData<char>(), str.getDataEnd<char>(count), ignoreCase, skip).getFoundPosition();
			}
		}

		inline unsigned int findLast(const StringRef& find, int start, unsigned int count, bool ignoreCase = false) const {
			return find(find, start, ignoreCase, -1, count);
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

		inline void assignCheapReference(const StringRef& str) {
			clear();
			matchTypes(str);
			if (isWide())
				_w.assignCheapReference(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.assignCheapReference(str.getData<char>(), str.getDataSize<char>());
			this->changed();
		}

		template <typename CHAR>
		inline void assignCheapReference(const CHAR* data, unsigned int size = lengthUnknown) {
			StringRef ref;
			ref.forceType( sizeof(CHAR) == sizeof(wchar_t) );
			ref.getDataBuffer<CHAR>().assignCheapReference(data, lengthUnknown);
			this->assignCheapReference(ref);
		}

		inline void assign(const wchar_t* data, unsigned int size = lengthUnknown) {
			StringRef ref(data, size);
			assign(ref);
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
			_w.reset();
			_a.reset();
			_w.setActive(false);
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

		/** Returns true if String is locked to MultiByte characters */
		inline bool isMBLocked() const {
			return _a.isMajor();
		}

		/** Sets lock on MultiByte characters */
		inline void setMBLock(bool lock) {
			_a.setMajor(lock);
		}

		template <typename CHAR>
		inline void forceType() {
			forceType(sizeof(CHAR) == sizeof(wchar_t));
		}

		/** Switches internal buffer to UNICODE or ANSI. */
		inline void forceType(bool wide) {
			if (isMBLocked() && wide) return;
			this->prepareType(wide);
			_w.setActive(wide);
		}

		template <typename CHAR>
		inline void prepareType() {
			prepareType(sizeof(CHAR) == sizeof(wchar_t));
		}

		/** Prepares conversion buffer if it's not valid.
		@warning It's a const function, however it modifies internal buffers! 
		*/
		void prepareType(bool wide) const {
			if (wide == isWide()) return; // mamy juø ten typ na pewno
			StringCP<CP>* noconst = const_cast<StringCP<CP>* >(this);
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

		inline void matchConstTypes(const StringRef& str) const {
			if (str.isWide() == this->isWide()) return;
			if (str.isWide()) this->prepareType(true);
			if (this->isWide()) str.prepareType(true);
		}

		/** Matches current and string's types choosing the best option.
		If @a this uses MultiByte, and @a str uses Wide - @a this becomes Wide if it's not locked (major), or @a str prepares conversion otherwise.
		If @a this uses Wide, and @a str uses MultiByte - @a str prepares conversion buffer
		If both use the same type, nothing happens
		*/
		inline void matchTypes(const StringRef& str) {
			if (str.isWide() == this->isWide()) return;
			if (str.isWide()) { 
				if (isMBLocked()) { // wymuszamy u nas MB - przystosowujemy str
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
	typedef String::Ref StringRef;
	typedef StringCP<cpUTF8> StringUTF;
	typedef String::Ref StringUTFRef;

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