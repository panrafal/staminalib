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
#include "Logger.h"

namespace Stamina {


	class LoggerImpl: public SharedObject<Logger> {
	public:

		LoggerImpl(LogLevel level) {
			_level = level;
		}
	
		virtual void logV(LogLevel level, const char* module, const char* where, const char* format, va_list va);


		STAMINA_OBJECT_CLASS(Stamina::LoggerImpl, Logger);
	};


	class LoggerDebugOutput: public LoggerImpl {
	public:
		LoggerDebugOutput(LogLevel level): LoggerImpl(level) {
		}

		void logMsg(LogLevel level, const char* module, const char* where, const StringRef& msg);

	};


	class LoggerFile: public LoggerImpl {
	public:
		LoggerFile(FILE* file, LogLevel level): LoggerImpl(level) {
			_file = file;
		}

		void logMsg(LogLevel level, const char* module, const char* where, const StringRef& msg);

	private:
		FILE* _file;
	};

};