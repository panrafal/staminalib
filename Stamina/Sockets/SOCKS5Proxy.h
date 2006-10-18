/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#ifndef __STAMINA_SOCKS5PROXY_H__
#define __STAMINA_SOCKS5PROXY_H__

#include "SocketClient.h"
#include <Stamina/String.h>

namespace Stamina {
	class SOCKS5Proxy : public SocketClient {
	public:

		/**
		 * Describes proxy information.
		 */
		struct Proxy {
			String proxyHost;	/// Proxy server hostname.
			Port proxyPort;		/// Proxy server port.
			String destHost;	/// Destination host.
			Port destPort;		/// Destination port.
			String username;	/// Username if proxy needs authorization.
			String password;	/// Password if proxy needs authorization.
		};

		enum Mode {
			mClient,	// Proxy runs in client mode
			mServer		// Proxy runs in server mode
		};

		enum ProxyState {
			psNotConnected,		// Proxy is not connected
			psVersionSend,		// In client mode, socks version sent
			psAuthSend,
			psConnectSend,
			psConnected
		};

		STAMINA_OBJECT_CLASS_VERSION(Stamina::SOCKS5Proxy, SocketClient, Version(0,1,0,0));

		SOCKS5Proxy(const oSocketClient& socket, Mode mode = mClient);
		
		bool connect(const Proxy& proxy);
		void close();

		int write(const char* data, Size size);
		int read(char* data, Size size);

		inline Mode getMode() const {
			return _mode;
		}

	protected:
		oSocketClient _socket;
		Mode _mode;
		ProxyState _proxyState;
		Proxy _proxy;

	private:
		void onConnected();
		void sendProxyHandshake();
		void onProxyHandshakeReply();
		void sendProxyConnect();
		void onProxyConnectReply();
		void sendProxyAuthorization();
		void onProxyAuthorizationReply();
		void onMethodSelected();
		void onReceived();
		void onClose();
		void onError(const SocketError& error);

		// hide method
		bool connect(const StringRef& host, Port port) {
      return false;
		}
	};
}

#endif	// __STAMINA_SOCKS5PROXY_H__