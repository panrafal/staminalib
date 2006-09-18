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

#include "stdafx.h"

#include "LoggerImpl.h"

namespace Stamina {


	oLogger mainLogger;

	void LoggerImpl::logV(LogLevel level, const char* module, const char* where, const char* format, va_list va) {
		if (!this->hasLevel(level)) 
			return;

		int size = _vscprintf(format, va);
		char * buff = new char [size + 2];
		buff[size + 1] = 0;
#if (_MSC_VER >= 1400)
		size = _vsnprintf_s(buff, size+1, size, format, va);
#else
		size = _vsnprintf(buff, size+1, format, va);
#endif
		buff[size] = 0;
		this->logMsg(level, module, where, buff);
		delete [] buff;
	}

	void LoggerDebugOutput::logMsg(LogLevel level, const char* module, const char* where, const StringRef& msg) {
		CStdString txt;
		txt.Format("L> %s::%s - %s \n", module, where, msg.a_str());
		OutputDebugString(txt);
	}

	void LoggerFile::logMsg(LogLevel level, const char* module, const char* where, const StringRef& msg) {
		fprintf(_file, "%s::%s - %s\n", module, where, msg.a_str());
	}


};