#include "stdafx.h"
#include "TCPSocket.h"
#include <vector>
using namespace std;

namespace Stamina {
	bool TCPSocket::_wsa = false;

	TCPSocket::TCPSocket(int major, int minor) {
		if (!_wsa) {
			WSAData wsaData;
			WORD wVersionRequested;

			wVersionRequested = MAKEWORD(major, minor);
			if (WSAStartup(wVersionRequested, &wsaData))
				throw ExceptionSocket(WSAGetLastError());
			_wsa = true;
		}

		if ((_event = WSACreateEvent()) == WSA_INVALID_EVENT)
			throw ExceptionSocket(WSAGetLastError());

		_state = stOffline;
	}

	TCPSocket::TCPSocket(SOCKET socket, const StringRef& host, unsigned int port) {
		if (!socket)
			throw ExceptionSocket("Invalid socket!");
		
		_socket = socket;
		
		if ((_event = WSACreateEvent()) == WSA_INVALID_EVENT ||
			WSAEventSelect(_socket, _event, FD_CONNECT|FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR)
		{
			closesocket(_socket);
			throw ExceptionSocket(WSAGetLastError());
		}

		_host = host;
		_port = port;
		_state = stConnected;
	}

	TCPSocket::TCPSocket(const TCPSocket& socket) {
		(*this) = socket;
	}

	TCPSocket::~TCPSocket() {
		WSACloseEvent(_event);
		if (_wsa && this->getUseCount() <= 1) {
			_wsa = false;
            WSACleanup();
		}
	}

	TCPSocket& TCPSocket::operator=(const TCPSocket& right) {
		_socket = right._socket;
		_host = right.getHost();
		_port = right.getPort();

		if ((_event = WSACreateEvent()) == WSA_INVALID_EVENT ||
			WSAEventSelect(_socket, _event, FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR)
		{
			closesocket(_socket);
			throw ExceptionSocket(WSAGetLastError());
		}

		return (*this);
	}

	unsigned int TCPSocket::connecting() {
		hostent *hp;
		unsigned long addr;
		struct sockaddr_in server;
		unsigned long ul = 1;

		_state = stConnecting;

		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);

		if (_socket == INVALID_SOCKET) {
			evtOnError(WSAGetLastError());
			return (-1);
		}

		// resolve host
		if (inet_addr(_host.c_str()) == INADDR_NONE)
			hp = gethostbyname(_host.c_str());
		else {
			addr = inet_addr(_host.c_str());
			hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
		}

		if (hp == NULL) {
			closesocket(_socket);
			evtOnError(WSAGetLastError());
			return (-1);
		}
		else {
			server.sin_addr.s_addr =*((unsigned long*)hp->h_addr);
			server.sin_family = AF_INET;
			server.sin_port = htons( _port );

			long option = 60*1000;
			// set to send keep-alives.
			setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE,(char*)&option, sizeof(option));
			// set receives time-out in milliseconds
			setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&option, sizeof(option));
			// set send time-out in milliseconds
			setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO,(char*)&option, sizeof(option));

			if (WSAEventSelect(_socket, _event, FD_CONNECT|FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR ||
				!_threads.runEx(boost::bind(&TCPSocket::loop, this), "TCPSocket::loop") ||
				::connect(_socket, (const sockaddr*)&server, sizeof(sockaddr_in)) != SOCKET_ERROR ||
				WSAGetLastError() != WSAEWOULDBLOCK) {
					closesocket(_socket);
					this->evtOnError(WSAGetLastError());
					return (-1);
				}
		}
		return 0;
	}

	bool TCPSocket::close()
	{
		if (_state == stOffline)
			return false;

		if (_socket && _socket != INVALID_SOCKET)
		{
			LINGER lin = {0};
			lin.l_linger = 0;
			lin.l_onoff = 1;
			setsockopt(_socket, SOL_SOCKET, SO_LINGER, (const char*)&lin, sizeof(lin));
			_state = stDisconnecting;
			closesocket(_socket);
			_socket = NULL;
			return true;
		}
		return false;
	}
	
	unsigned TCPSocket::loop() {
		DWORD wr;
		WSANETWORKEVENTS nev = {0};

		while (_state != stOffline && _state != stDisconnecting) {

			// czekamy na sygnal od systemu...
			wr = WaitForSingleObject(_event, -1);
			if(wr != WAIT_OBJECT_0)
				return 0;

			// pobieramy rodzaj zdarzenia
			if (WSAEnumNetworkEvents(_socket, _event, &nev))
			{
				evtOnError(WSAGetLastError());
				return (-1);
			}

			// wykonujemy zaleznie od tego jakie zdarzenie bylo
			if (nev.lNetworkEvents & FD_READ)
				onReceived();
			else if (nev.lNetworkEvents & FD_ACCEPT)
				onAccept();
			else if (nev.lNetworkEvents & FD_CONNECT)
				onConnected();
			else if (nev.lNetworkEvents & FD_WRITE)
				onWrite();
			else if (nev.lNetworkEvents & FD_CLOSE) 
				onClose();

		}
		return 0;
	}

	void TCPSocket::send(const ByteBuffer& data) {
		if (data.getLength()) {
			int sent = 0;
			while ((sent = ::send(_socket, (const char*)data.getBuffer() + sent, data.getBufferSize(), 0)) < data.getBufferSize())
				if (sent == WSAEWOULDBLOCK)
					Sleep(100);
				else if (sent < 0)
					throw ExceptionSocket(WSAGetLastError());
		}
	}

	void TCPSocket::listen(unsigned int port) {
		_socket =  WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);
		
		if (_socket == INVALID_SOCKET)
			throw ExceptionSocket(WSAGetLastError());

		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port for the socket that is being bound.
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = INADDR_ANY;
		service.sin_port = htons(port);

		if (bind(_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
			closesocket(_socket);
			throw ExceptionSocket(WSAGetLastError());
		}

		// if wsa event has been created and
		// thread has started 
		if (WSAEventSelect(_socket, _event, FD_ACCEPT|FD_CLOSE) == SOCKET_ERROR ||
			!_threads.runEx(boost::bind(&TCPSocket::loop, this), "TCPSocket::loop")) {
				closesocket(_socket);
				throw ExceptionSocket(WSAGetLastError());
			}

		//----------------------
		// Listen for incoming connection requests 
		// on the created socket
		if (::listen(_socket, 10) == SOCKET_ERROR)
		{
			closesocket(_socket);
			throw ExceptionSocket(WSAGetLastError());
		}
		_state = stListen;
	}

	void TCPSocket::onConnected() {
		_state = stConnected;
		evtOnConnected();
	}

	void TCPSocket::onWrite() {
	}

	void TCPSocket::onReceived() {
		ByteBuffer buffer;
		char buff;

		// pobieramy kolejne bajty
		while (recv(_socket, &buff, 1, 0) != SOCKET_ERROR)
			buffer.append((const unsigned char*)&buff, 1);

		// wysylamy sygnal
		this->evtOnReceived(buffer);
	}

	void TCPSocket::onAccept()
	{
		SOCKET sock;
		sockaddr_in addr;
		int size = sizeof(addr);

		sock = accept(_socket, (sockaddr*)&addr, &size);
		if (sock == INVALID_SOCKET)
			evtOnError(WSAGetLastError());
		//else
			//evtOnAccept(TCPSocket(sock, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)));
	}

	void TCPSocket::onClose()
	{
		if (_event)
			WSACloseEvent(_event);
		
		_state = stOffline;
		evtOnClose();
	}

};