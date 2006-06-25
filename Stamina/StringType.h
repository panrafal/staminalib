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


#ifndef __STAMINA_STRINGTYPE__
#define __STAMINA_STRINGTYPE__

#pragma once

#include "StringCharTraits.h"

namespace Stamina {


	/** StringType provides static functions for operations on variable width text.
	
	@warning Currently UTF-8 characters > 0x7F can't change case (therefore all operations cannot be always case insensitive). 
	@warning Codepages different than cpACP or cpUTF8 might be handled incorrectly. Only UTF-8 support is implemented. Collating and case of characters is currently handled by the globally selected locale.
	*/
	template<class CT>
	class StringType: public CT {
	public:

		typedef typename CT::tChar tChar;
		typedef tChar CHAR;
		typedef typename CT::tCharacter tCharacter;
		typedef typename CT::tCharacterBuffer tCharacterBuffer;
		const static unsigned notFound = 0xFFFFFFFF;

		class ConstIterator {
		public:
			ConstIterator(const CHAR* ptr, unsigned int position = 0):_p(const_cast<CHAR*>(ptr)),_char(position) {
			}
			ConstIterator():_p(0),_char(notFound) {
			}
			inline void add(unsigned int offset) {
				_char += offset;
				if (CT::constWidth) {
					_p += offset;
				} else {
					while (offset--) {
						_p += charSize();
					}
				}
			}
			inline void sub(unsigned int offset) {
				_char -= offset;
				if (CT::constWidth) {
					_p -= offset;
				} else {
					while (offset--) {
						_p -= prevCharSize();
					}
				}
			}

			inline ConstIterator& operator = (const ConstIterator& b) {
				_p = b._p;
				_char = b._char;
				return *this;
			}

			inline ConstIterator& operator = (const CHAR* ptr) {
				_p = const_cast<CHAR*>(ptr);
				_char = ptr ? 0 : notFound;
				return *this;
			}

			inline ConstIterator& operator ++ () {
				add(1);
				return *this;
			}
			inline ConstIterator& operator -- () {
				sub(1);
				return *this;
			}
			inline ConstIterator& operator += (unsigned int offset) {
				add(offset);
				return *this;
			}
			inline ConstIterator& operator -= (unsigned int offset) {
				sub(offset);
				return *this;
			}
			inline int operator - (const ConstIterator& b) {
				return _p - b._p;
			}
			inline int operator + (const ConstIterator& b) {
				return _p + b._p;
			}
			inline ConstIterator operator + (unsigned int offset) {
				ConstIterator it (*this);
				it.add(offset);
				return it;
			}
			inline ConstIterator operator - (unsigned int offset) {
				ConstIterator it (*this);
				it.sub(offset);
				return it;
			}

			inline CHAR operator * () const {
				return *_p;
			}

			inline const CHAR* operator()() const {
				return _p;
			}

			/*
			inline operator const CHAR* () {
				return _p;
			}
			*/

/*			inline operator unsigned int () {
				return getPosition();
			}*/

			inline unsigned int getPosition() const {
				return _char;
			}

			inline void setPosition(unsigned pos) {
				_char = pos;
			}

			inline unsigned int getFoundPosition(const ConstIterator& end) const {
				if (*this == end) return notFound;
				return _char;
			}

			inline unsigned int getDataPosition(const CHAR* start) const {
				return this->_p - start;
			}


			/** Returns full character sequence (usually one byte).
			@warning The sequence might not be null terinated! Use getCharacter() instead.
			*/
			inline tCharacter character() const {
				return CT::getCharacter(_p);
			}

			/** Returns null terminated character sequence (usually one byte).
			@param buff tCharacterBuff to act as a character buffer.
			*/
			inline CHAR* getCharacter(tCharacterBuffer& buff) const {
				buff = character();
				return (CHAR*)&buff;
			}

			inline bool isLower() const {
				return CT::isLower(_p);
			}

			inline bool isUpper() const {
				return CT::isUpper(_p);
			}


			inline tCharacter getLower() const {
				return CT::getLower(_p);
			}

			inline tCharacter getUpper() const {
				return CT::getUpper(_p);
			}

