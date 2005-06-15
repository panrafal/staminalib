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

#include "Object.h"

namespace Stamina {

    enum LogLevel {
		logNone = 0,
		logNet = 1,
		logTraffic = 2 , 
		logDump = 4 , 
		logFunc = 8 , 
		logMisc = 0x10 ,
		logError = 0x20 , 
		logWarn = 0x40 ,
		logAssert = 0x80 ,
		logLog = 0x100 ,
		logDebug = 0x1000 ,
		logEvent = 0x2000 ,
		logAll = 0xFFFFFF
	};


	class Logger: public iSharedObject {
	public:
	
		inline bool hasLevel(LogLevel level) {
			return (_level & level) == level;
		}
		inline LogLevel getLevel(LogLevel level = logAll) {
			return _level;
		}

		inline void log(LogLevel level, const char* format, ...) {
			if (!this->hasLevel(level)) 
				return;
			va_list ap;
			va_start(ap, format);
			this->logV(level , "", "", format , ap);
			va_end(ap);
		}

		inline void log(LogLevel level, const char* module, const char* where, const char* format, ...) {
			if (!this->hasLevel(level)) 
				return;
			va_list ap;
			va_start(ap, format);
			this->logV(level , module, where, format , ap);
			va_end(ap);
		}

		virtual void logV(LogLevel level, const char* module, const char* where, const char* format, va_list va) = 0;

		virtual void logMsg(LogLevel level, const char* module, const char* where, const char* msg) = 0;


	protected:

		LogLevel _level;

	};

	typedef SharedPtr<Logger> oLogger;

	extern oLogger mainLogger;

	inline void log(LogLevel level, const char* format, ...) {
		if (!mainLogger) 
			return;
		va_list ap;
		va_start(ap, format);
		mainLogger->logV(level , "", "", format , ap);
		va_end(ap);
	}

	inline void log(LogLevel level, const char* module, const char* where, const char* format, ...) {
		if (!mainLogger) 
			return;
		va_list ap;
		va_start(ap, format);
		mainLogger->logV(level , module, where, format , ap);
		va_end(ap);
	}

	inline bool hasLogLevel(LogLevel level) {
		if (mainLogger) {
			return mainLogger->hasLevel(level);
		} else {
			return false;
		}
	}



};