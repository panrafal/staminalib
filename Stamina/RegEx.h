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

$Id$

*/


#ifndef __STAMINA_REGEX__
#define __STAMINA_REGEX__

#include "Stamina.h"
#include "String.h"
#include <boost\smart_ptr.hpp>
#include "Helpers.h"

namespace Stamina {

	/** Regular expressions.


	*/
	class RegEx {
	public:

		class Compiled;
		class Locale;
		typedef boost::shared_ptr<Compiled> oCompiled;
		typedef boost::shared_ptr<Locale> oLocale;
		typedef std::string (__stdcall*fReplace)(class RegEx * , void * param);


		class CompileException {
		public:
			CompileException(const char* error, int pos):error(error), pos(pos) {
			}
			const char* error;
			int pos;
		};

		/** Compiled pattern.
		*/
		class Compiled {
		public:
			friend class RegEx;
			Compiled(const std::string& pattern, int flags, const oLocale& locale=oLocale()) throw(CompileException) {
				compile(pattern, flags, locale);
			}
			Compiled(const std::string& pattern, const oLocale& locale=oLocale(), int addFlags=0) throw(CompileException);
			~Compiled();

			int getSubpatternsCount() const;
			int getNamedIndex(const std::string& named) const;
#ifdef _PCRE_H
			pcre* getCompiled() const {
				return _compiled;
			}
			pcre_extra* getExtra() const {
				return _extra;
			}
			int getExecFlags() const {
				return _flags & (PCRE_NOTBOL | PCRE_NOTEOL | PCRE_NOTEMPTY | PCRE_ANCHORED);
			}
			int getCompileFlags() const {
				return _flags & ~(PCRE_NOTBOL | PCRE_NOTEOL | PCRE_NOTEMPTY | PCRE_ANCHORED);
			}
#endif

			int getFlags() const {
				return _flags;
			}


		private:
			void compile(const std::string& pattern, int flags, const oLocale& locale=oLocale()) throw(...);
#ifdef _PCRE_H
			pcre* _compiled;
			pcre_extra* _extra;
#else
			void* _compiled;
			void* _extra;
#endif
			int _flags;
		};

		class Locale {
		public:
			Locale(const char * locale);
			const unsigned char * getTable() {
				return _tableptr;
			}
			~Locale();
		private:
			const unsigned char * _tableptr;
		};




		static oCompiled compile(const std::string& pattern, int flags, const oLocale& locale=oLocale()) throw(...) {
			return oCompiled(new Compiled(pattern, flags, locale));
		}
		static oCompiled compileNoThrow(const std::string& pattern, int flags, const oLocale& locale=oLocale(), const char** error=0, int* errorPos=0);
		static oCompiled compilePerl(const std::string& pattern, const oLocale& locale=oLocale(), int addFlags=0) throw(...) {
			return oCompiled(new Compiled(pattern, locale, addFlags));
		}
		static oCompiled compilePerlNoThrow(const std::string& pattern, const oLocale& locale=oLocale(), const char** error=0, int* errorPos=0, int addFlags=0);


	public:

		RegEx ();
		~RegEx();

		/** Returns results at index @a i.
		*/
		std::string operator [] (int i) const;
		/** Returns results with name @a named.
		*/
		std::string operator [] (const std::string& named) const;
		//inline std::string operator () (int i) const {return (*this)[i];}

		/** Returns results at index @a i.
		*/
		inline std::string getSub(int i) const {return (*this)[i];}

		/** Checks if at index @a i is result.
		*/
		inline bool hasSub(int i) const {
			if (_result <= i || _vector[i*2]<0 || _vector[i*2] == _vector[i*2+1]) return false;
			return true;
		}

		/** Return index of results with name @a named.
		*/
		inline int getNamedIndex(const std::string& named) const {
			if (!_compiled) return -1;
            return _compiled->getNamedIndex(named);
		}

		/** @todo co to robi?
		*/
		const char* getByVector(int i);

		/** Sets the regular expression's pattern.
		*/
		void setPattern (const StringRef& v);

		/** Sets compiled regular expression's pattern.
		*/
		void setCompiledPattern (const oCompiled& pattern);

		/** Sets subject for a match or replace to the regular expression given in pattern.
		*/
		inline void setSubject (const StringRef& v) {
			this->_result = -1;
			this->reset();
			_subject = v;
		}


		//string getPattern() {return _pattern_orig;}
		std::string getSubject() {
			return _subject;
		}
		std::string getSubject(size_t start , size_t end) {
			return _subject.substr(start , end<start?end:end - start);
		}

		/** Returns refenrece to subject.
		*/
		const std::string& getSubjectRef() {
			return _subject;
		}