			inline bool operator == (const ConstIterator& b) const {
				return _p == b._p;
			}
			inline bool operator != (const ConstIterator& b) const {
				return _p != b._p;
			}
			inline bool operator >= (const ConstIterator& b) const {
				return _p >= b._p;
			}
			inline bool operator <= (const ConstIterator& b) const {
				return _p <= b._p;
			}
			inline bool operator < (const ConstIterator& b) const {
				return _p < b._p;
			}
			inline bool operator > (const ConstIterator& b) const {
				return _p > b._p;
			}

			inline bool charEq(const ConstIterator&b, bool noCase = false) const {
				return CT::charEq(_p, b._p, noCase);
			}

			inline int charCmp(const ConstIterator& test, bool noCase = false) const {
 				return CT::charCmp(_p, test._p, noCase);
			}

			inline unsigned int charSize() const {
				return CT::charSize(_p);
			}

			inline unsigned int prevCharSize() const {
				return CT::prevCharSize(_p);
			}

			inline static unsigned int charSize(tCharacter ch) {
				return CT::charSize((CHAR*)&ch);
			}

			inline bool canChangeCase() const {
				return CT::canChangeCase(_p);
			}

			inline static bool isWide() {
				return CT::isWide;
			}


		protected:
			CHAR* _p;
			unsigned int _char;
		};


		class Iterator: public ConstIterator {
		public:
			Iterator(CHAR* ptr):ConstIterator(ptr) {
			}
			Iterator() {
			}
			inline Iterator& operator = (const Iterator& b) {
				*(ConstIterator*)this = b;
				return *this;
			}

			inline Iterator& operator = (CHAR* ptr) {
				*(ConstIterator*)this = ptr;
				return *this;
			}

			inline bool set(tCharacter ch) {
				ConstIterator it = (CHAR*)&ch;
				return set(it);
			}

			inline bool set(const ConstIterator& b) {
				//S_ASSERT(charSize() == b.charSize());
				if (charSize() != b.charSize()) return false;
				overwrite(b);
				return true;
			}
			inline void overwrite(const ConstIterator& b) {
				memcpy( _p, b(), b.charSize() * sizeof(CHAR) );
			}
			inline void overwrite(tCharacter ch) {
				ConstIterator it = (CHAR*)&ch;
				overwrite(it);
			}

			inline CHAR& operator * () {
				return *_p;
			}


		};


		inline static unsigned int getLength(const CHAR* from, const CHAR* to) {
			ConstIterator it = from;
			for (; it < to; ++it) {
			}
			return it.getPosition();
		}

		inline static unsigned int getDataPos(const CHAR* from, const CHAR* to, int charPos) {
			if (charPos < 0) {
				charPos = -charPos;
				ConstIterator it = to;
				for (; it > from && charPos > 0; --it, --charPos) {
				}
				return it - from;
			} else {
				ConstIterator it = from;
				for (; it < to && charPos > 0; ++it, --charPos) {
				}
				return it - from;
			}
		}
		
		inline static unsigned int getCharPos(const CHAR* from, const CHAR* to, unsigned int dataPos) {
			ConstIterator it = from;
			const CHAR* aim = from + dataPos;
			for (; it < to; ++it) {
				if (it == aim) break;
				if (it > aim) {
					return it.getPosition() - 1;
				}
			}
			return it.getPosition();
		}


		static bool equal(const CHAR* abegin, const CHAR* aend, const CHAR* bbegin, const CHAR* bend, bool noCase) {
			if (noCase) {
				ConstIterator a = abegin;
				ConstIterator b = bbegin;
				for (; a < aend && b < bend; ++a, ++b) {
					if (!a.charEq(b, noCase)) return false;
				}
				return a == aend && b == bend;
			} else {
				if (aend - abegin != bend - bbegin) return false;
				// dzia³a dla wszystkich, wiêc mo¿e byæ tu u¿yte...
				return memcmp(abegin, bbegin, (aend - abegin) * sizeof(CHAR)) == 0;
			}
		}

		static int compare(const CHAR* abegin, const CHAR* aend, const CHAR* bbegin, const CHAR* bend, bool noCase) {
			ConstIterator a = abegin;
			ConstIterator b = bbegin;
			for (; a < aend && b < bend; ++a, ++b) {
				int res = a.charCmp(b, noCase);
				if (res != 0) return res;
			}
			if (a == aend && b == bend) return 0;
			if (a >= aend) return -1; // a krótszy
			if (b >= bend) return 1;
			return 0; //equal;
		}

