/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#ifndef __STAMINA_SOCKETERROR_H__
#define __STAMINA_SOCKETERROR_H__

#include <Stamina/Object.h>
#include <Stamina/ObjectImpl.h>
#include <Stamina/String.h>

namespace Stamina {

	class SocketError {
	public:

		enum ErrorType {
			etUserError,
			etWSAError,
			etProxyError
		};

		SocketError(ErrorType errorType, DWORD errorCode) 
			: _errorType(errorType), _errorCode(errorCode) {
		}

		SocketError(ErrorType errorType, const StringRef& msg)
			: _errorType(errorType), _message(msg), _errorCode(0) {
		}

		ErrorType getErrorType() const {
			return _errorType;
		}

		DWORD getErrorCode() const {
			return _errorCode;
		}

		bool hasMessage() const {
			return _message.getLength() != 0;
		}

		String getMessage () const {
			return _message;
		}
	protected:
		ErrorType _errorType;
		DWORD _errorCode;
		String _message;
	};
}

#endif	// __STAMINA_SOCKETERROR_H__