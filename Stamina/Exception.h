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

#include "String.h"

#define EXCEPTION_PURE_VIRTUAL_CALL 0xE0006001

#define IMPLEMENT_PURECALL_EXCEPTION(name)\
	int _purecall() {\
		const char* args [] = {name};\
		RaiseException(EXCEPTION_PURE_VIRTUAL_CALL, 0, 1, (ULONG_PTR*)args);\
		return 0;\
	}


namespace Stamina {


	class Exception {
	public:

		virtual String getReason() = 0;

	private:
	};

	class ExceptionString {
	public:

		ExceptionString(const StringRef& reason):_reason(reason) {
		}

		String getReason() {
			return _reason;
		}
		bool hasReason() {
			return !_reason.empty();
		}

	private:
		String _reason;
	};


};