		static ConstIterator find(ConstIterator begin, const CHAR* end, const CHAR* findBegin, const CHAR* findEnd, bool noCase, int skip = 0) {
			if (skip == 0) {
				ConstIterator found = findBegin;
				ConstIterator current = begin;
				ConstIterator last;
				while (current < end) {
					if (current.charEq(found, noCase)) {
						if (last == 0) last = current;
						++found;
						if (found >= findEnd) return last;
					} else if (found != findBegin) {
						found = findBegin;
						last = 0;
					}
					++current;
				}
				return current;
			} else {
				return findSkip< StringType<CT> >(begin, end, findBegin, findEnd, noCase, skip, true);
			}
		}

		static ConstIterator findChars(ConstIterator begin, const CHAR* end, const CHAR* findBegin, const CHAR* findEnd, bool noCase, int skip = 0) {
			if (skip == 0) {
				ConstIterator current = begin;
				ConstIterator last;
				while (current < end) {
					ConstIterator found = findBegin;
					while (found < findEnd) {
						if (current.charEq(found, noCase)) {
							return current;
						}
						++found;
					}
					++current;
				}
				return current;
			} else {
				return findSkip< StringType<CT> >(begin, end, findBegin, findEnd, noCase, skip, false);
			}
		}

		template <class STRINGTYPE>
		static ConstIterator findSkip(ConstIterator begin, const CHAR* end, const CHAR* findBegin, const CHAR* findEnd, bool noCase, int skip, bool findString) {
			ConstIterator last;
			ConstIterator current = begin;
			bool findLast = (skip < 0);
			unsigned pos = 0;
			while (findLast || skip-- >= 0) {
				ConstIterator found;
				if (findString) {
					found = STRINGTYPE::find(current, end, findBegin, findEnd, noCase, 0);
				} else {
					found = STRINGTYPE::findChars(current, end, findBegin, findEnd, noCase, 0);
				}
				if (found == end) { // koniec
					if (findLast)
						break;
					else
						return found; // i tak ju¿ wiêcej nie bêdzie...
				}
				current = found;
				++current;
				//unsigned pos = last != 0 ? last.getPosition() : 0;
				last = found;
				//last.setPosition( last.getPosition() + pos ); 
			}
			return last;
		}
         
		static void makeLower(Iterator from, const CHAR* end) {
			while (from < end) {
				from.set(from.getLower());
				++from;
			}
		}

		static void makeUpper(Iterator from, const CHAR* end) {
			while (from < end) {
				from.set(from.getUpper());
				++from;
			}
		}


		static Iterator replaceChars(Iterator str, ConstIterator end, const CHAR* fromBegin, const CHAR* fromEnd, const CHAR* toBegin, const CHAR* toEnd, bool noCase = false, bool keepCase = false, bool swapMatch = false, unsigned int limit = -1) {
			//unsigned length = end - str;
			bool erase = (toBegin == toEnd);
			Iterator current = str;
			ConstIterator from = fromBegin;
			ConstIterator to = toBegin;
			Iterator next = str;
			while (str < end) {
				from = fromBegin;
				++next;
				bool match = false;
				if (swapMatch) { // ³apiemy wszystkie _spoza_ zakresu
					match = limit > 0;
					while (from < fromEnd && match) {
						if (str.charEq(from, noCase)) {
							match = false;
							break;
						}
						++from;
					}
					if (match) {
						limit --;
						if (erase) {
						} else {
							current.overwrite(to);
							++current;
							++to;
							if (to >= toEnd) to = toBegin;
						}
					}
				} else { // wymieniamy znalezione
					to = toBegin;
					while (from < fromEnd && limit > 0) {
						match = str.charEq(from, noCase);
						if (str.charEq(from, noCase)) {
							limit --;
							if (erase) {
							} else {
								if (keepCase) {
									if (str.isLower()) {
										current.overwrite(to.getLower());
									} else {
										current.overwrite(to.getUpper());
									}
								} else {
									current.overwrite(to);
								}
								++current;
							}
							break;
						}
						++from;
						if (!erase && to() + 1 < toEnd) { // utrzymujemy ostatni to
							++to;
						}
					}
				}
				if (!match) { // match juz sie zajal current
					if (current != str) { // przesuwamy znaki
						current.overwrite(str);
					}
					++current;
				}
				str = next;
			}
			return current;
		}


		static inline unsigned int convertToWideCharLength(const char* start, unsigned int length) {
			return MultiByteToWideChar(codepage, 0, start, length, 0, 0);
		}

