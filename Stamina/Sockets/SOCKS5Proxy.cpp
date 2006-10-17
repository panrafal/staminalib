/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#include "stdafx.h"
#include "SOCKS5Proxy.h"

namespace Stamina {
	SOCKS5Proxy::SOCKS5Proxy(const oSocketClient& socket, Mode mode) 
		: _socket(socket), _mode(mode), _proxyState(psNotConnected) {
		
		// connect signals
		_socket->evtOnConnected.connect(boost::bind(&SOCKS5Proxy::onConnected, this));
		_socket->evtOnClose.connect(boost::bind(&SOCKS5Proxy::onClose, this));
		_socket->evtOnReceived.connect(boost::bind(&SOCKS5Proxy::onReceived, this));
		_socket->evtOnError.connect(boost::bind(&SOCKS5Proxy::onError, this, _1));
	}

	bool SOCKS5Proxy::connect(const StringRef& host, Port port) {
		this->_host = host;
		this->_port = port;
		return _socket->connect(host, port);
	}

	void SOCKS5Proxy::close() {
		this->_state = stDisconnected;
		_socket->close();
	}

	int SOCKS5Proxy::write(const char* data, Size size) {
		return _socket->write(data, size);
	}

	int SOCKS5Proxy::read(char* data, Size size) {
		return _socket->read(data, size);
	}

	void SOCKS5Proxy::onConnected() {
		switch (_mode) {
			case mClient:
				onConnectedClient();
				break;
			case mServer:
				break;
		}
	}

	void SOCKS5Proxy::onConnectedClient() {

	}

	void SOCKS5Proxy::onClose() {
	}

	void SOCKS5Proxy::onReceived() {
	}

	void SOCKS5Proxy::onError(unsigned int error) {
		this->evtOnError(error);
	}
}