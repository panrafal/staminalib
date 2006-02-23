#include "stdafx.h"
#include "Socket.h"

namespace Stamina {

	bool Socket::connect(const StringRef& host, unsigned int port) {
		if (_state != stOffline)
			return false;
	
		_host = host;
		_port = port;
		_state = stConnecting;

		_threads->run(boost::bind(&Socket::connecting, this), "Socket::connecting");
		return true;
	}

	bool Socket::setEvtOnError(fUnsigned f) {
		if (f.empty())
			return false;
		_evtOnError.connect(f);
		return true;
	}

	bool Socket::setEvtOnAccept(fSocket f) {
		if (f.empty())
			return false;
		_evtOnAccept.connect(f);
		return true;
	}

	bool Socket::setEvtOnConnected(fVoid f) {
        if (f.empty())
			return false;
		_evtOnConnected.connect(f);
		return true;
	}

	bool Socket::setEvtOnReceived(fBuffer f) {
		if (f.empty())
			return false;
		_evtOnReceived.connect(f);
		if (!_buffer.isEmpty()) {
			_evtOnReceived(_buffer);
			_buffer.reset();
		}
		return true;
	}

	bool Socket::setEvtOnClose(fVoid f) {
		if (f.empty())
			return false;
		_evtOnClose.connect(f);
		return true;
	}

	bool Socket::sendProxyHandshake() {
		ByteBuffer head;
		switch (_proxy) {
			case proxySOCKS5:
				head.append((unsigned char*)5, 1);	// SOCKS ver
				head.append((unsigned char*)2, 1);	// number of authentication methods
				head.append((unsigned char*)0, 1);	// no authentication
				head.append((unsigned char*)2, 1);	// plain text username/password
				break;
			default:
				return false;
		}
		send(head);
		return true;
	}

	bool Socket::handleProxyHandshakeReply(const ByteBuffer& reply) {
		unsigned char* t = reply.getBuffer();
		switch (_proxy) {
			case proxySOCKS5: {
					if (t[0] != 5)	// wrong SOCKS version
						return false;
					switch (t[1]) {
						case 0:	// no authorization
							break;
						default:	// not supported method
							return false;
					}
				}
				break;
			default:
				return false;
		}
	}
}