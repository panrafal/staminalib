/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
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
		size = _vsnprintf(buff, size+1, format, va);
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