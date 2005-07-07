/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

//---------------------------------------------------------------------------
//#pragma link "MEMMGR.LIB"
#include "stdafx.h"
#include <windows.h>


#include <pcre.h>
#include "RegEx.h"
#include "Assert.h"



/*
*  PREG (Perl-style Regular Expressions C++ wrapper for pcre)
*  (c)Copyright 2002-2003  Rafa³ Lindemann | Stamina
*  http://www.stamina.eu.org/
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License Version 2 as
*  published by the Free Software Foundation.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/  



using namespace std;

namespace Stamina {

	void RegEx::init() {
		_vector = 0;
		_vectorSize = 0;
		_localeOneTime = false;
		_start = 0;
		_result = -1;
	}

	RegEx::RegEx() {
		init();
	}

	RegEx::~RegEx() {
		if (_vector)
			delete [] _vector;
	}




	// ------------------------   Internal
	void RegEx::setLocale(const char * locale, bool useOneTime){
		this->setLocale(oLocale(new Locale(locale)), useOneTime);
	}
	void RegEx::setLocale(const oLocale& locale, bool useOneTime){
		this->_localeOneTime = useOneTime;
		this->_locale = locale;
	}
	void RegEx::resetLocale() {
		this->_locale.reset();
	}

	void RegEx::reserveVector(int size) {
		if (_vectorSize < size) {
			if (_vector)
				delete [] _vector;
			_vector = new int [size];
			_vectorSize = size;
		}
	}

	void RegEx::setPattern (const std::string& pattern) {
		setCompiledPattern(compilePerl(pattern));
	}
	void RegEx::setCompiledPattern (const oCompiled& pattern) {
		if (this->_localeOneTime)
			_locale.reset();
		if (pattern) 
			this->reserveVector((pattern->getSubpatternsCount() * 2 + 4) * 2);
		this->_compiled = pattern;
	}


	std::string RegEx::operator [] (int i) const {
		if (_result <= i || _vector[i*2]<0) return "";
		return _subject.substr(_vector[i*2] , _vector[i*2+1] - _vector[i*2]);
	}
	std::string RegEx::operator [] (const std::string& named) const {
		int index = this->getNamedIndex(named);
		if (index == PCRE_ERROR_NOSUBSTRING) 
			return "";
		else
			return (*this)[index];
	}



	int RegEx::process() {
		this->_result = -1;
		S_ASSERT(_compiled);
		if (!_compiled) return -1;

		int flags = _compiled->getExecFlags();
		if (_start > 0)
			flags |= PCRE_NOTBOL;

		this->_result = pcre_exec(_compiled->getCompiled() , _compiled->getExtra() , _subject.c_str() , _subject.length() , _start , flags , _vector , _vectorSize);

		return _result;
	}

	const char* RegEx::getByVector(int i) {
		if (_vector[i]<0 || i < 0 || i >= _vectorSize) return 0;
		const char* c = _subject.c_str();
		return c + _vector[i];
	}


	string RegEx::insertVars(const string& sub) {
		string re = "";
		re.reserve(sub.length());
		for (unsigned int i = 0 ; i < sub.length() ; i++) {
			if ((sub[i]=='$' || sub[i]=='\\') && i+1<sub.length() &&
				((sub[i+1]>='0' && sub[i+1]<='9') || sub[i+1]=='&') ) {
					char index = 0;
					if (sub[i+1]!='&'/* && sub[i]!='$'*/) 
						index = sub[i+1] - '0';
					re+=(*this)[index];
					i++;
				} else re+=sub[i];

		}
		return re;
	}


	// ------------------------   API


	int RegEx::match(const char * pat , const char * sub) {
		setPattern(pat);
		setSubject(sub);
		return match();
	}

	string RegEx::replace(const char * chg , int limit) {
		string re = "";
		int i = 0;
		if (_start) re=_subject.substr(0 , _start);
		while (limit==0 || i<limit) {
			if (process()<=0) break;

			re += _subject.substr(_start,_vector[0] - _start);

			re += insertVars(chg);
			_start = _vector[1];
			i++;
		}
		re += _subject.substr(_start , _subject.length() - _start);
		this->reset();
		return re;
	}



	string RegEx::replace(fReplace callback , int limit , void * param) {
		string re = "";
		int i = 0;
		if (_start) re=_subject.substr(0 , _start);
		while (limit==0 || i<limit) {
			if (process()<=0) break;
			re += _subject.substr(_start,_vector[0] - _start);
			re += callback(this , param);
			_start = _vector[1];
			i++;
		}
		re += _subject.substr(_start , _subject.length() - _start);
		this->reset();
		return re;
	}



	int RegEx::match_global() {
		this->process();
		if (this->isMatched()) _start = _vector[1];
		return this->getMatched();
	}


