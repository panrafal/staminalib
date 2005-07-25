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
//#include "StringType.h"

namespace Stamina {

	typedef std::string String;

#ifdef UGABUGA

	/** Universal String class. Transparently supports both MultiByte(template specified CorePage) and WideChar (UTF-16) encodings at the same time.
	Default codepage is CP_ACP (default system codepage).

	All positions refer to character positions - ie. in polish word "≥Ûdü" a letter 'd' is in UTF-8 stored at byte 4, however it's real character position is 2
	*/
	template <CP = CP_ACP>
	class String: public iObject {
	public:

		STAMINA_OBJECT_CLASS_VERSION(Stamina::String, iObject, Version(0,1,0,0));

		typedef StringIterator<CP> iterator;
		typedef StringType<char, CP>::Iterator iteratorA;
		typedef StringType<char, CP>::Iterator iteratorW;

		const static unsigned int lengthUnknown = 0x0FFFFFFF;
		const static unsigned int wholeData = 0xFFFFFFFF;
		const static unsigned int npos = 0xFFFFFFFF;
		const static unsigned int notFound = 0xFFFFFFFF;

	public:
		String() {
		}

		template <OTHERCP>
		String(const StringRef<OTHERCP>& b) {
			b.prepareType(true);
			assign(b.getData<wchar_t>(), b.getDataSize<wchar_t>());
		}

		template<>
		String(const StringRef<CP>& str) {
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
		const StringBuffer<CHAR>& getBuffer() const;

		inline template <>	const StringBuffer<char>& getBuffer<char>() const {
			return _a;
		}

		inline template <> const StringBuffer<wchar_t>& getBuffer<wchar_t>() const {
			return _w;
		}

		template <int CODEPAGE = CP>
		inline String<CODEPAGE> getString() const {
			return String<CODEPAGE>(getData<wchar_t>(), getDataSize<wchar_t>());
		}

		template <typename CHAR>
		inline StringRef<CP> getRef() const {
			return StringRef<CP>(getData<CHAR>(), getDataSize<CHAR>());
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
				getBuffer<CHAR>().makeRoom(size, 0);
			}
			return getBuffer<CHAR>().getBuffer();
		}

		template <typename CHAR>
		void releaseBuffer(unsigned int length = lengthUnknown) {
			getBuffer<CHAR>().setLength(length);
			getBuffer<CHAR>().markValid();
			changed();
		}

		// ------ character access

		template <typename CHAR>
		inline unsigned int getDataSize() const {
			getBuffer<CHAR>().getDataSize();
		}

