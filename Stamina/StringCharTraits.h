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

The Initial Developer of the Original Code is "STAMINA" - Rafa� Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa� Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id: $

*/


#ifndef __STAMINA_STRINGCHARTRAITS__
#define __STAMINA_STRINGCHARTRAITS__

#pragma once

#include <locale>
#include "../depends/ConvertUTF.h"

namespace Stamina {

	template <typename CHAR>
	class CharTraits_CRT {
	public:
		typedef CHAR tChar;
		typedef CHAR tCharacter;
		typedef int tCharacterBuffer;

		const static int codepage = CP_ACP;
		const static bool constWidth = true;
		const static bool fullCharProperties = true;
		const static bool isWide = (sizeof(tChar) == sizeof(wchar_t));

		inline static unsigned int charSize(const tChar* ch) {
			return 1;
		}
		inline static unsigned int prevCharSize(const tChar* ch) {
			return 1;
		}
		inline static tCharacter getCharacter(const tChar* ch) {
			return *ch;
		}

		inline static const std::locale& locale() {
			static std::locale loc;
			return loc;
		}

		inline static bool isLower(const tChar* ch) {
			return std::islower(*ch, locale());
		}
		inline static bool isUpper(const tChar* ch) {
			return std::isupper(*ch, locale());
		}

		inline static tCharacter getLower(const tChar* ch) {
			return std::tolower(*ch, locale());
		}
		inline static tCharacter getUpper(const tChar* ch) {
			return std::toupper(*ch, locale());
//					std::use_facet<std::ctype<CHAR> >( locale() ).tolower( (CHAR*)&ch, (CHAR*)&ch + charSize() );

		}

		inline static bool canChangeCase(const tChar* ch) {
			return true;
		}

	};

	class CharTraits_char: public CharTraits_CRT<char> {
	public:

		inline static tCharacter getLower(const tChar* ch) {
			return (tCharacter)tolower(*ch);
		}
		inline static tCharacter getUpper(const tChar* ch) {
			return (tCharacter)toupper(*ch);

		}


		inline static bool charEq(const char* a, const char* b, bool noCase) {
			if (noCase) {
				return _strnicmp(a, b, 1) == 0;
			} else {
				return *a == *b;
			}
		}

		inline static int charCmp(const char* a, const char* b, bool noCase) {
			if (noCase) {
				return _strnicoll(a, b, 1);
			} else {
				return _strncoll(a, b, 1);
			}
		}

	};


	class CharTraits_wchar: public CharTraits_CRT<wchar_t> {
	public:
		inline static tCharacter getLower(const tChar* ch) {
			return towlower(*ch);
		}
		inline static tCharacter getUpper(const tChar* ch) {
			return towupper(*ch);

		}

		inline static bool charEq(const wchar_t* a, const wchar_t* b, bool noCase) {
			if (noCase) {
				return _wcsnicmp(a, b, 1) == 0;
			} else {
				return *a == *b;
			}
		}

		inline static int charCmp(const wchar_t* a, const wchar_t* b, bool noCase) {
			if (noCase) {
				return _wcsnicoll(a, b, 1);
			} else {
				return _wcsncoll(a, b, 1);
			}
		}
	};







// ---------------------------------------------------------    UTF-8

	class CharTraits_UTF8: public CharTraits_CRT<char> {
	public:
		typedef char tChar;
		typedef int tCharacter;
		typedef __int64 tCharacterBuffer;

		const static int codepage = CP_UTF8;
		const static bool constWidth = false;
		const static bool fullCharProperties = false;

