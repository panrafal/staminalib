#ifndef __STAMINA_PROXY_H__
#define __STAMINA_PROXY_H__

#include <Stamina/Object.h>
#include <Stamina/String.h>
#include <Stamina/StringBuffer.h>

#include <boost/signal.hpp>

namespace Stamina {
	/// Proxy types
	enum ProxyType {
		proxySocks5	/// Uses SOCKS5 proxy
	};
	/** This class represents proxy settings and necessary functions, 
	which allow to connect to proxy server or accept client connection.
	*/
	class Proxy :
		public iObject {
	public:
		/// Types of proxy work
		enum Mode {
			modeClient,	/// Proxy works as a client (trying to connect to prox server)
			modeServer	/// Proxy works as a server (authorizes incoming connection)
		};
		/// Errors
		enum Error {
			erProxyType,	/// This type of proxy is not supported
			erPacket,		/// Wrong packet received
			erProxyVer,		/// Wrong proxy version
			erNoMethods,	/// No acceptable authorization method
			erLogin,		/// Error in login
			erServer,		/// Server error
			erAType			/// Wrong Socks atyp
		};
		/// Constructs proxy in client mode with no authorization
		Proxy(ProxyType type, const StringRef& host, unsigned int port);
		/// Constructs proxy in client mode with username/password authorization
		Proxy(ProxyType type, const StringRef& host, unsigned int port, const StringRef& username, const StringRef& password);
		/** In client mode checks if authorization was successfull.
		*/
		bool operator!() {
			return _authorized;
		}
		/** Incoming data */
		Proxy& operator>>(const ByteBuffer& in);

		/** Signal fires when proxy object must send data */
		boost::signal<void (const ByteBuffer&)> evtSend;
		/** Signal fires when establish connection */
		boost::signal<void (const StringRef&, unsigned int)> evtOnConnected;

		/** Starts authorization process (client mode) */
		bool authorize();

		/** Gets error type */
		inline Error getLastError() const {
			return _error;
		}
	private:
		void handleSocks5Head(const ByteBuffer& reply);
		void handleSocks5MethodReply(const ByteBuffer& reply);
		void handleSocks5AuthReply(const ByteBuffer& reply);
		void handleSocks5ConnectReply(const ByteBuffer& reply);
		void sendSocks5Connect();
		void sendSocks5Auth();
	private:
		/// Authorization type.
		enum authType {
			authNone,		/// No authorization
			authUsername	/// Plain text username/password authorization
		};
		enum State {
			stSocks5MethodsSent,
			stSocks5ConnectSent,
			stSocks5AuthSent
		};
		ProxyType _type;
		bool _authorized;
		authType _auth;
		String _username;
		String _password;
		String _host;
		unsigned int _port;
		State _state;
		Error _error;
	};
}

#endif	// __STAMINA_PROXY_H__