		/** Returns number of results.
		*/
		inline int getResult() {
			return _result;
		}
		inline bool isMatched() {
			return _result > 0;
		}
		inline bool isFailed() {
			return _result < 0;
		}
		inline int getMatched() {
			return prepareResult(this->_result);
		}
		inline static int prepareResult(int v) {
			return v < 0 ? 0 : v;
		}


		inline void reset() {
			_start=0;
		}
		/** Returns index of start position in subject to perform next match or replace.
		*/
		inline int getStart() {
			return _start;
		}

		/** Sets index of start position in subjecto to perform next match or replace.
		*/
		inline void setStart(unsigned int start) {
			if (start < this->_subject.length())
				_start = start;
			else
				_start = this->_subject.length();
		}

		int getVector(unsigned int num) {
			if (_result > 0 && num < (unsigned int)_result*2) 
				return _vector[num]; 
			else 
				return 0;
		}

		/** Searches subject for a match to the regular expression given in pattern.
		@param pat Compiled pattern.
		@return Number of matches.
		*/
		int match(const oCompiled& pat) {
			this->setCompiledPattern(pat); 
			return this->match();
		}

		/** Searches subject for a match to the regular expression given in pattern.
		@param pat Pattern.
		@return Number of matches.
		*/
		int match(const StringRef& pat) {
			this->setPattern(pat); 
			return this->match();
		}

		/** Searches subject for a match to the regular expression given in pattern.
		@param pat Pattern.
		@param sub Subject.
		@return Number of matches.
		*/
		int match(const StringRef& pat , const StringRef& sub);

		/** Searches subject for a match to the regular expression given in pattern.
		*/
		int match() {
			this->process();
			return this->getMatched();
		}
		/** Searches subject for all matches to the regular expression given in pattern.
		@return Number of matches.
		*/
		int match_global();

		/** Searches subject for all matches to the regular expression given in pattern.
		@param pat Pattern.
		@param sub Subject.
		@return Number of matches.
		*/
		int match_global(const StringRef& pat , const StringRef& sub) {
			this->setPattern(pat);
			this->setSubject(sub);
			return this->match_global();
		}
		/** Perform a regular expression search and replace.
		@param chg Replacement.
		@param limit Limit of replacements.
		@return Replaced string.
		*/
		std::string replace(const StringRef& chg , int limit=0);

		/** Perform a regular expression search and replace.
		@param pat Pattern.
		@param chg Replacement.
		@param sub Subject.
		@param limit Limit of replacements.
		@return Replaced string.
		*/
		inline std::string replace(const StringRef& pat , const StringRef& chg , const StringRef& sub , int limit=0) {
			this->setPattern(pat);
			this->setSubject(sub);
			return this->replace(chg , limit);
		}

		/** Perform a regular expression search and replace using callback function.
		@param callback Callback function.
		@param limit Limit of replacements.
		@param param User defined param.
		@return Replaced string.
		*/
		std::string replace(fReplace callback , int limit=0 , void * param=0);
		/** Perform a regular expression search and replace using callback function.
		@param pat Pattern.
		@param callback Callback function.
		@param sub Subject.
		@param limit Limit of replacements.
		@param param User defined param.
		@return Replaced string.
		*/
		std::string replace(const StringRef& pat , fReplace callback , const StringRef& sub , int limit=0, void * param=0) {
			this->setPattern(pat);
			this->setSubject(sub);
			return this->replace(callback, limit, param);
		}

		/** Replaces using callback function.
		@param func Callback function.
		@param limit Limit of replacements.
		@return Replaced string.
		*/
		template <typename F>
			std::string replaceCB(F func, int limit=0) 
		{
			std::string re = "";
			int i = 0;
			if (_start) re = _subject.substr(0 , _start);
			while (limit==0 || i<limit) {
				if (process()<=0) break;
				re += _subject.substr(_start,_vector[0] - _start);
				re += func(this);
				_start = _vector[1];
				i++;
			}
			re += _subject.substr(_start , _subject.length() - _start);
			this->reset();
			return re;
		}

		/** Perform a regular expression search and replace using callback function.
		@param pat Pattern.
		@param func Callback function.
		@param sub Subject.
		@param limit Limit of replacements.
		@return Replaced string.
		*/
		template <typename F>
			std::string replaceCB(const StringRef& pat, F func, const StringRef& sub , int limit=0) {
			this->setPattern(pat);
			this->setSubject(sub);
			return this->replace(func, limit);
		}


		inline void replaceItself(const StringRef& chg, int limit=0) {
			this->setSubject( this->replace(chg, limit) );
		}
		inline void replaceItself(const StringRef& pat, const StringRef& chg, int limit=0) {
			this->setPattern(pat);
			this->replaceItself(chg, limit);
		}
		
