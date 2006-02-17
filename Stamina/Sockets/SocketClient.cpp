#include "stdafx.h"
#include "SocketClient.h"

namespace Stamina {
	
    SocketClient::SocketClient(const oSocket& socket)
		: _socket(socket) {
		_socket->evtOnError.connect(boost::bind(&SocketClient::onError, this, _1));
		_socket->evtOnClose.connect(boost::bind(&SocketClient::onClose, this));
		_socket->evtOnConnected.connect(boost::bind(&SocketClient::onConnected, this));
		_socket->evtOnReceived.connect(boost::bind(&SocketClient::onReceived, this, _1));
	}

	void SocketClient::onClose() {
		evtOnClose();
	}

	void SocketClient::onConnected() {
		_connected.set();
		evtOnConnected();
	}

	void SocketClient::onError(unsigned int err) {
		evtOnError(err);
	}

	void SocketClient::onReceived(const ByteBuffer& buff) {
        _received.set();
		_buff.assign(buff);
		evtOnReceived(buff);
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

	oSocketClient& operator>>(oSocketClient& client, ByteBuffer& buff) {
		(*client.get()) >> buff;
		return client;
	}
}