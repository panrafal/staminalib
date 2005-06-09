/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once

#include <Wininet.h>
#include <boost/intrusive_ptr.hpp>
#include "Exception.h"
#include "ObjectImpl.h"
#include "Time64.h"

namespace Stamina {

	class ExceptionInternet: public ExceptionString {
	public:
		ExceptionInternet(const std::string& reason):ExceptionString(reason) {
		}
	};

	typedef boost::intrusive_ptr<class Internet> oInternet;
	typedef boost::intrusive_ptr<class Connection> oConnection;
	typedef boost::intrusive_ptr<class Request> oRequest;


	class Internet: public Stamina::SharedObject<Stamina::iSharedObject> {
	public:

		Internet(HINTERNET session);

		~Internet();

		HINTERNET getHandle() {
			return _hSession;
		}

        void setReadTimeout(int timeout);
        void setWriteTimeout(int timeout);

		oRequest handlePostWithRedirects(const std::string& url, const std::string& headers, const std::string& data, const char* referer = 0, const char** acceptTypes = 0, int flags = 0);


		static std::string getUrlProtocol(const std::string& url);
		static std::string getUrlHost(const std::string& url);
		static std::string getUrlQuery(const std::string& url);
		static int getUrlPort(const std::string& url);

		static std::string Internet::combineUrl(const std::string& url, const std::string& parent);

	private:
		HINTERNET _hSession;

	};

	class Connection: public SharedObject<iSharedObject> {
	public:
		enum Type {
			typeUnknown,
			typeHttp
		};

		class ExceptionCantConnect: public ExceptionInternet {
		public:
			ExceptionCantConnect(oConnection c):ExceptionInternet("Can't connect"), connection(c) {
			}
			oConnection connection;
		};

		class ExceptionBadUrl: public ExceptionInternet {
		public:
			ExceptionBadUrl(std::string s):ExceptionInternet(s) {
			}
		};


		Connection(const oInternet& internet, Type type, const std::string& host, int port, const char* user = 0, const char* password = 0);

		Connection(const oInternet& internet, const std::string& url) throw (...);

		~Connection();

		HINTERNET getHandle() {
			this->connect();
			return _hConnect;
		}

		CStdString getHost() {
			return this->_host;
		}


		inline bool isConnected() {
			return this->_hConnect != 0;
		}

		void connect() throw (...);

		static Type getConnectionType(const std::string& protocol);

		bool isCompatible(const std::string& url);
		bool isCompatible(Type type, const std::string& host, int port) {
			return type == _type && host == _host && port == _port;
		}

	private:

		void init();

		Type _type;
		std::string _host;
		int _port;
		std::string _user;
		std::string _password;

		oInternet _internet;
		HINTERNET _hConnect;
	};

	class Request: public SharedObject<iSharedObject> {
	public:

		class ExceptionBadRequest: public ExceptionInternet {
		public:
			ExceptionBadRequest():ExceptionInternet("") {
			}
		};
		class ExceptionRequest: public ExceptionInternet {
		public:
			ExceptionRequest(const oRequest& r, const std::string& reason):ExceptionInternet(reason), request(r) {
			}
			oRequest request;
		};
		class ExceptionFailedSend: public ExceptionRequest {
		public:
			ExceptionFailedSend(const oRequest& r, int errorCode):ExceptionRequest(r, "Failed send"), errorCode(errorCode) {}
			int errorCode;
		};
		class ExceptionFailedRead: public ExceptionRequest {
		public:
			ExceptionFailedRead(const oRequest& r):ExceptionRequest(r, "Failed read") {}
		};


		enum Type {
			typeGet, typePost, typeHead
		};

		Request(const oConnection& connection, const std::string& uri, Type type = typeGet, const char* version = 0, const char* referer = 0, const char** acceptTypes = 0, int flags = 0) throw (...);
		~Request();

		HINTERNET getHandle() {
			return _hRequest;
		}

		oConnection getConnection() {
			return _connection;
		}

		std::string getUri() {
			return _uri;
		}

		Type getType() {
			return _type;
		}

		bool send(const std::string& headers="", const std::string& data="");

		void sendThrowable(const std::string& headers="", const std::string& data = "") throw (...) {
			if (!send(headers, data)) {
				throw ExceptionFailedSend(this, GetLastError());
			}
		}

		int getStatusCode();
		std::string getStatusText();
		std::string getInfoString(int type, DWORD size = 100);
		std::string getLocation();
		int getInfoInt(int type);

		int getContentLength() {
			this->getInfoInt(HTTP_QUERY_CONTENT_LENGTH);
		}

		Date64 getLastModificationTime();

		std::string getResponse() throw (...);
		int readResponse(char * buff, int buffSize);

	private:
		Type _type;
		std::string _uri;
		oConnection _connection;
		HINTERNET _hRequest;
	};


};