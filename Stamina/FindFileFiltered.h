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

#pragma once

#include <list>
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