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


#ifndef __STAMINA_STRINGSTL__
#define __STAMINA_STRINGSTL__

#pragma once

#include "String.h"

/**@file STL compatible String implementation

This file introduces STL compatible string class that inherits from Stamina::
StringRef.

The only difference from StringRef or String is that *STL classes are locked to the specified character type, and work only in the application's default codepage. You can still assign and retrieve data of different character type.

These classes incorporate also some STL required info and functions, like iterators.



*/

namespace Stamina {

	template <typename CHAR>
	class StringRefSTL: public StringRefT<stACP> {
	public:
		STAMINA_OBJECT_CLASS(StringRefSTL<CHAR>, StringRefT<stACP>);


	public:

		StringRefSTL() {
			this->forceType();
		}

		StringRefSTL(PassStringRef& pass) {
			this->swapBuffers(pass._str);
			this->forceType();
		}

		StringRefSTL(const StringRef& str) {
			this->forceType();
			assignCheapReference(str);
		}
		StringRefSTL(const StringRefSTL& str) {
			this->forceType();
			assignCheapReference(str);
		}
		StringRefSTL(const char* ch, unsigned size = lengthUnknown) {
			assignCheapReference(ch, size);
			this->forceType();
		}
		StringRefSTL(const wchar_t* ch, unsigned size = lengthUnknown) {
			assignCheapReference(ch, size);
			this->forceType();
		}

#ifdef _STRING_
		StringRefSTL(const std::string& ch) {
			assignCheapReference(ch.c_str(), ch.size());
			this->forceType();
		}
		StringRefSTL(const std::wstring& ch) {
			assignCheapReference(ch.c_str(), ch.size());
			this->forceType();
		}
#endif
#ifdef STDSTRING_H
		StringRefSTL(const CStdStringA& ch) {
			assignCheapReference(ch.c_str(), ch.size());
			this->forceType();
		}
		StringRefSTL(const CStdStringW& ch) {
			assignCheapReference(ch.c_str(), ch.size());
			this->forceType();
		}
#endif

		inline StringRefSTL& operator = (const StringRef& b) {
			this->assign(b);
			return *this;
		}

		inline StringRefSTL& operator = (const PassStringRef& pass) {
			this->swapBuffers(pass._str);
			this->forceType();
			return *this;
		}

	public:

		const CHAR* c_str() const {
			return this->str<CHAR>();
		}

		const CHAR operator [] (int i) const {
			return this->str<CHAR>()[i];
		}

		CHAR& operator [] (int i) {
			return this->getDataBuffer<CHAR>().getBuffer()[i];
		}

	protected:

		inline void forceType() {
			// mo¿emy zmieniaæ tylko na jeden rodzaj
			StringRefT<stACP>::forceType<CHAR>();
			setTypeLock();
		}

		inline void setTypeLock() {
			__super::setTypeLock(true);
		}

	};

	template <typename CHAR>
	class StringSTL: public StringRefSTL<CHAR> {
	public:

		STAMINA_OBJECT_CLASS(StringSTL<CHAR>, StringRefSTL<CHAR>);

	public:

		StringSTL() {}
		/*
		String(const iString& b) {
			if (b.getCodePage() == CP) {
				assign();
			} else {
				b.prepareType(true);
				assign(b.getData<wchar_t>(), b.getDataSize<wchar_t>());
			}
		}*/

		StringSTL(const StringRef& str) {
			assign(str);
		}
		StringSTL(const StringSTL& str) {
			assign(str);
		}

		inline StringSTL(const PassStringRef& pass):__super(pass) {
		}

		inline StringSTL& operator = (const StringRef& b) {
			StringRefSTL<CHAR>::operator = (b);
			return *this;
		}

		inline StringSTL& operator = (const PassStringRef& pass) {
			StringRefSTL<CHAR>::operator = (pass);
			return *this;
		}


	};


	typedef StringRefSTL<char> AStringRef;
	typedef StringRefSTL<wchar_t> WStringRef;

	typedef StringSTL<char> AString;
	typedef StringSTL<wchar_t> WString;

};

#endif