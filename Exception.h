/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once

#include <string>

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

		virtual std::string getReason() = 0;

	private:
	};

	class ExceptionString {
	public:

		ExceptionString(const std::string& reason):_reason(reason) {
		}

		std::string getReason() {
			return _reason;
		}
		bool hasReason() {
			return !_reason.empty();
		}

	private:
		std::string _reason;
	};


};