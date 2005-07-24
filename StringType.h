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


namespace Stamina {

	template <unsigned CODEPAGE>
	struct CodePage {
		const static unsigned cp = CODEPAGE;
	};

	typedef CodePage<CP_ACP> cpACP;
	typedef CodePage<CP_UTF8> cpUTF8;

	template<typename CHAR, class CP = cpACP>
	class StringType {
	public:

		typedef unsigned tCharacter;
		const static std::locale& locale ();
		const static unsigned notFound = -1;

		template<typename CHAR, class CP>
		class ConstIterator {
		public:
			ConstIterator(const CHAR* ptr):_p(ptr),_char(0) {
			}
			ConstIterator():_p(0),_char(notFound) {
			}
			inline void add(unsigned int offset) {
				_p += offset;
				_char += offset;
			}
			inline void sub(unsigned int offset) {
				_p -= offset;
				_char -= offset;
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
			inline CHAR operator * () {
				return *_p;
			}

			/*
			inline operator const CHAR* () {
				return _p;
			}
			*/

/*			inline operator unsigned int () {
				return getPosition();
			}*/

			inline unsigned int getPosition() {
				return _char;
			}

			inline void setPosition(unsigned pos) {
				_char = pos;
			}

			inline unsigned int getFoundPosition(const ConstIterator& end) {
				if (*this == end) return notFound;
				return _char;
			}

			inline tCharacter character() {
				tCharacter ch = 0;
				for (unsigned int i = 0; i < charSize(); i++) {
					((CHAR*)ch)[i] = _p[i];
				}
				return ch;
			}

			inline tCharacter getLower() {
				tCharacter ch = character();
				use_facet<ctype<CHAR> >( locale ).tolower( (CHAR*)ch, (CHAR*)ch + charSize() );
			}

			inline tCharacter getUpper(tCharacter& ch) {
				character(ch);
				use_facet<ctype<CHAR> >( locale ).toupper( (CHAR*)ch, (CHAR*)ch + charSize() );
			}

			inline bool operator == (const ConstIterator<CHAR, CP>& b) {
				return _p == b._p;
			}
			inline bool operator != (const ConstIterator<CHAR, CP>& b) {
				return _p != b._p;
			}
			inline bool operator >= (const ConstIterator<CHAR, CP>& b) {
				return _p >= b._p;
			}
			inline bool operator <= (const ConstIterator<CHAR, CP>& b) {
				return _p <= b._p;
			}
			inline bool operator < (const ConstIterator<CHAR, CP>& b) {
				return _p < b._p;
			}
			inline bool operator > (const ConstIterator<CHAR, CP>& b) {
				return _p > b._p;
			}

			inline bool charEq(const ConstIterator<CHAR, CP>&b, bool noCase = false) {
				if (noCase) {
					return getLower() == b.getLower();
				} else {
					return character() == b.character();
				}
			}

			inline bool charCmp(const ConstIterator<CHAR, CP>&b, bool noCase = false) {
				tCharacter a, b;
				if (noCase) {
					a = getLower();
					b = b.getLower();
				} else {
					a = character();
					b = b.character();
				}
				return use_facet<collate<CHAR> > ( locale ).compare ( (CHAR*)a, (CHAR*)a + charSize(), (CHAR*)b, (CHAR*)b + charSize*() );
			}

			inline unsigned int charSize() {
				return 1;
			}

		private:
			const CHAR* _p;
			unsigned int _char;
		};


		static unsigned int getLength(const CHAR* from, const CHAR* to) {
			ConstIterator<CHAR, CP> it = from;
			for (; it < to; ++it) {
			}
			return it.getPosition();
		}

		static unsigned int getDataPos(const CHAR* from, const CHAR* to, int charPos) {
			if (charPos < 0) {
				charPos = -charPos;
				ConstIterator<CHAR, CP> it = to - 1;
				for (; it >= from; --it) {
				}
				return it - from;
			} else {
				ConstIterator<CHAR, CP> it = from;
				for (; it < to; ++it) {
				}
				return it - from;
			}
		}
		
		static unsigned int getCharPos(const CHAR* from, const CHAR* to, unsigned int dataPos) {
			ConstIterator<CHAR, CP> it = from;
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
			ConstIterator<CHAR, CP> a = abegin;
			ConstIterator<CHAR, CP> b = bbegin;
			for (; abegin < aend && bbegin < bend; ++a ++b) {
				if (!a.charEq(b, noCase)) return false;
			}
			return true;
		}

		static int compare(const CHAR* abegin, const CHAR* aend, const CHAR* bbegin, const CHAR* bend, unsigned int count, bool noCase) {
			ConstIterator<CHAR, CP> a = abegin;
			ConstIterator<CHAR, CP> b = bbegin;
			for (; abegin < aend && bbegin < bend && count > 0; ++a ++b --count) {
				int res = a.charCmp(b, noCase);
				if (res != 0) return res;
			}
			if (count == 0) return 0;
			if (abegin >= aend) return -1; // a krótszy
			if (bbegin >= bend) return 1;
			return 0 //equal;
		}

		static ConstIterator<CHAR, CP> find(const CHAR* begin, const CHAR* end, const CHAR* findBegin, const CHAR* findEnd, unsigned int count, unsigned int seq, bool noCase) {
			if (seq > 0) {
				ConstIterator<CHAR, CP> last;
				bool findLast = (seq == -1);
				while (seq--) {
					ConstIterator<CHAR, CP> found = find(begin, end, findBegin, findEnd, count, 0, noCase);
					if (found == end) { // koniec
						if (findLast)
							break;
						else
							return found; // i tak ju¿ wiêcej nie bêdzie...
					}
					unsigned pos = last.getPosition();
					last = found;
					last.setPosition( last.getPosition() + pos ); 
				}
				return last;
			} else {
				ConstIterator<CHAR, CP> found = findBegin;
				ConstIterator<CHAR, CP> current = begin;
				ConstIterator<CHAR, CP> last;
				while (current < end) {
					if (current.charEq(found, noCase)) {
						if (last == 0) last = current;
						found ++;
						if (found >= findBegin) return last;
					} else if (found != findBegin) {
						found = findBegin;
						last = 0;
					}
					++current;
				}
				return ConstIterator<CHAR, CP>();
			}

		}
            
		void replaceChars(const CHAR* str, const CHAR* end, const CHAR* fromBegin, const CHAR* fromEnd, const CHAR* toBegin, const CHAR* toEnd, unsigned int count, unsigned int limit, bool swap) {
			unsigned length = end - str;
			bool erase = (toBegin == toEnd);
			while (str < end && limit > 0 && count-- > 0) {
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


	template<> inline unsigned int StringType<char, cpUTF8>::ConstIterator<char, cpUTF8>::charSize() {
		if ((*_p & 0x80) == 0) return 1;
		if ((*_p & 0xF0) == 0xF0) return 4;
		if ((*_p & 0xE0) == 0xE0) return 3;
		if ((*_p & 0xC0) == 0xC0) return 2;
	}


}

#endif