// ---------------------------------------------------------

	void RegEx::Compiled::compile(const std::string& pattern, int flags, const oLocale& locale) throw(...) {
		this->_compiled = 0;
		this->_extra = 0;
        this->_flags = flags;
		const char* error;
		int errorPos;
	
		this->_compiled = pcre_compile(pattern.c_str() , this->getCompileFlags() , &error , &errorPos , locale ? locale->getTable() : 0);
		if (!this->_compiled) {
			throw CompileException(error, errorPos);
		}
	}
	RegEx::Compiled::Compiled(const std::string& pattern, const oLocale& locale, int addFlags) throw(...) {
		this->_compiled = 0;
		this->_extra = 0;
		oLocale overrideLocale;
		int flags = addFlags;
		char de = pattern[0];
		int pos = pattern.find_last_of(de);
		if (pos>=(int)pattern.length()) throw CompileException("Closing delimiter not found!", -1);
		for (int i = pos+1 ; i < (int)pattern.length() ; i++) {
			switch (pattern[i]) {
				case 'i': flags |= PCRE_CASELESS;break;
				case 'm': flags |= PCRE_MULTILINE;break;
				case 's': flags |= PCRE_DOTALL;break;
				case 'x': flags |= PCRE_EXTENDED;break;
				case 'A': flags |= PCRE_ANCHORED;break;
				case 'D': flags |= PCRE_DOLLAR_ENDONLY;break;
				case 'X': flags |= PCRE_EXTRA;break;
				case 'U': flags |= PCRE_UNGREEDY;break;
				case '8': flags |= PCRE_UTF8;break;

				case 'B': flags |= PCRE_NOTBOL;break;
				case 'N': flags |= PCRE_NOTEOL;break;
				case 'E': flags |= PCRE_NOTEMPTY;break;
				case 'L': 
					{
						overrideLocale.reset(new Locale(pattern.substr(i+1).c_str()));
						i = pattern.length(); // konczymy szukanie...
					}
			}
		}
		this->compile(pattern.substr(1, pos - 1) , flags, overrideLocale ? overrideLocale : locale);
	}


	RegEx::Compiled::~Compiled() {
		if (this->_compiled)
			free(this->_compiled);
		if (this->_extra)
			free(this->_extra);
	}


	int RegEx::Compiled::getSubpatternsCount() const {
		S_ASSERT(_compiled);
		if (!_compiled) return 0;
		int c = 0;
		pcre_fullinfo(_compiled, _extra, PCRE_INFO_CAPTURECOUNT, &c);
		return c;
	}
	int RegEx::Compiled::getNamedIndex(const std::string& named) const {
		S_ASSERT(_compiled);
		if (!_compiled) return PCRE_ERROR_NOSUBSTRING;
		return pcre_get_stringnumber(_compiled, named.c_str());
	}



	RegEx::oCompiled RegEx::compileNoThrow(const std::string& pattern, int flags, const oLocale& locale, const char** error, int* errorPos) {
		oCompiled comp;
		try {
			comp.reset(new Compiled(pattern, flags, locale));
		} catch (CompileException e) {
			comp.reset();
			if (error)
				*error = e.error;
			if (errorPos)
				*errorPos = e.pos;
		}
		return comp;
	}
	RegEx::oCompiled RegEx::compilePerlNoThrow(const std::string& pattern, const oLocale& locale, const char** error, int* errorPos, int addFlags) {
		oCompiled comp;
		try {
			comp.reset(new Compiled(pattern, locale, addFlags));
		} catch (CompileException e) {
			comp.reset();
			if (error)
				*error = e.error;
			if (errorPos)
				*errorPos = e.pos;
		}
		return comp;
	}


// ---------------------------------------------------------

	RegEx::Locale::Locale(const char * locale) {
		S_ASSERT(locale);
		string coll (setlocale(LC_COLLATE, 0));
		string ctyp (setlocale(LC_CTYPE, 0));
		string mone (setlocale(LC_MONETARY, 0));
		string nume (setlocale(LC_NUMERIC, 0));
		string time (setlocale(LC_TIME, 0));
		setlocale(LC_ALL, locale);
		this->_tableptr = pcre_maketables();
		setlocale(LC_COLLATE, coll.c_str());
		setlocale(LC_CTYPE, ctyp.c_str());
		setlocale(LC_MONETARY, mone.c_str());
		setlocale(LC_NUMERIC, nume.c_str());
		setlocale(LC_TIME, time.c_str());
	}

	RegEx::Locale::~Locale() {
		pcre_free((void*)this->_tableptr);
	}

};