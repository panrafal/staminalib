/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#include "stdafx.h"


#include "Internet.h"
#include "RegEx.h"
#include "Helpers.h"

namespace Stamina {
	using std::string;

	Internet::Internet(HINTERNET session) {
		_hSession = session;
	}

	Internet::~Internet() {
		if (_hSession) InternetCloseHandle(_hSession);
	}

	void Internet::setReadTimeout(int timeout) {
		InternetSetOption(_hSession , INTERNET_OPTION_RECEIVE_TIMEOUT , (void*)&timeout , 4);
	}

	void Internet::setWriteTimeout(int timeout) {
		InternetSetOption(_hSession , INTERNET_OPTION_SEND_TIMEOUT , (void*)&timeout , 4);
	}

	oRequest Internet::handlePostWithRedirects(const std::string& url, const std::string& headers, const std::string& data, const char* referer, const char** acceptTypes, int flags) {
		std::string location = url;
		oConnection connection;
		oRequest request;
		while (1) {
			request = oRequest();
			if (!connection || !connection->isCompatible(location)) {
				connection = new Connection(this, location);
			}
			request = new Request(connection, getUrlQuery(location), Request::typePost, 0, referer, acceptTypes, flags | INTERNET_FLAG_NO_AUTO_REDIRECT/* | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD*/);

			request->sendThrowable(headers, data);
			int status = request->getStatusCode();
			if (status == 301 || status == 302) {
				string newLocation = request->getLocation();
				if (newLocation.empty() == false) {
					location = combineUrl(newLocation, location);
					continue; // jeszcze raz...
				}
			}
			break; // zwracamy co znaleŸliœmy...
		}
		return request;
	}


	string Internet::getUrlProtocol(const string& url) {
		CStdString protocol = RegEx::doGet("/^(\\w+):/", url.c_str(), 1);
		return protocol.ToLower();
	}
	string Internet::getUrlHost(const string& url) {
		return RegEx::doGet("#^\\w+:(?://(?:[^/@]+@)?|[^/@]+@)([^/@:?&]+)#", url.c_str(), 1);

	}
	string Internet::getUrlQuery(const string& url) {
		return RegEx::doGet("#^\\w+://[^/]+(/.+)#", url.c_str(), 1);
	}

	int Internet::getUrlPort(const string& url) {
		return 80;
	}

	std::string Internet::combineUrl(const std::string& url, const std::string& parent) {
		Stamina::RegEx regex;
		if (url.empty() || regex.match("#^\\w+://#", url.c_str())) {
			return url;
		}
		// wyci¹gamy poszczególne elementy URLa
		if (!regex.match("#^(\\w+://[^/]+/)([^\\?]+/)?([^\\?/]*)(\\?.*)?$#", parent.c_str()))
			return url;
		if (url[0] == '.' && (url.length() < 2 || url[1] != '.')) {
			// (http://..../) + (katalog/) + url bez kropki
			return regex[1] + regex[2] + url.substr(1);
		} else if (url[0] == '/') {
			// (http://..../) + url bez kreki
			return regex[1] + url.substr(1);
		} else {
			// (http://..../) + (katalog/) + url
			return regex[1] + regex[2] + url;
		}
	}


// -----------------------------------------------------------------------

	void Connection::init() {
		this->_type = typeHttp;
		this->_port = 0;
		this->_hConnect = 0;
	}

	Connection::Connection(const oInternet& internet, Type type, const string& host, int port, const char* user, const char* password) {
		this->init();
		this->_type = type;
		this->_host = host;
		this->_port = port;
		this->_internet = internet;
		if (user) {
			this->_user = user;
		}
		if (password) {
			this->_password = password;
		}
	}
	
	Connection::Connection(const oInternet& internet, const string& url)  {
		this->init();
		CStdString server = url;
		server.ToLower();
		if (server.substr(0,7) != "http://") {
			throw ExceptionBadUrl("Protocol: " + server);
		}
		server = url.substr(7 , url.find('/' , 8)-7);
        this->_host = server;
		this->_type = typeHttp;
		this->_port = 80;
		this->_internet = internet;
	}