		inline unsigned int getDataSize() const {
			if (isWide())
				return _w.getDataSize();
			else
				return _a.getDataSize();
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
			return getDataSize() > 0;
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
		/** Returns real character position */
		inline unsigned int getCharPos(unsigned int dataPos) {
			if (isWide()) {
				return getCharPos<wchar_t>(dataPos);
			} else {
				return getCharPos<char>(dataPos);
			}
		}

		inline StringRef<CP> substr(int start) const {
			unsigned int count = getDataSize();
			start = getDataPos(start);
			if (start > count) {
				return StringRef<CP>();
			}
			count -= start;
			if (isWide())
				return StringRef<CP>(getData<wchar_t>() + start , count);
			else 
				return StringRef<CP>(getData<char>() + start , count);
		}

		inline String<CP> substr(int start, unsigned int count) const {
			start = getDataPos(start);
			count = getDataPos(start + count);
			if (start > count) {
				return StringRef<CP>();
			}
			count -= start;
			String<CP> str;
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

		inline bool equal(const StringRef<CP>& str, bool ignoreCase = false) const {
			matchConstTypes(str);
			if (isWide()) {
				return StringType<wchar_t, CP>::equal(getData<wchar_t>(), getDataEnd<wchar_t>(), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(), ignoreCase);
			} else {
				return StringType<char, CP>::equal(getData<char>(), getDataEnd<char>(), str.getData<char>(), str.getDataEnd<char>(), ignoreCase);
			}
			/*TODO*/
		}

		inline int compare(const StringRef<CP>& str, bool ignoreCase = false, unsigned int count = lengthUnknown) const {
			matchConstTypes(str);
			if (isWide()) {
				return StringType<wchar_t, CP>::compare(getData<wchar_t>(), getDataEnd<wchar_t>(count), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(count), ignoreCase);
			} else {
				return StringType<char, CP>::compare(getData<char>(), getDataEnd<char>(count), str.getData<char>(), str.getDataEnd<char>(count), ignoreCase);
			}
		}

		// ------ search

		inline unsigned int find(const StringRef<CP>& find, int start, bool ignoreCase = false, int skip = 0, unsigned int count = lengthUnknown) const {
			matchConstTypes(str);
			start = getDataPos(start);
			if (isWide()) {
				return StringType<wchar_t, CP>::find(getData<wchar_t>() + start, getDataEnd<wchar_t>(count), str.getData<wchar_t>(), str.getDataEnd<wchar_t>(count), ignoreCase, skip);
			} else {
				return StringType<char, CP>::find(getData<char>() + start, getDataEnd<char>(count), str.getData<char>(), str.getDataEnd<char>(count), ignoreCase, skip).getFoundPosition();
			}
		}

		inline unsigned int findLast(const StringRef<CP>& find, int start, unsigned int count, bool ignoreCase = false) const {
			return find(find, start, ignoreCase, -1, count);
		}

		// ------ basic modification

		inline void assign(const StringRef<CP>& str) {
			matchTypes(str);
			if (isWide())
				_w.assign(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.assign(str.getData<char>(), str.getDataSize<char>());
			this->changed();
		}

		inline void assignCheapReference(const StringRef<CP>& str) {
			matchTypes(str);
			if (isWide())
				_w.assignCheapReference(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.assignCheapReference(str.getData<char>(), str.getDataSize<char>());
			this->changed();
		}

		inline void assign(const wchar_t* data, unsigned int size = lengthUnknown) {
			forceType(true);
			if (size == lengthUnknown) {
				size = wcslen(data);
			}
			_w.assign(data, size);
			this->changed();
		}


		inline void append(const StringRef<CP>& str) {
			matchTypes(str);
			if (isWide())
				_w.append(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.append(str.getData<char>(), str.getDataSize<char>());
			this->changed();
		}

		inline void prepend(const StringRef<CP>& str) {
			matchTypes(str);
			if (isWide())
				_w.prepend(str.getData<wchar_t>(), str.getDataSize<wchar_t>());
			else
				_a.prepend(str.getData<char>(), str.getDataSize<char>());
			this->changed();
		}

		inline void insert(unsigned int pos, const StringRef<CP>& str) {
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

		inline void replace(unsigned int pos, const StringRef<CP>& str, unsigned int count = lengthUnknown) {
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

		inline unsigned int replace(const StringRef<CP>& find, const StringRef<CP>& replace,  int start, bool ignoreCase = false, unsigned int limit = lengthUnknown, unsigned int count = lengthUnknown) {
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
		inline void replaceChars(const StringRef<CP>& from, const StringRef<CP>& to, bool ignoreCase = false, bool keepCase = false, bool swapMatch = false, unsigned int limit = -1, unsigned int count = lengthUnknown) {
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

		inline unsigned int getCodePage() {
			return CP;
		}

		/** Returns true if String is using Wide (UTF-16) characters */
		inline bool isWide() {
			return _w.isActive();
		}

		/** Returns true if String is locked to MultiByte characters */
		inline bool isMBLocked() {
			return _a.isMajor();
		}

		/** Sets lock on MultiByte characters */
		inline void setMBLock(bool lock) {
			_a.setMajor(lock);
		}

		/** Switches internal buffer to UNICODE or ANSI. */
		inline void forceType(bool wide) {
			if (isMBLocked() && wide) return;
			this->prepareType(wide);
			_w.setActive(wide);
		}

		/** Prepares conversion buffer if it's not valid.
		@warning It's a const function, however it modifies internal buffers! 
		*/
		void prepareType(bool wide) const {
			if (wide == isWide()) return; // mamy juø ten typ na pewno
			String<CP>& noconst = *const_cast<String<CP> >(this);
			if (wide) {
				if (_w.isValid() || _a.isValid() == false) return;
				noconst->_w.makeRoom( MultiByteToWideChar(CP, 0, noconst->_a.getString(), noconst->_a.getDataSize(), 0, 0), 0);
				noconst->_w.setLength( MultiByteToWideChar(CP, 0, noconst->_a.getString(), noconst->_a.getDataSize(), noconst->_w.getBuffer(), noconst->_w.getBufferSize()));
				_w.markValid();
			} else {
				if (_a.isValid() || _w.isValid() == false) return;
				noconst->_a.makeRoom( WideCharToMultiByte(CP, 0, noconst->_w.getString(), noconst->_w.getDataSize(), 0, 0, 0, 0), 0);
				noconst->_a.setLength( WideCharToMultiByte(CP, 0, noconst->_w.getString(), noconst->_w.getDataSize(), noconst->_a.getBuffer(), noconst->_a.getBufferSize(), 0, 0));
				noconst->_a.markValid();
		);
			}
		}

		inline void matchConstTypes(const StringRef<CP>& str) const {
			if (str.isWide() == this->isWide()) return;
			if (str.isWide()) this->prepareType(true);
			if (this->isWide()) str.prepareType(true);
		}

		/** Matches current and string's types choosing the best option.
		If @a this uses MultiByte, and @a str uses Wide - @a this becomes Wide if it's not locked (major), or @a str prepares conversion otherwise.
		If @a this uses Wide, and @a str uses MultiByte - @a str prepares conversion buffer
		If both use the same type, nothing happens
		*/
		inline void matchTypes(const StringRef<CP>& str) {
			if (str.isWide() == this->isWide()) return;
			if (str.isWide()) { 
				if (_a.isMajor()) { // wymuszamy u nas MB - przystosowujemy str
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
		StringBuffer<CHAR>& getBuffer();

		template <>inline StringBuffer<char>& getBuffer<char>() {
			return _a;
		}

		template <> inline StringBuffer<wchar_t>& getBuffer<wchar_t>() {
			return _w;
		}

		template <typename CHAR>
		inline const CHAR* getData() const {
			return getBuffer<CHAR>().getString();
		}

		template <typename CHAR>
		inline const CHAR* getDataEnd(unsigned int size = -1) const {
			if (size > getBuffer<CHAR>().getDataSize()) size = getBuffer<CHAR>().getDataSize();
			return getBuffer<CHAR>().getString() + size;
		}

		template <typename CHAR>
		inline unsigned int getKnownLength() const {
			return getBuffer<CHAR>().getKnownLength();
		}

	protected:

		StringBuffer<char> _a;
		StringBuffer<wchar_t> _w;

	};


#endif

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

}

#endif