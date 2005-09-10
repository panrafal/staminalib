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

#include <Wininet.h>
#include <boost/intrusive_ptr.hpp>
#include "Exception.h"
#include "ObjectImpl.h"
#include "Time64.h"

namespace Stamina {

	/**@defgroup Internet Internet
	Group of classes to help to manage internet connection
	and http's requests.
	@{
	*/

	/** @exception Handle string that contains an error.
	*/
	class ExceptionInternet: public ExceptionString {
	public:
		ExceptionInternet(const StringRef& reason):ExceptionString(reason) {
		}
	};

	typedef boost::intrusive_ptr<class Internet> oInternet;
	typedef boost::intrusive_ptr<class Connection> oConnection;
	typedef boost::intrusive_ptr<class Request> oRequest;


	/** @brief Represents an Internet connection.
	*/
	class Internet: public Stamina::SharedObject<Stamina::iSharedObject> {
	public:

		STAMINA_OBJECT_CLASS_VERSION(Stamina::Internet, iSharedObject, Version(1,0,0,0));

		/** Constructor
		@param session Handle to a valid session.
		*/
		Internet(HINTERNET session);

		/** Destructor
		*/
		~Internet();

		/** @brief Returns handle to a valid session.
		@return Handle to a valid session.
		*/
		HINTERNET getHandle() {
			return _hSession;
		}

		/** @brief Sets integer value that contains the time-out value	to receive
		a response to a request
		@param timeout Time-out value in milliseconds
		*/
        void setReadTimeout(int timeout);

		/** @brief Sets integer value that contains the time-out value to send a request
		@param timeout Time-out value in milliseconds
		*/
        void setWriteTimeout(int timeout);

		/** @brief Sends POST with redirects

        After status code \a 301 and \a 302 typically it is sent \b GET,
		with this function is sent \b POST.
		@param url String that contains the name of the target object of the specified HTTP verb.
		@param headers  String that contains the additional headers to be appended to the request.
		@param referer Pointer to a null-terminated string that specifies the URL of the document from which the URL in the request was obtained. If this parameter is NULL, no referrer is specified. 
		@param acceptTypes Pointer to a null-terminated array of strings that indicates media types accepted by the client. If this parameter is NULL, no types are accepted by the client.
		@param flags Internet option flags
		*/
		oRequest handlePostWithRedirects(const StringRef& url, const StringRef& headers, const StringRef& data, const char* referer = 0, const char** acceptTypes = 0, int flags = 0);


		/** @brief Returns protocol from url
		@param url String that contains the URL of target object.
		@return String that contains the protocol used in \a url.
		*/
		static String getUrlProtocol(const StringRef& url);

		/** @brief Returns Host from url
		@param url String that contains the URL of target object.
		@return String that contains the Host of target object.
		*/
		static String getUrlHost(const StringRef& url);

		/** @brief Returns url query
		@param url String that contains the URL of target object.
		@return String that contains the query value target object.
		*/
		static String getUrlQuery(const StringRef& url);

		/** @brief Returns url port
		@param url String that contains the URL of target object.
		@return Integer value that represents port number.
		*/
		static int getUrlPort(const StringRef& url);

		/** @brief Combine two urls.

		Completes \a url of elements from \a parent, if it's needed.
		Example:
		combineUrl("index.html" , "http://blah.com/") = "http://blah.com/index.html";
		combineUrl("/index.html" , "http://blah.com/katalog") = "http://blah.com/index.html";
		combineUrl("http://blurp.info/index.html" , "http://blah.com/") = "http://blurp.info/index.html";

		@param url String that contains the URL of target object.
		@param parent String that contains the URL of parent target object.
		@return String that contains combined url of \a url and \a parent.
		@todo Poprawiæ dokumentacjê tej funkcji.
		*/
		static String Internet::combineUrl(const StringRef& url, const StringRef& parent);

	private:
		HINTERNET _hSession;

	};

    /** @brief Connection class
	*/
	class Connection: public SharedObject<iSharedObject> {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::Connection, iSharedObject, Version(1,0,0,0));


		/** @brief Enumeration of connection types.
		*/
		enum Type {
			typeUnknown,	//< Unknown connection type
			typeHttp		//< HTTP connection type
		};

		/** @exception Handle connection object that causes an exception.
		*/
		class ExceptionCantConnect: public ExceptionInternet {
		public:
			ExceptionCantConnect(oConnection c):ExceptionInternet("Can't connect"), connection(c) {
			}
			oConnection connection;
		};

