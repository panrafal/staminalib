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

namespace Stamina {
	class SOCKS5Proxy : public SocketClient {
	public:
		enum Mode {
			mClient,	// Proxy runs in client mode
			mServer		// Proxy runs in server mode
		};

		enum ProxyState {
			psNotConnected,		// Proxy is not connected
			psVersionSend		// In client mode, socks version sent
		};

		STAMINA_OBJECT_CLASS_VERSION(Stamina::SOCKS5Proxy, SocketClient, Version(0,1,0,0));

		SOCKS5Proxy(const oSocketClient& socket, Mode mode = mClient);
		
		bool connect(const StringRef& host, Port port);
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
	private:
		void onConnected();
		void onConnectedClient();
		void onReceived();
		void onClose();
		void onError(unsigned int error);
	};
}

#endif	// __STAMINA_SOCKS5PROXY_H__