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

	bool SOCKS5Proxy::connect(const Proxy& proxy) {
		this->_host = proxy.proxyHost;
		this->_port = proxy.proxyPort;
		_proxy = proxy;
		return _socket->connect(_host, _port);
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
				sendProxyHandshake();
				break;
			case mServer:
				break;
		}
	}

	void SOCKS5Proxy::sendProxyHandshake() {

		ByteBuffer buffer;

		if (_proxy.username.getLength() != 0) {
			buffer.append((unsigned char*)5, 1);	// VER
			buffer.append((unsigned char*)2, 1);	// NMETHODS
			buffer.append((unsigned char*)0, 1);	// NO AUTHENTICATION REQUIRED
			buffer.append((unsigned char*)2, 1);	// USERNAME/PASSWORD
		} else {
			buffer.append((unsigned char*)5, 1);	// VER
			buffer.append((unsigned char*)1, 1);	// NMETHODS
			buffer.append((unsigned char*)0, 1);	// NO AUTHENTICATION REQUIRED
		}

		_proxyState = psVersionSend;
		_socket->write(buffer);
	}

	void SOCKS5Proxy::onClose() {
		this->evtOnClose();
	}

	void SOCKS5Proxy::onProxyHandshakeReply() {
		ByteBuffer buffer;

		_socket->read(buffer);

		unsigned char* buff = buffer.getBuffer();
		if (buff[0] != 5)
			this->evtOnError(SocketError(SocketError::etProxyError, "Invalid SOCKS5 server version."));

		switch (buff[1]) {
			case 0:
				sendProxyConnect();
				break;
			case 2:
				sendProxyAuthorization();
				break;
			case 255:
				this->evtOnError(SocketError(SocketError::etProxyError, "No acceptable methods."));
				break;
		}
	}

	void SOCKS5Proxy::onReceived() {
		switch (_proxyState) {
			case psVersionSend:
				onProxyHandshakeReply();
				break;
			case psAuthSend:
				onProxyAuthorizationReply();
				break;
			case psConnectSend:
				onProxyConnectReply();
				break;
		}
	}

	void SOCKS5Proxy::onError(const SocketError& error) {
		this->evtOnError(error);
	}

	void SOCKS5Proxy::sendProxyAuthorization() {
		ByteBuffer buffer;

		buffer.append((unsigned char*)1, 1); // Version

		// username
		if (_proxy.username.getLength() <= 255) {
			buffer.append((unsigned char*)_proxy.username.getLength(), 1);
			buffer.append((unsigned char*)_proxy.username.c_str(), _proxy.username.getLength());
		} else {
			buffer.append((unsigned char*)255, 1);
			buffer.append((unsigned char*)_proxy.username.substr(0, 255).c_str(), 255);
		}

		// password
		if (_proxy.password.getLength() <= 255) {
			buffer.append((unsigned char*)_proxy.password.getLength(), 1);
			buffer.append((unsigned char*)_proxy.password.c_str(), _proxy.password.getLength());
		} else {
			buffer.append((unsigned char*)255, 1);
			buffer.append((unsigned char*)_proxy.password.substr(0, 255).c_str(), 255);
		}

		_proxyState = psAuthSend;
		_socket->write(buffer);		
	}

	void SOCKS5Proxy::onProxyAuthorizationReply() {
		ByteBuffer buffer;

		_socket->read(buffer);

		if (buffer.getLength() < 2) {
			this->evtOnError(SocketError(SocketError::etProxyError, "Invalid SOCKS5 server reply on authorization."));
			_proxyState = psNotConnected;
			_socket->close();
			return;
		}

		if (buffer.getBuffer()[1] == 0) {
			sendProxyConnect();
		} else {
			this->evtOnError(SocketError(SocketError::etProxyError, "SOCKS5 server authorization failed."));
			_proxyState = psNotConnected;
			_socket->close();
			return;
		}
	}

	void SOCKS5Proxy::sendProxyConnect() {
		ByteBuffer buffer;

		buffer.append((unsigned char*)5, 1); // VER
		buffer.append((unsigned char*)1, 1); // CMD CONNECT
		buffer.append((unsigned char*)0, 1); // RSV
		buffer.append((unsigned char*)3, 1); // ATYP DOMAINNAME

		if (_proxy.destHost.getLength() <= 255) {
			buffer.append((unsigned char*)_proxy.destHost.getLength(), 1);
			buffer.append((unsigned char*)_proxy.destHost.c_str(), _proxy.destHost.getLength());
		} else {
			buffer.append((unsigned char*)255, 1);
			buffer.append((unsigned char*)_proxy.destHost.substr(0, 255).c_str(), 255);
		}

		buffer.append((unsigned char*)_proxy.destPort/256, 1);
		buffer.append((unsigned char*)_proxy.destPort%256, 1);

		_proxyState = psConnectSend;
		_socket->write(buffer);
	}

	void SOCKS5Proxy::onProxyConnectReply() {
		ByteBuffer buffer;

		_socket->read(buffer);

		unsigned char* buff = buffer.getBuffer();

		if (buffer.getLength() < 7 || buff[0] != 5 || buff[2] != 0) {
			this->evtOnError(SocketError(SocketError::etProxyError, "Invalid SOCKS5 server reply on connect command."));
			_proxyState = psNotConnected;
			_socket->close();
			return;
		}

		int min_len = 7;

		switch (buff[3]) {
			case 1: // IPv4
				min_len = 10;
				break;
			case 3: // Domainname
				min_len = buff[4] + 7;
				break;
			case 4: // IPv6
				min_len = 22;
				break;
		}

		if (buffer.getLength() < min_len) {
			this->evtOnError(SocketError(SocketError::etProxyError, "Invalid SOCKS5 server reply on connect command."));
			_proxyState = psNotConnected;
			_socket->close();
			return;
		}

		switch (buff[1]) {
			case 0: // succeeded
				_proxyState = psConnected;
				this->_state = stConnected;
				this->_host = _proxy.destHost;
				this->_port = _proxy.destPort;
				this->evtOnConnected();
				break;
			case 1:
				this->evtOnError(SocketError(SocketError::etProxyError, "General SOCKS server failure."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
			case 2:
				this->evtOnError(SocketError(SocketError::etProxyError, "Connection not allowed by ruleset."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
			case 3:
				this->evtOnError(SocketError(SocketError::etProxyError, "Network unreachable."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
			case 4:
				this->evtOnError(SocketError(SocketError::etProxyError, "Host unreachable."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
			case 5:
				this->evtOnError(SocketError(SocketError::etProxyError, "Connection refused."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
			case 6:
				this->evtOnError(SocketError(SocketError::etProxyError, "TTL expired."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
			case 7:
				this->evtOnError(SocketError(SocketError::etProxyError, "Command not supported."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
			case 8:
				this->evtOnError(SocketError(SocketError::etProxyError, "Address type not supported."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
			default:
				this->evtOnError(SocketError(SocketError::etProxyError, "Unexpected reply from SOCKS5 server."));
				_proxyState = psNotConnected;
				_socket->close();
				break;
		}
	}
}