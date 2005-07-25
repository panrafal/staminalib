#pragma once

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: WinHelper.h 17 2005-06-18 12:52:05Z hao $
 */

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

	inline bool operator == (const std::string& a, const std::wstring& b) {
		return toUnicode(a) == b;
	}
	inline bool operator == (const std::wstring& a, const std::string& b) {
		return a == toUnicode(b);
	}



};