	Connection::~Connection() {
		if (_hConnect) InternetCloseHandle(_hConnect);
	}

	void Connection::connect() {
		if (this->isConnected()) return;

		int service = INTERNET_SERVICE_HTTP;
		_hConnect = InternetConnect(_internet->getHandle(), _host.c_str(),
			_port, _user.empty() ? 0 : _user.c_str(), _password.empty() ? 0 : _password.c_str(), service, 0, 1);
		if (!_hConnect) {
			throw ExceptionCantConnect(this);
		}
		return;
	}

	Connection::Type Connection::getConnectionType(const string& protocol) {
		if (protocol == "http") {
			return typeHttp;
		}
		return typeUnknown;
	}

	bool Connection::isCompatible(const string& url) {
		return isCompatible( getConnectionType( Internet::getUrlProtocol(url) ), Internet::getUrlHost(url), Internet::getUrlPort(url) );
	}


	// -------------------------------------------------------------------



	Request::Request(const oConnection& connection, const std::string& uri, Type type, const char* version, const char* referer, const char** acceptTypes, int flags) {
		this->_uri = RegEx::doReplace("#^[a-z]+://[^/]+#i" , "", uri.c_str() );
		this->_type = type;
		this->_connection = connection;
		const char* verb;
		switch (_type) {
			case typePost:
				verb = "POST";
				break;
			case typeHead:
				verb = "HEAD";
				break;
			default:
				verb = "GET";
		}
		_hRequest = HttpOpenRequest(connection->getHandle(), verb,
			_uri.empty() ? "/" : _uri.c_str(), version, referer, acceptTypes, flags, 1);
		if (!_hRequest) {
			throw ExceptionBadRequest();
		}
	}
	
	Request::~Request() {
		if (_hRequest) InternetCloseHandle(_hRequest);
	}

	bool Request::send(const std::string& headers, const std::string& data) {
		return HttpSendRequest(_hRequest, headers.c_str(), headers.length(), (void*)data.c_str(), data.length());
	}

	int Request::getStatusCode() {
		DWORD read = 10;
		char buff [10];
		if (HttpQueryInfo(_hRequest , HTTP_QUERY_STATUS_CODE , buff , &read , 0)) {
			return atoi(buff);
		}
		return 0;
	}

	std::string Request::getStatusText() {
		return this->getInfoString(HTTP_QUERY_STATUS_TEXT, 100);
	}

	std::string Request::getInfoString(int type, DWORD size) {
		string txt;
		if (HttpQueryInfo(_hRequest , type , Stamina::stringBuffer(txt, size) , &size , 0)) {
			stringRelease(txt, size);
			return txt;
		} else {
			if (GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
				return getInfoString(type, size + 1);
			} else {
				return "";
			}
		}
	}

	int Request::getInfoInt(int type) {
		int value;
		DWORD size = 4;
		if (HttpQueryInfo(_hRequest , type | HTTP_QUERY_FLAG_NUMBER , &value , &size , 0)) {
			return value;
		} else {
			return 0;
		}
	}


	std::string Request::getLocation() {
		return getInfoString(HTTP_QUERY_LOCATION);
	}

	Date64 Request::getLastModificationTime() {
		SYSTEMTIME time;
		DWORD size = sizeof(time);
		if (HttpQueryInfo(_hRequest , HTTP_QUERY_LAST_MODIFIED | HTTP_QUERY_FLAG_SYSTEMTIME , &time , &size , 0)) {
			return Date64(time);
		} else {
			return Date64(false);
		}
	}


	std::string Request::getResponse()  {
		string response;
		char buff [501];
		int read;
		do {
			read = readResponse(buff, 500);
			if (read < 0) {
				throw ExceptionFailedRead(this);
			}
			buff[read] = 0;
			if (read) {
				response.append(buff, read);
			}
		} while (read);

		return response;
	}

	int Request::readResponse(char * buff, int buffSize) {
		DWORD read;
		if (!InternetReadFile(_hRequest , buff , buffSize , &read)) {
			return -1;
		} else {
			return read;
		}
	}
















};