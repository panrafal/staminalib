/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once

#include "ObjectImpl.h"
#include "RegEx.h"
#include "FindFile.h"

namespace Stamina {

	class FileFilter: public SharedObject<iSharedObject> {
	public:

		virtual bool filter(const FindFile::Found& file)=0;

	};

	class FileFilter_RegEx: public FileFilter {
	public:

		FileFilter_RegEx(const RegEx::oCompiled& compiled, bool positive = true) {
			_re.setCompiledPattern(compiled);
			_positive = positive;
		}
		FileFilter_RegEx(const std::string& pattern, bool positive = true) {
			_re.setPattern(pattern);
			_positive = positive;
		}

		RegEx* operator -> () {
			return &_re;
		}
		RegEx& operator * () {
			return _re;
		}
		RegEx& getRE () {
			return _re;
		}

		virtual bool filter(const FindFile::Found& file) {
			_re.setSubject(file.getFileName());
			_re.match();
			return _re.isMatched() ? _positive : !_positive;
		}

	private:
		RegEx _re;
		bool _positive;

	};

	typedef SharedPtr<FileFilter> oFileFilter;

	class FindFileFiltered: public FindFile {
	public:
		FindFileFiltered(const std::string& mask = ""):FindFile(mask) {
		}

		void addFilter(const oFileFilter& filter) {
			_filters.push_back(filter);
		}



	private:
		typedef std::list<oFileFilter> tFilterList;

		virtual bool filter() {
			if (FindFile::filter() == false) return false; // nie ma sensu nawet patrzeæ dalej...
			for (tFilterList::iterator it = _filters.begin(); it != _filters.end(); ++it) {
				if ((*it)->filter(_found) == false) return false;
			}
			return true;
		}

		tFilterList _filters;

	};

}