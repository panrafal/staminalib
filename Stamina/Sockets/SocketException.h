/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */

#ifndef __STAMINA_SOCKETEXCEPTION_H__
#define __STAMINA_SOCKETEXCEPTION_H__

#include <Stamina/Exception.h>
#include <Stamina/String.h>

namespace Stamina {
	class SocketException : public Exception {
	public:
		STAMINA_OBJECT_CLASS(Stamina::SocketException, Exception);
		
		SocketException() {}
		SocketException(const StringRef& reason) : _reason(reason) {}

		virtual String getReason() const {
			return _reason;
		}

		inline bool hasReason() const {
			return !_reason.empty();
		}
		
		inline int getErrorCode() const {
			return _errCode;
		}

		virtual String toString(iStringFormatter *f = 0) const {
			return getReason();
		}
	protected:
		String _reason;
		int _errCode;
	};

	class InvalidSocketException : public SocketException {
	public:
		InvalidSocketException() 
			: SocketException("Invalid socket descriptor.") {}
	};


	class NullPointerException : public SocketException {
	};


	class WSASocketException : public SocketException {
	public:
		STAMINA_OBJECT_CLASS(Stamina::WSASocketException, Stamina::SocketException);

		WSASocketException(int errCode) {
			LPVOID lpMsgBuf;
			if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR)&lpMsgBuf, 0, NULL))
			{
				this->_reason = (wchar_t*)lpMsgBuf;
				this->_errCode = errCode;
                // Free the buffer.
                LocalFree(lpMsgBuf);
			}
		}
	};
}

#endif	// __STAMINA_SOCKETEXCEPTION_H__