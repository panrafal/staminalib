/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: String.h 51 2005-07-18 10:13:35Z hao $
 */

#ifndef __STAMINA_STRINGTYPE__
#define __STAMINA_STRINGTYPE__

#pragma once

#include <locale>

namespace Stamina {

	template <unsigned CODEPAGE>
	struct CodePage {
		const static unsigned cp = CODEPAGE;
	};

	typedef CodePage<CP_ACP> cpACP;
	typedef CodePage<CP_UTF8> cpUTF8;

	/** StringType provides static functions for operations on variable width text.
	
	@warning Currently UTF-8 characters > 0x7F can't change case (therefore all operations cannot be always case insensitive). 
	@warning Codepages different than cpACP or cpUTF8 might be handled incorrectly. Only UTF-8 support is implemented. Collating and case of characters is currently handled by the globally selected locale.
	*/
	template<typename CHAR, class CP = cpACP>
	class StringType {
	public:

		typedef unsigned tCharacter;
		typedef unsigned tCharacterBuff;
		const static unsigned notFound = -1;

		static const std::locale& locale() {
			static std::locale loc = std::locale();
			return loc;
		}

		class ConstIterator {
		public:
			ConstIterator(const CHAR* ptr):_p(ptr),_char(0) {
			}
			ConstIterator():_p(0),_char(notFound) {
			}
			inline void add(unsigned int offset) {
				_char += offset;
				while (offset--) {
					_p += charSize();
				}
			}
			inline void sub(unsigned int offset) {
				_char -= offset;
				while (offset--) {
					_p -= prevCharSize();
				}
			}

			inline ConstIterator& operator = (const ConstIterator& b) {
				_p = b._p;
				_char = b._char;
				return *this;
			}

			inline ConstIterator& operator = (const CHAR* ptr) {
				_p = ptr;
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

			/** Returns full character sequence (usually one byte).
			@warning The sequence might not be null terinated! Use getCharacter() instead.
			*/
			inline tCharacter character() const {
				tCharacter ch = 0;
				for (unsigned int i = 0; i < charSize(); i++) {
					((CHAR*)&ch)[i] = _p[i];
				}
				return ch;
			}

			/** Returns null terminated character sequence (usually one byte).
			@param buff tCharacterBuff to act as a character buffer.
			*/
			inline CHAR* getCharacter(tCharacterBuff& buff) const {
				buff = character();
				return (CHAR*)&buff;
			}

			inline tCharacter getLower() const {
				tCharacter ch = character();
				if (canChangeCase()) {
					std::use_facet<std::ctype<CHAR> >( locale() ).tolower( (CHAR*)&ch, (CHAR*)&ch + charSize() );
				}
				return ch;
			}

			inline tCharacter getUpper() const {
				tCharacter ch = character();
				if (canChangeCase()) {
					std::use_facet<std::ctype<CHAR> >( locale() ).toupper( (CHAR*)&ch, (CHAR*)&ch + charSize() );
				}
				return ch;
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
				if (noCase) {
					return getLower() == b.getLower();
				} else {
					return character() == b.character();
				}
			}

			inline int charCmp(const ConstIterator& test, bool noCase = false) const {
				tCharacter a, b;
				if (noCase) {
					a = getLower();
					b = test.getLower();
				} else {
					a = character();
					b = test.character();
				}
				if (isWide()) {
					// inaczej specjalizacja by³aby problematyczna...
					return _wcsncoll((wchar_t*)&a, (wchar_t*)&b, charSize());
				} else {
					return std::use_facet<std::collate<CHAR> > ( locale() ).compare ( (CHAR*)&a, (CHAR*)&a + charSize(), (CHAR*)&b, (CHAR*)&b + charSize() );
				}
			}

			inline unsigned int charSize() const {
				return 1;
			}

			inline unsigned int prevCharSize() const {
				return 1;
			}

			inline bool canChangeCase() const {
				return true;
			}

			inline static bool isWide() {
				return sizeof(CHAR) == sizeof(wchar_t);
			}


		private:
			const CHAR* _p;
			unsigned int _char;
		};

		inline static bool isWide() {
			return sizeof(CHAR) == sizeof(wchar_t);
		}

		static unsigned int getLength(const CHAR* from, const CHAR* to) {
			ConstIterator it = from;
			for (; it < to; ++it) {
			}
			return it.getPosition();
		}

		static unsigned int getDataPos(const CHAR* from, const CHAR* to, int charPos) {
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
		
		static unsigned int getCharPos(const CHAR* from, const CHAR* to, unsigned int dataPos) {
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
			ConstIterator a = abegin;
			ConstIterator b = bbegin;
			for (; a < aend && b < bend; ++a, ++b) {
				if (!a.charEq(b, noCase)) return false;
			}
			return true;
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
			if (skip != 0) {
				ConstIterator last;
				ConstIterator current = begin;
				bool findLast = (skip < 0);
				unsigned pos = 0;
				while (findLast || skip-- >= 0) {
					ConstIterator found = find(current, end, findBegin, findEnd, noCase, 0);
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
			} else {
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
			}

		}
            
		static void replaceChars(const CHAR* str, const CHAR* end, const CHAR* fromBegin, const CHAR* fromEnd, const CHAR* toBegin, const CHAR* toEnd, unsigned int limit = -1, bool swap = false) {
			unsigned length = end - str;
			bool erase = (toBegin == toEnd);
			while (str < end && limit > 0) {
				const CHAR* from = fromBegin;
				const CHAR* to = toBegin;
				while (from < fromEnd) {
					bool match = *str == *from;
					if (match ^ swap) {
						limit --;
						if (erase) {
							--length;
							CHAR* move = str;
							while (move < str + length) {
								*move = *(move+1);
								++move;
							}

						} else {
							*str = *to;
						}
					}
					++from;
					if (!erase && to + 1 < toEnd) {
						++to;
					}
				}
				++str;
			}
			return length;
		}


	};


	// ---------------------------   CodePage Specific


	// ----- char / UTF-8


	template<> inline unsigned int StringType<char, cpUTF8>::ConstIterator::charSize() const {
		if ((*_p & 0x80) == 0) return 1;
		if ((*_p & 0xF0) == 0xF0) return 4;
		if ((*_p & 0xE0) == 0xE0) return 3;
		if ((*_p & 0xC0) == 0xC0) return 2;
		return 1; // b³¹d!
	}

	template<> inline unsigned int StringType<char, cpUTF8>::ConstIterator::prevCharSize() const {
		const CHAR* p = _p - 1;
		if ((*p & 0x80) == 0) return 1;
		if ((*p & 0xC0) == 0xC0) return 1; // b³¹d!
		p--;
		if ((*p & 0xC0) != 0x80) return 2; // 10xxxxxx
		p--;
		if ((*p & 0xC0) != 0x80) return 3; // 10xxxxxx
		return 4;
	}

	template<> inline bool StringType<char, cpUTF8>::ConstIterator::canChangeCase() const {
		return charSize() == 1;
	}



}

#endif