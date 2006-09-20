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

	inline LogLevel operator | (const LogLevel& a, const LogLevel& b) {
		return (LogLevel)((int)a | (int)b);
	}


	class Logger: public iSharedObject {
	public:
	
		STAMINA_OBJECT_CLASS_VERSION(Logger, iSharedObject, Version(1,0,0,0));

		inline bool hasLevel(LogLevel level) {
			return (_level & level) == level;
		}
		inline LogLevel getLevel(LogLevel level = logAll) {
			return _level;
		}

		virtual void setLevel(LogLevel level, LogLevel levelMask = logAll) {
			_level = (LogLevel)((this->_level & ~levelMask) | level);
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

		virtual void logMsg(LogLevel level, const char* module, const char* where, const StringRef& msg) = 0;


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


	STAMINA_REGISTER_CLASS_VERSION(Logger);

};