		inline void replaceItself(fReplace callback , int limit=0 , void * param=0) {
			this->setSubject( this->replace(callback, limit, param) );
		}
		inline void replaceItself(const StringRef& pat, fReplace callback , int limit=0 , void * param=0) {
			this->setPattern(pat);
			this->replaceItself(callback, limit, param);
		}

		template <typename F>
			inline void replaceItself(F func, int limit=0) 
		{
			this->setSubject( this->replace(func, limit) );
		}
		template <typename F>
			inline void replaceItself(const StringRef& pat, F func, int limit=0) 
		{
			this->setPattern(pat);
			this->replaceItself(func, limit);
		}


		template <typename F>
			int match_global(F func, int limit=0) 
		{
			int i = 0;
			while ((limit==0 || i<limit) && match_global()) {
				func(this);
				i++;
			}
			this->reset();
			return i;
		}

		template <typename F>
			int match_global(F func, const StringRef& pat, const StringRef& sub , int limit=0) {
			this->setPattern(pat);
			this->setSubject(sub);
			return this->match_global(func, limit);
		}


		void setLocale(const char * locale, bool useOneTime=false);
		void setLocale(const oLocale& locale, bool useOneTime=false);
		void resetLocale();
		oCompiled getCompiled() {
			return _compiled;
		}
		oLocale getLocale() {
			return _locale;
		}

	public:

		/** Perform a regular expression match.
		@param pat Pattern.
		@param sub Subject.
		@return Number of matches.
		*/
		inline static int doMatch(const StringRef& pat , const StringRef& sub) {
			return doMatch(oCompiled(new Compiled(pat)), sub);
		}
		/** Perform a regular expression match.
		@param pat Compiled pattern.
		@param sub Subject.
		@return Number of matches.
		*/
		inline static int doMatch(const oCompiled& pat , const StringRef& sub) {
			RegEx r;
			r.setSubject(sub);
			return r.match(pat);
		}

		/** Perform a regular expression search and replace.
		@param pat Pattern.
		@oaram chg Replacement.
		@param sub Subject.
		@param limit Limit of replacements.
		@return Replaced string.
		*/
		inline static String doReplace(const StringRef& pat , const StringRef& chg , const StringRef& sub , int limit=0) {
			RegEx r;
			return r.replace(pat, chg, sub, limit);
		}

		/** Perform a regular expression search and replace using callback function.
		@param pat Pattern.
		@oaram callback Callback function.
		@param sub Subject.
		@param limit Limit of replacements.
		@param param User defined param.
		@return Replaced string.
		*/
		inline static String doReplace(const StringRef& pat , fReplace callback , const StringRef& sub , int limit=0, void * param=0) {
			RegEx r;
            return r.replace(pat, callback, sub, limit, param);
		}

		/** Perform a regular expression search and replace using callback function.
		@param pat Pattern.
		@oaram callback Callback function.
		@param sub Subject.
		@param limit Limit of replacements.
		@return Replaced string.
		*/
		template <typename F> inline static  String doReplace(const StringRef& pat, F func, const StringRef& sub , int limit=0) 
		{
			RegEx r;
			return r.replace(pat, func, sub, limit);
		}

		/** Returns result at index @a subPattern.

		Perform a regular expression match and returns result at index @a subPattern if matched
		or return @def if not matched.
		@param pat Pattern.
		@param sub Subject.
		@param subPattern Index of result.
		@param def Default value to return if not matched.
		@return Matched result at index @subPattern.
		*/
		inline static String doGet(const StringRef& pat, const StringRef& sub, int subPattern = 0, const StringRef& def = "") {
			return doGet(oCompiled(new Compiled(pat)), sub, subPattern, def);
		}

		/** Returns result at index @a subPattern.
		@see doGet
		@param pat Compiled pattern.
		@param sub Subject.
		@param subPattern Index of result.
		@param def Default value to return if not matched.
		@return Matched result at index @subPattern.
		*/
		inline static String doGet(const oCompiled& pat, const StringRef& sub, int subPattern = 0, const StringRef& def = "") {
			RegEx r;
			r.setSubject(sub);
			r.match(pat);
			if (r.isMatched()) {
				return r[subPattern];
			} else {
				return def;
			}
		}

		inline static String addSlashes(const StringRef& str) {
			return ::Stamina::addSlashes(str, "\"'\\/^$!?()[]+.{}*", '\\');
		}

		// ----------------------------------------------------
	private:

		oLocale _locale;
		oCompiled _compiled;
		int* _vector;
		int _vectorSize;
		bool _localeOneTime;
		int _start;
		int _result;
		std::string _subject;

		void init();
		void reserveVector(int size);
		int process();
		std::string insertVars(const std::string& sub);

	};


};

#endif