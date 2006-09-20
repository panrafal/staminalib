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


#pragma once


#include "Helpers.h"

namespace Stamina {


	inline std::wstring toUnicode(const std::string& in, UINT codePage = CP_ACP) {
		std::wstring out;
		stringRelease(out, 
			MultiByteToWideChar(codePage, 0, in.c_str(), in.length() + 1 /*0*/, stringBuffer(out, in.length() + 1), in.length() + 1) - 1
		);
		return out;
	}

	inline std::string fromUnicode(const std::wstring& in, UINT codePage = CP_ACP) {
		std::string out;
		stringRelease(out, 
			WideCharToMultiByte(codePage, 0, in.c_str(), in.length() + 1 /*0*/, stringBuffer(out, in.length()*2 + 1), in.length()*2 + 1, 0, 0) - 1
		);
		return out;
	}

	inline std::wstring keepWideChar(const std::wstring& in, UINT codePage = CP_ACP) {
		return in;
	}
	inline std::wstring keepWideChar(const std::string& in, UINT codePage = CP_ACP) {
		return toUnicode(in, codePage);
	}

	inline std::string keepMultiByte(const std::string& in, UINT codePage = CP_ACP) {
		return in;
	}
	inline std::string keepMultiByte(const std::wstring& in, UINT codePage = CP_ACP) {
		return fromUnicode(in, codePage);
	}

	template <typename STROUT, class STRIN>
	inline STROUT keepChar(const STRIN& in , UINT codePage = CP_ACP) {
		if (sizeof(STROUT::value_type) == sizeof(char)) {
			return *((STROUT*)&keepMultiByte(in, codePage));
		} else {
			return *((STROUT*)&keepWideChar(in, codePage));
		}
	}


	inline std::ostream& operator << (std::ostream& stream, const wchar_t * txt) {
		return stream << keepMultiByte(txt);
	}
	inline std::ostream& operator << (std::ostream& stream, const std::wstring& txt) {
		return stream << keepMultiByte(txt);
	}

	/*
	inline bool operator == (const std::string& a, const std::wstring& b) {
		return toUnicode(a) == b;
	}
	inline bool operator == (const std::wstring& a, const std::string& b) {
		return a == toUnicode(b);
	}
	*/



};