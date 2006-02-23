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
#include "TCPSocket.h"
#include <vector>
using namespace std;

namespace Stamina {
	bool TCPSocket::_wsa = false;

	TCPSocket::TCPSocket(int major, int minor)
		: _endEvent(NULL) {
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

		_threads->run(boost::bind(&TCPSocket::loop, this), "TCPSocket::loop");
	}

	TCPSocket::~TCPSocket() {
		_state = stOffline;
		SetEvent(_endEvent);
		_threads->waitForThreads(1000, 5000, true);
		WSACloseEvent(_event);
		if (_wsa && this->getUseCount() <= 1) {
			_wsa = false;
            WSACleanup();
		}
	}

	unsigned int TCPSocket::connecting() {
		hostent *hp;
		unsigned long addr;
		struct sockaddr_in server;
		unsigned long ul = 1;

		_state = stConnecting;

		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);

		if (_socket == INVALID_SOCKET) {
			_evtOnError(WSAGetLastError());
			ExitThread(-1);
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
			_evtOnError(WSAGetLastError());
			ExitThread(-1);
		}
		else {
			server.sin_addr.s_addr =*((unsigned long*)hp->h_addr);
			server.sin_family = AF_INET;
			server.sin_port = htons( _port );

			//long option = 60*1000;
			//// set to send keep-alives.
			//setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE,(char*)&option, sizeof(option));
			//// set receives time-out in milliseconds
			//setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&option, sizeof(option));
			//// set send time-out in milliseconds
			//setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO,(char*)&option, sizeof(option));

			_endEvent = CreateEvent(NULL, FALSE, FALSE, "TCPSocket::EndLoop");
			_threads->run(boost::bind(&TCPSocket::loop, this), "TCPSocket::loop");

			if (WSAEventSelect(_socket, _event, FD_CONNECT|FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR ||
				::connect(_socket, (const sockaddr*)&server, sizeof(sockaddr_in)) != SOCKET_ERROR ||
				WSAGetLastError() != WSAEWOULDBLOCK) {
					closesocket(_socket);
					_evtOnError(WSAGetLastError());
					ExitThread(-1);
				}
			//SleepEx(1, FALSE);
		}
		ExitThread(NO_ERROR);
	}

	bool TCPSocket::close()
	{
		LockerCS locker(_critical);

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
		HANDLE events[] = {_endEvent, _event};

		while (_state != stOffline && _state != stDisconnecting) {

			// czekamy na sygnal od systemu...
			wr = WaitForMultipleObjects(2, events, FALSE, INFINITE);
			if ((wr - WAIT_OBJECT_0) == 0)	// endEvent
				break;

			// pobieramy rodzaj zdarzenia
			if (WSAEnumNetworkEvents(_socket, _event, &nev) == SOCKET_ERROR)
			{
				_evtOnError(WSAGetLastError());
				ExitThread(-1);
			}

			// wykonujemy zaleznie od tego jakie zdarzenie bylo
			if (nev.lNetworkEvents & FD_READ)
				if (!nev.iErrorCode[FD_READ_BIT])
                    onReceived();
				else
					_evtOnError(nev.iErrorCode[FD_READ_BIT]);
			else if (nev.lNetworkEvents & FD_ACCEPT)
				if (!nev.iErrorCode[FD_ACCEPT_BIT])
                    onAccept();
				else
					_evtOnError(nev.iErrorCode[FD_ACCEPT_BIT]);
			else if (nev.lNetworkEvents & FD_CONNECT)
				if (!nev.iErrorCode[FD_CONNECT_BIT])
                    onConnected();
				else
					_evtOnError(nev.iErrorCode[FD_CONNECT_BIT]);
			else if (nev.lNetworkEvents & FD_WRITE)
				if (!nev.iErrorCode[FD_WRITE_BIT])
                    onWrite();
				else
					_evtOnError(nev.iErrorCode[FD_WRITE_BIT]);
			else if (nev.lNetworkEvents & FD_CLOSE)
				if (!nev.iErrorCode[FD_CLOSE_BIT]) 
                    onClose();
				else
					_evtOnError(nev.iErrorCode[FD_CLOSE_BIT]);

		}
		ExitThread(NO_ERROR);
	}

	void TCPSocket::send(const ByteBuffer& data) {
		LockerCS locker(_critical);
		if (data.getLength()) {
			int sent = 0;
			/** @todo B³ad przy wysylaniu, send zwraca SOCKET_ERROR
			* pozatym przy socket_error tracimy dane o wyslanych bajtach.
			* Moze warto przejsc na WSASend?...
			*/
			while ((sent = ::send(_socket, (const char*)data.getBuffer() + sent, data.getLength(), 0)) < data.getBufferSize())
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

		if (WSAEventSelect(_socket, _event, FD_ACCEPT|FD_CLOSE) == SOCKET_ERROR) {
			closesocket(_socket);
			throw ExceptionSocket(WSAGetLastError());
		}

		_state = stListen;
		_endEvent = CreateEvent(NULL, FALSE, FALSE, "TCPSocket::EndLoop");
		_threads->run(boost::bind(&TCPSocket::loop, this), "TCPSocket::loop");

		//----------------------
		// Listen for incoming connection requests 
		// on the created socket
		if (::listen(_socket, 10) == SOCKET_ERROR)
		{
			closesocket(_socket);
			throw ExceptionSocket(WSAGetLastError());
		}
	}

	void TCPSocket::onConnected() {
		_state = stConnected;
		_evtOnConnected();
	}

	void TCPSocket::onWrite() {
	}

	void TCPSocket::onReceived() {
		ByteBuffer buffer;
		char buff[1024];
		WSABUF wsaBuff;
		DWORD recvd, flags = 0;

		wsaBuff.buf = buff;
		wsaBuff.len = 1024;

		while (WSARecv(_socket, &wsaBuff, 1, &recvd, &flags, NULL, NULL) != SOCKET_ERROR &&
			recvd != 0) {
			buffer.append((const unsigned char*)&buff, recvd);
		}

		// jeœli jeszcze nie ma ¿adnego podlaczonego slota
		// to lepiej nie marnowac danych ;)
		if (this->_evtOnReceived.num_slots())
            this->_evtOnReceived(buffer);
		else
			_buffer.append(buffer.getBuffer(), buffer.getLength());
	}

	void TCPSocket::onAccept()
	{
		SOCKET sock;
		sockaddr_in addr;
		int size = sizeof(addr);

		sock = accept(_socket, (sockaddr*)&addr, &size);
		if (sock == INVALID_SOCKET)
			_evtOnError(WSAGetLastError());
		else
			_evtOnAccept(new TCPSocket(sock, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)));
	}

	void TCPSocket::onClose()
	{
		_state = stOffline;
		_evtOnClose();
	}

};