		inline static unsigned int charSize(const tChar* ch) {
			if ((*ch & 0x80) == 0) return 1;
			if ((*ch & 0xF0) == 0xF0) return 4;
			if ((*ch & 0xE0) == 0xE0) return 3;
			if ((*ch & 0xC0) == 0xC0) return 2;
			return 1;
		}
		inline static unsigned int prevCharSize(const tChar* ch) {
			const CHAR* p = ch - 1;
			if ((*p & 0x80) == 0) return 1;
			if ((*p & 0xC0) == 0xC0) return 1; // b��d!
			p--;
			if ((*p & 0xC0) != 0x80) return 2; // 10xxxxxx
			p--;
			if ((*p & 0xC0) != 0x80) return 3; // 10xxxxxx
			return 4;
		}
		inline static tCharacter getCharacter(const tChar* ch) {
			tCharacter chr = 0;
			for (unsigned int i = 0; i < charSize(ch); i++) {
				((char*)&chr)[i] = ch[i];
			}
			return chr;
		}

		inline static const std::locale locale() {
			return std::locale::classic();
		}

		inline static wchar_t* toUTF16(const char* ch, tCharacterBuffer& buff) {
			tCharacterBuffer buff2 = getCharacter(ch);
			UTF16 * buff16 = (UTF16*)&buff;
			const UTF8 * buff8 = (UTF8*)&buff2;
			ConvertUTF8toUTF16(&buff8, buff8 + 4, &buff16, buff16 + 3, strictConversion);
			return (wchar_t*)&buff;
		}

		inline static char* fromUTF16(const wchar_t* ch, tCharacterBuffer& buff) {
			const UTF16 * buff16 = (UTF16*)ch;
			UTF8 * buff8 = (UTF8*)&buff;
			ConvertUTF16toUTF8(&buff16, buff16 + 3, &buff8, buff8 + 4, strictConversion);
			return (char*)&buff;
		}


		inline static bool isLower(const tChar* ch) {
			if (isAscii(ch)) {
				return islower(*ch) != 0;
			} else {
				tCharacterBuffer buff;
				return CharTraits_wchar::isLower(toUTF16(ch, buff));
			}
		}
		inline static bool isUpper(const tChar* ch) {
			if (isAscii(ch)) {
				return isupper(*ch) != 0;
			} else {
				tCharacterBuffer buff;
				return CharTraits_wchar::isUpper(toUTF16(ch, buff));
			}
		}

		inline static tCharacter getLower(const tChar* ch) {
			if (isAscii(ch)) {
				return tolower(*ch);
			} else {
				tCharacterBuffer buff1;
				tCharacterBuffer buff2;
				buff1 = CharTraits_wchar::getLower(toUTF16(ch, buff1));
				fromUTF16( (wchar_t*)&buff1, buff2 );
				return (tCharacter)buff2;
			}
		}
		inline static tCharacter getUpper(const tChar* ch) {
			if (isAscii(ch)) {
				return toupper(*ch);
			} else {
				tCharacterBuffer buff1;
				tCharacterBuffer buff2;
				buff1 = CharTraits_wchar::getUpper(toUTF16(ch, buff1));
				fromUTF16( (wchar_t*)&buff1, buff2 );
				return (tCharacter)buff2;
			}
		}

		inline static bool canChangeCase(const tChar* ch) {
			return true;
		}

		inline static bool isAscii(const tChar* ch) {
			return __isascii(*ch);
		}

		inline static bool charEq(const char* a, const char* b, bool noCase) {
			if (isAscii(a) && isAscii(b)) {
				if (noCase && canChangeCase(a) && canChangeCase(b)) {
					return tolower(*a) == tolower(*b);
				} else {
					return *a == *b;
				}
			} else {
				if (noCase) {
					return getLower(a) == getLower(b);
				} else {
					return getCharacter(a) == getCharacter(b);
				}
			}
		}

		inline static int charCmp(const char* a, const char* b, bool noCase) {
			if (isAscii(a) && isAscii(b)) {
				if (noCase) {
					return _strnicmp(a, b, 1);
				} else {
					return strncmp(a, b, 1);
				}
			} else {
				tCharacterBuffer buffA, buffB;
				return CharTraits_wchar::charCmp(toUTF16(a, buffA), toUTF16(b, buffB), noCase);
			}
		}

	};

};

#endif