		/** @exception Handle bad url
		*/
		class ExceptionBadUrl: public ExceptionInternet {
		public:
			ExceptionBadUrl(const StringRef& s):ExceptionInternet(s) {
			}
		};


		/** @brief Constructor
		@param internet Pointer to Internet object.
		@param type Type of Connection.
		@param host Address of target object.
		@param port Port number on which connection is established.
		@param user String that contains username. 
		@param password String that contains password.
		@see Internet
		*/
		Connection(const oInternet& internet, Type type, const StringRef& host, int port, const char* user = 0, const char* password = 0);


		/** @brief Constructor
		@param internet Pointer to Internet object.
		@param url String that contains the URL of target object.
		*/
		Connection(const oInternet& internet, const StringRef& url) throw (...);

		/** @brief Destructor
		*/
		~Connection();

		/** @brief Returns handle to a valid session.
		@warning If connection is not established, it will connect.
		@return Handle to a valid session.
		*/
		HINTERNET getHandle() {
			this->connect();
			return _hConnect;
		}

		/** @brief Returns string that contains name of host.
		@return String that contains name of host.
		*/
		String getHost() {
			return this->_host;
		}

		/** @brief Returns true if connection is established,
		in other case returns false.
		@return True if connected, false if not.
		*/
		inline bool isConnected() {
			return this->_hConnect != 0;
		}

		/** @brief Try to establish connection.
		*/
		void connect() throw (...);

		/** @brief Returns connection type.
		@param protocol String that contains protocol name.
		@return Type enum.
		*/
		static Type getConnectionType(const StringRef& protocol);

		/** @brief Returns true if \a url suits established connection,
		in other case returns false.
		@param url String that contains the URL of target object.
		@return True if \a url suits established connection, in other case returns false.
		*/
		bool isCompatible(const StringRef& url);

		/** @brief Returns true if params suits established connection,
		in other case return false.
		@param type Type enum.
		@param host String that contains name of host.
		@param port Number of port.
		*/
		bool isCompatible(Type type, const StringRef& host, int port) {
			return type == _type && host == _host && port == _port;
		}

	private:

		void init();

		Type _type;
		String _host;
		int _port;
		String _user;
		String _password;