		static inline unsigned int convertToWideChar(const char* start, unsigned int length, wchar_t* buffer, unsigned int buffSize) {
			return MultiByteToWideChar(codepage, 0, start, length, buffer, buffSize);
		}

		static inline unsigned int convertToCharLength(const wchar_t* start, unsigned int length) {
			return WideCharToMultiByte(codepage, 0, start, length, 0, 0, 0, 0);
		}

		static inline unsigned int convertToChar(const wchar_t* start, unsigned int length, char* buffer, unsigned int buffSize) {
			return WideCharToMultiByte(codepage, 0, start, length, buffer, buffSize, 0, 0);
		}

	};






// ---------------------------------------------------------------------  StringType_ACP

	class StringType_char: public StringType< CharTraits_char > {
	public:

		static bool equal(const char* abegin, const char* aend, const char* bbegin, const char* bend, bool noCase) {
			if (aend - abegin != bend - bbegin) return false;
			if (noCase) {
				return strnicmp(abegin, bbegin, (aend - abegin)) == 0;
			} else {
				// dzia³a dla wszystkich, wiêc mo¿e byæ tu u¿yte...
				return memcmp(abegin, bbegin, (aend - abegin)) == 0;
			}
		}

		static int compare(const char* abegin, const char* aend, const char* bbegin, const char* bend, bool noCase) {
			int r;
			int l = min((aend - abegin), (bend - bbegin));
			if (noCase) {
				r = _strnicoll(abegin, bbegin, l);
			} else {
				r = _strncoll(abegin, bbegin, l);
			}
			if (r == 0) {
				if ((aend - abegin) > (bend - bbegin)) 
					return 1;
				else if ((aend - abegin) < (bend - bbegin)) 
					return -1;
			}
			return r;
		}

	};

	class StringType_wchar: public StringType< CharTraits_wchar > {
	public:

		static bool equal(const wchar_t* abegin, const wchar_t* aend, const wchar_t* bbegin, const wchar_t* bend, bool noCase) {
			if (aend - abegin != bend - bbegin) return false;
			if (noCase) {
				return wcsnicmp(abegin, bbegin, (aend - abegin)) == 0;
			} else {
				// dzia³a dla wszystkich, wiêc mo¿e byæ tu u¿yte...
				return memcmp(abegin, bbegin, (aend - abegin) * 2) == 0;
			}
		}

		static int compare(const wchar_t* abegin, const wchar_t* aend, const wchar_t* bbegin, const wchar_t* bend, bool noCase) {
			int r;
			int l = min((aend - abegin), (bend - bbegin));
			if (noCase) {
				r = _wcsnicoll(abegin, bbegin, l);
			} else {
				r = _wcsncoll(abegin, bbegin, l);
			}
			if (r == 0) {
				if ((aend - abegin) > (bend - bbegin)) 
					return 1;
				else if ((aend - abegin) < (bend - bbegin)) 
					return -1;
			}
			return r;
		}

	};



	typedef StringType_char stACP;
	typedef StringType_wchar stUNICODE;


	class StringType_UTF8: public StringType<CharTraits_UTF8> {
	public:

		static inline unsigned int convertToWideCharLength(const char* start, unsigned int length) {
			return length; // d³u¿szy nie bêdzie
		}

		static inline unsigned int convertToWideChar(const char* start, unsigned int length, wchar_t* buffer, unsigned int buffSize) {
			const UTF8* buff8 = (UTF8*)start;
			UTF16* buff16 = (UTF16*)buffer;
			ConvertUTF8toUTF16(&buff8, buff8 + length, &buff16, buff16 + buffSize, lenientConversion);
			return buff16 - (UTF16*)buffer;
		}

		static inline unsigned int convertToCharLength(const wchar_t* start, unsigned int length) {
			return length * 2; // raczej d³u¿szy nie bêdzie...
		}

		static inline unsigned int convertToChar(const wchar_t* start, unsigned int length, char* buffer, unsigned int buffSize) {
			UTF8* buff8 = (UTF8*)buffer;
			const UTF16* buff16 = (UTF16*)start;
			ConvertUTF16toUTF8(&buff16, buff16 + length, &buff8, buff8 + buffSize, lenientConversion);
			return buff8 - (UTF8*)buffer;
		}


	};

	typedef StringType_UTF8 stUTF8;

}

#endif