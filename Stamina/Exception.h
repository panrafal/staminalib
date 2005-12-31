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


	class Exception: public iObject {
	public:

		STAMINA_OBJECT_CLASS_VERSION(Exception, iObject, Version(1,0,0,0));

		virtual String getReason() const {
			return this->toString();
		}
		virtual String toString(iStringFormatter* f = 0) const {
			return iObject::toString(f);
		}

	private:
	};

	class ExceptionString: public Exception {
	public:

		STAMINA_OBJECT_CLASS(ExceptionString, Exception);

		ExceptionString(const StringRef& reason):_reason(reason) {
		}

		virtual String getReason() const {
			return _reason;
		}
		bool hasReason() const {
			return !_reason.empty();
		}
		virtual String toString(iStringFormatter* format=0) const {
			return getReason();
		}

	private:
		String _reason;
	};


	class ExceptionDeprecated: public ExceptionString {
	public:
		STAMINA_OBJECT_CLASS(ExceptionDeprecated, ExceptionString);

		ExceptionDeprecated(const StringRef& reason):ExceptionString(reason) {
		}

	};

	class ExceptionBadCast: public Exception {
	public:
		STAMINA_OBJECT_CLASS(ExceptionBadCast, Exception);

		ExceptionBadCast(const ObjectClassInfo& from, const ObjectClassInfo& to):from(from), to(to) {
		}

		const ObjectClassInfo& from;
		const ObjectClassInfo& to;

	};

	template <class TO> inline TO* iObject::tryCastObject() throw (...) {
		TO* obj = this->castObject<TO>();
		if (obj == 0) {
			throw ExceptionBadCast(this->getClass(), TO::staticClassInfo());
		}
		return obj;
	}

	STAMINA_REGISTER_CLASS_VERSION(Exception);

};