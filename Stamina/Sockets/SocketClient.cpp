/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */


#include "stdafx.h"
#include "SocketClient.h"

namespace Stamina {
	
    SocketClient::SocketClient(const oSocket& socket)
		: SocketController(socket) {
		_socket->setEvtOnConnected(boost::bind(&SocketClient::onConnected, this));
		_socket->setEvtOnReceived(boost::bind(&SocketClient::onReceived, this, _1));
	}

	void SocketClient::onConnected() {
		_connected.set();
	}

	void SocketClient::onReceived(const ByteBuffer& buff) {
        _received.set();
		_buff.assign(buff);
	}

	bool SocketClient::connect(const StringRef& host, unsigned int port, unsigned int timeout) {
		if (!_socket->connect(host, port))
			return false;
		if (_connected.wait(timeout) == WAIT_OBJECT_0)
			return true;
		return false;
	}

	bool SocketClient::connect(const StringRef& host, unsigned int port) {
		return _socket->connect(host, port);
	}

	SocketClient& SocketClient::operator <<(const ByteBuffer& right) {
		_socket->send(right);
		return (*this);
	}

	SocketClient& SocketClient::operator <<(const StringRef& right) {
		ByteBuffer buff;
		buff.assign((unsigned char*)right.c_str(), right.getLength());
		return (*this) << buff;
	}

	SocketClient& SocketClient::operator >>(ByteBuffer& right) {
		_sync = true;
		if (_received.wait(INFINITE) == WAIT_OBJECT_0) {
			right.assign(_buff);
			_buff.reset();
		}
		return (*this);
	}

	SocketClient& SocketClient::operator >>(String& right) {
		ByteBuffer buff;
		(*this) >> buff;
		if (buff.getBufferSize())
			right = (char*)buff.getBuffer();
		return (*this);
	}

	oSocketClient& operator<<(oSocketClient& client, const ByteBuffer& buff) {
		(*client.get()) << buff;
		return client;
	}

	oSocketClient& operator<<(oSocketClient& client, const String& txt) {
		(*client.get()) << txt;
		return client;
	}

	oSocketClient& operator>>(oSocketClient& client, ByteBuffer& buff) {
		(*client.get()) >> buff;
		return client;
	}

	oSocketClient& operator>>(oSocketClient& client, String& txt) {
		(*client.get()) >> txt;
		return client;
	}
}