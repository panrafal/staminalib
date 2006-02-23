#include "stdafx.h"
#include "Proxy.h"

namespace Stamina {
	Proxy::Proxy(ProxyType type, const StringRef& host, unsigned int port)
		: _authorized(false), _auth(authNone), _type(type), _host(host), _port(port) {
	}

	Proxy::Proxy(ProxyType type, const StringRef& host, unsigned int port, const StringRef& username, const StringRef& password)
		: _authorized(false), _auth(authUsername), _username(username),
		_password(password), _type(type), _host(host), _port(port) {
	}

	bool Proxy::authorize() {
		ByteBuffer head;
		switch (_type) {
			case proxySocks5:
				head.append((unsigned char*)5, 1);	// SOCKS ver
				head.append((unsigned char*)2, 1);	// NMETHOD
				head.append((unsigned char*)0, 1);	// NO AUTHENTICATION REQUIRED
				head.append((unsigned char*)2, 1);	// USERNAME/PASSWORD
				_state = stSocks5MethodsSent;
				break;
			default:
				_error = erProxyType;
				return false;
		}
		evtSend(head);
		return true;
	}

	Proxy& Proxy::operator>>(const ByteBuffer& in) {
		switch (_type) {
			case proxySocks5:
				handleSocks5Head(in);
				break;
			default:
				_error = erProxyType;
		}
		return (*this);
	}

	void Proxy::handleSocks5Head(const ByteBuffer& reply) {
		switch (_state) {
			case stSocks5MethodsSent:
				handleSocks5MethodReply(reply);
				break;
			case stSocks5AuthSent:
				handleSocks5AuthReply(reply);
				break;
			case stSocks5ConnectSent:
				handleSocks5ConnectReply(reply);
				break;
		}
	}

	void Proxy::handleSocks5ConnectReply(const ByteBuffer& reply) {
		if (reply.getLength() < 7) {	// min length of packet
			_error = erPacket;
			return;
		}
		unsigned char* data = reply.getBuffer();
		ByteBuffer buff;
		String host;
		unsigned int port;

		if (data[0] != 5) {	// SOCKS ver
			_error = erProxyVer;
			return;
		}
		// REP
		if (data[1] != 0) { /// @todo: ropoznawanie typu bledu jaki zwraca serwer
			_error = erServer;
			return;
		}
		// RSV
		if (data[2] != 0) {
			_error = erPacket;
			return;
		}
		// ATYP
		switch (data[3]) {
			case 1:		// IPv4
				buff.assign(data+5, 10);
				host = (char*)buff.getBuffer();
				port = data[15]*256 + data[16];
				break;
			case 3:		// DOMAINNAME
				buff.assign(data+6, (int)data[4]);
				host = (char*)buff.getBuffer();
				port = data[5+(int)data[4]+1]*256 + data[5+(int)data[4]+2];
				break;
			case 4:		// IPv6
				buff.assign(data+5, 22);
				host = (char*)buff.getBuffer();
				port = data[27]*256 + data[28];
				break;
			default:
				_error = erAType;
				return;
		}
		evtOnConnected(host, port);
	}

	void Proxy::handleSocks5AuthReply(const ByteBuffer& reply) {
		if (reply.getLength() != 2) {
			_error = erPacket;
			return;
		}
		
		unsigned char* data = reply.getBuffer();
		if (data[0] != 1) {
			_error = erProxyVer;
			return;
		}

		if (data[1] != 0) {
			_error = erLogin;
			return;
		}

		sendSocks5Connect();
	}

	void Proxy::handleSocks5MethodReply(const ByteBuffer& reply) {
		unsigned char* data = reply.getBuffer();
		if (reply.getLength() > 2) {
			_error = erPacket;
			return;
		}
		if (data[0] != 5) {	// Wrong Socks ver
			_error = erProxyVer;
			return;
		}
		switch (data[1]) {
			case 0:		// no authentication required
				sendSocks5Connect();
				break;
			case 2:		// username authentication required
				sendSocks5Auth();
				break;
			default:
				_error = erNoMethods;
		}
	}

	void Proxy::sendSocks5Connect() {
		ByteBuffer header;

		header.append((unsigned char*)5, 1);		// SOCKS ver
		header.append((unsigned char*)1, 1);		// CMD CONNECT
		header.append((unsigned char*)0, 1);		// RSV
		header.append((unsigned char*)3, 1);		// ATYP DOMAINNAME
		
		// DST.ADDR
		if (_host.getLength() > 255) {
			header.append((unsigned char*)255, 1);
			header.append((unsigned char*)_host.substr(0, 255).c_str(), 255);
		}
		else {
			header.append((unsigned char*)_host.length(), 1);
			header.append((unsigned char*)_host.c_str(), _host.length());
		}
		// DST.PORT
		header.append((unsigned char*)(_port/256), 1);
		header.append((unsigned char*)(_port%256), 1);
		
		_state = stSocks5ConnectSent;
		evtSend(header);
	}

	void Proxy::sendSocks5Auth() {
		ByteBuffer header;

		header.append((unsigned char*)1, 1);	// VER

		if (_username.length() > 255) {
			header.append((unsigned char*)255, 1); // ULEN
			header.append((unsigned char*)_username.c_str(), 255);	// UNAME
		}
		else {
			header.append((unsigned char*)_username.length(), 1);	// ULEN
			header.append((unsigned char*)_username.c_str(), _username.length()); // UNAME
		}

		if (_password.length() > 255) {
			header.append((unsigned char*)255, 1); // PLEN
			header.append((unsigned char*)_password.c_str(), 255);	// PASSWD
		}
		else {
			header.append((unsigned char*)_password.length(), 1);	// PLEN
			header.append((unsigned char*)_password.c_str(), _password.length()); // PASSWD
		}
		_state = stSocks5AuthSent;
		evtSend(header);
	}
}