		oInternet _internet;
		HINTERNET _hConnect;
	};

	/** @brief Represents HTTP request.
	*/
	class Request: public SharedObject<iSharedObject> {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::Request, iSharedObject, Version(1,0,0,0));

		/** @exception Bad request.
		*/
		class ExceptionBadRequest: public ExceptionInternet {
		public:
			ExceptionBadRequest():ExceptionInternet("") {
			}
		};

		/** @exception Handle Request object and string that contains reason.
		*/
		class ExceptionRequest: public ExceptionInternet {
		public:
			ExceptionRequest(const oRequest& r, const StringRef& reason):ExceptionInternet(reason), request(r) {
			}
			oRequest request;
		};

		/** @exception Handle Request object and error code.
		*/
		class ExceptionFailedSend: public ExceptionRequest {
		public:
			ExceptionFailedSend(const oRequest& r, int errorCode):ExceptionRequest(r, "Failed send"), errorCode(errorCode) {}
			int errorCode;
		};

		/** @exception Handle Request object.
		*/
		class ExceptionFailedRead: public ExceptionRequest {
		public:
			ExceptionFailedRead(const oRequest& r):ExceptionRequest(r, "Failed read") {}
		};


		/** @brief Request type enumeration.
		*/
		enum Type {
			typeGet,	//< GET 
			typePost,	//< POST
			typeHead	//< HEAD
		};

		/** @brief Constructor
		@param connection Pointer to Connection object.
		@param uri String that contains the name of the target object.
		@param type Type of request (i.e. GET or POST)
		@param version Pointer to a null-terminated string that contains the HTTP version.
		@param referer Pointer to a null-terminated string that specifies the URL of the document from which the URL in the request was obtained.
		@param acceptTypes Pointer to a null-terminated array of strings that indicates media types accepted by the client. If this parameter is NULL, no types are accepted by the client.
		@param flags Internet options
		*/
		Request(const oConnection& connection, const StringRef& uri, Type type = typeGet, const char* version = 0, const char* referer = 0, const char** acceptTypes = 0, int flags = 0) throw (...);

		/** @brief Destructor.
		*/
		~Request();

		/** @brief Returns handle to a valid session.
		@return Handle to a valid session.
		*/
		HINTERNET getHandle() {
			return _hRequest;
		}

		/** @brief Returns pointer to Connection object.
		@return Pointer to Connection object.
		*/
		oConnection getConnection() {
			return _connection;
		}

        /** @brief Returns string that contains URI of Request object.
		@return String that contains URI of Request object.
		*/
		String getUri() {
			return _uri;
		}

		/** @brief Returns type of Request.
		@return Type of Request.
		*/
		Type getType() {
			return _type;
		}

		/** @brief Sends the specified request to the HTTP server.
		@param headers string that contains the additional headers to be appended to the request.
		@param data String that contains any optional data to be sent immediately
				after the request headers. This parameter is generally used for POST 
				and PUT operations. The optional data can be the resource or information
				being posted to the server.
		@return True if successfully sent, in other case false.
		*/
		bool send(const StringRef& headers="", const StringRef& data="");

		/** @brief Sends the specified request to the HTTP server.
		
		Similar to Request::send() function, but if it fails, it will throw
		an exception ExceptionFailedSend.
		*/
		void sendThrowable(const StringRef& headers="", const StringRef& data = "") throw (...) {
			if (!send(headers, data)) {
				throw ExceptionFailedSend(this, GetLastError());
			}
		}

		/** @brief Returns status code.
		@return Http query status code.
		*/
		int getStatusCode();

		/** @brief Returns string that contains status text.
		@return String that contains status text.
		*/
		String getStatusText();

		/** @brief Return string that contains specyfied type of info.
		@param type Type of info. The same that takes HttpQueryInfo function.
		@param size Size of buffor that receive the info.
		@return String that contains specyfied type of info.
		*/
		String getInfoString(int type, DWORD size = 100);

		/** @brief Returns content of specyfied header.
		@param header String that contains name of header.
		@param size Size of buffor that receive the content.
		@return Content of specyfied header.
		*/
		String getHeaderString(const StringRef& header, DWORD size = 255);

		/** @brief Returns the absolute URI (Uniform Resource Identifier) used in a Location response-header.
		@return String that contains URI.
		*/
		String getLocation();

		/** @brief Returns a specyfied integer value.

		Function includes the flag HTTP_QUERY_FLAG_NUMBER when retrives query info.
		@param type Type; The same that takes HttpQueryInfo function.
		@return Integer value of specyfied info.
		*/
		int getInfoInt(int type);

		/** @brief Returns length of received content.
		@return Length of content.
		*/
		int getContentLength() {
			this->getInfoInt(HTTP_QUERY_CONTENT_LENGTH);
		}

		/** @brief Returns last modification time.
		@return Last modification time.
		*/
		Date64 getLastModificationTime();

		/** @brief Reads the server's response and return it as a string.
		
		If can't read throws an excpetion ExceptionCantRead
		@return String that contains server's response
		*/
		String getResponse() throw (...);

		/** @brief Reads the server's response
		@param buff Pointer to a buffer that receives the response.
		@param buffSize Size of buffor.
		@return Size of read response or if it fails, it will returns -1
		*/
		int readResponse(char * buff, int buffSize);

	private:
		Type _type;
		String _uri;
		oConnection _connection;
		HINTERNET _hRequest;
	};

	STAMINA_REGISTER_CLASS_VERSION(Internet);
	STAMINA_REGISTER_CLASS_VERSION(Connection);
	STAMINA_REGISTER_CLASS_VERSION(Request);
	// example of use Request object
	/** @code
	int runRemote( HINTERNET hInternet, const StringRef& command, StringRef &out )
	{
		Stamina::oInternet net( new Stamina::Internet( hInternet ) );
		Stamina::oConnection conn( new Stamina::Connection( net, command ) );
		Stamina::oRequest req( new Stamina::Request( conn, command ) );

		try
		{
            req->sendThrowable();
			out = req->getResponse();
			return Stamina::chtoint( req->getHeaderString( "Error" ).c_str() );
		}
		catch(Stamina::Request::ExceptionRequest e)
		{
			MessageBox( 0, e.getReason().c_str(), command.c_str(), MB_ICONINFORMATION );
		}

		return -1;
	}
	@endcode
	*/
	/** @} */ // end of Internet group


};
