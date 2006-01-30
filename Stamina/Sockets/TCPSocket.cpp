#include "stdafx.h"
#include "TCPSocket.h"
#include <vector>
using namespace std;

namespace Stamina {
	TCPSocket::TCPSocket(int major, int minor)
		: _threads(new ThreadRunnerStore), _event(NULL) {
		WSAData wsaData;
		WORD wVersionRequested;

		wVersionRequested = MAKEWORD(major, minor);
		if (WSAStartup(wVersionRequested, &wsaData))
			throw ExceptionSocket(WSAGetLastError());

		_state = stOffline;
	}

	TCPSocket::~TCPSocket() {
		if (this->_state != this->stOffline)
			close();
		if (_event)
			WSACloseEvent(_event);
		WSACleanup();
	}

	bool TCPSocket::connect(const StringRef& host, unsigned port) {
		if (_state != stOffline)
			return false;
	
		_host = host;
		_port = port;
		_state = stConnecting;

		if (!_threads->runEx(boost::bind(&TCPSocket::connecting, this), "TCPSocket::connecting")) {
			throw ExceptionSocket("Cannot create connecting thread.");
		}
		return true;
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

			// if wsa event has been created and select,
			// thread has started and socket has connected
			if ((_event = WSACreateEvent()) == (HANDLE)-1 || 
				WSAEventSelect(_socket, _event, FD_ACCEPT|FD_CONNECT|FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR ||
				!_threads->runEx(boost::bind(&TCPSocket::loop, this), "TCPSocket::loop") ||
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
		if (this->_state == this->stOffline)
			return false;

		if (this->_socket && this->_socket != INVALID_SOCKET)
		{
			LINGER lin = {0};
			lin.l_linger = 0;
			lin.l_onoff = 1;
			setsockopt(this->_socket, SOL_SOCKET, SO_LINGER, (const char*)&lin, 4);
			closesocket(this->_socket);
			this->_socket = NULL;
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
			if (!WSAEnumNetworkEvents(_socket, _event, &nev))
			{
				evtOnError(WSAGetLastError());
				return (-1);
			}

			// wykonujemy zaleznie od tego jakie zdarzenie bylo
			if (nev.lNetworkEvents & FD_READ)
				onReceived();
			else if (nev.lNetworkEvents & FD_ACCEPT)
				onAccept();
			else if (nev.lNetworkEvents & FD_CONNECT) {
				this->_state = stConnected;
				evtOnConnected();
			}
			else if (nev.lNetworkEvents & FD_WRITE);	// pozniej moze sie przydac
			else if (nev.lNetworkEvents & FD_CLOSE) {
				this->_state = this->stOffline;
				evtOnClosed();
			}
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

	void TCPSocket::onReceived() {
		ByteBuffer buffer;
		char buff;

		// pobieramy kolejne bajty
		while (recv(_socket, &buff, 1, 0) != SOCKET_ERROR)
			buffer.append((const unsigned char*)&buff, 1);

		// wysylamy sygnal
		this->evtOnReceived(buffer);
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

		//----------------------
		// Listen for incoming connection requests 
		// on the created socket
		if (::listen(_socket, 1) == SOCKET_ERROR)
		{
			closesocket(_socket);
			throw ExceptionSocket(WSAGetLastError());
		}
	}

	void TCPSocket::onAccept()
	{
		SOCKET sock;
		sockaddr_in addr;
		int size;

		sock = accept(_socket, (sockaddr*)&addr, &size);
		if (sock == INVALID_SOCKET)
			evtOnError(WSAGetLastError());
		else
			evtOnAccept(sock, addr, size);
	}

};