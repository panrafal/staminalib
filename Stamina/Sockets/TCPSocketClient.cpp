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
#include "TCPSocketClient.h"

namespace Stamina {

	bool TCPSocketClient::connect(const StringRef& host, unsigned short port) {
		if (_state != stDisconnected)
			return false;

		_state = stConnecting;
		_host = host;
		_port = port;
		_threads->run(boost::bind(&TCPSocketClient::connecting, this), "Socket::connecting");
		return true;
	}

	unsigned int TCPSocketClient::connecting() {
		hostent *hp;
		unsigned long addr;
		struct sockaddr_in server;
		unsigned long ul = 1;

		_state = stConnecting;

		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);

		if (_socket == INVALID_SOCKET) {
			evtOnError(WSAGetLastError());
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
			evtOnError(WSAGetLastError());
			ExitThread(-1);
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

			_threads->run(boost::bind(&TCPSocketClient::loop, this), "TCPSocketClient::loop");

			if (WSAEventSelect(_socket, _event, FD_CONNECT|FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR ||
				::connect(_socket, (const sockaddr*)&server, sizeof(sockaddr_in)) != SOCKET_ERROR ||
				WSAGetLastError() != WSAEWOULDBLOCK) {
					closesocket(_socket);
					evtOnError(WSAGetLastError());
					ExitThread(-1);
				}
			//SleepEx(1, FALSE);
		}
		ExitThread(NO_ERROR);
	}

	unsigned int TCPSocketClient::loop() {
		DWORD wr;
		WSANETWORKEVENTS nev = {0};

		while (_state != stDisconnected && _state != stDisconnecting) {

			// czekamy na sygnal od systemu...
			wr = WaitForSingleObject(_event, INFINITE);

			// pobieramy rodzaj zdarzenia
			if (WSAEnumNetworkEvents(_socket, _event, &nev) == SOCKET_ERROR)
			{
				evtOnError(WSAGetLastError());
				ExitThread(-1);
			}

			// wykonujemy zaleznie od tego jakie zdarzenie bylo
			if (nev.lNetworkEvents & FD_READ)
				if (!nev.iErrorCode[FD_READ_BIT])
                    onReceived();
				else
					evtOnError(nev.iErrorCode[FD_READ_BIT]);
			else if (nev.lNetworkEvents & FD_CONNECT)
				if (!nev.iErrorCode[FD_CONNECT_BIT])
                    onConnected();
				else
					evtOnError(nev.iErrorCode[FD_CONNECT_BIT]);
			else if (nev.lNetworkEvents & FD_WRITE)
				if (!nev.iErrorCode[FD_WRITE_BIT])
                    onWrite();
				else
					evtOnError(nev.iErrorCode[FD_WRITE_BIT]);
			else if (nev.lNetworkEvents & FD_CLOSE)
				if (!nev.iErrorCode[FD_CLOSE_BIT]) 
                    onClose();
				else
					evtOnError(nev.iErrorCode[FD_CLOSE_BIT]);

		}
		ExitThread(NO_ERROR);
	}

	void TCPSocketClient::onConnected() {
		_state = stConnected;
		evtOnConnected();
	}

	void TCPSocketClient::onReceived() {
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

		evtOnReceived(buffer);
	}

	void TCPSocketClient::onWrite() {
	}

	void TCPSocketClient::onClose() {
		_state = stDisconnected;
		evtOnClose();
	}

	int TCPSocketClient::send(const ByteBuffer &buffer) {
		LockerCS locker(_critical);
		if (buffer.getLength()) {
			unsigned int sent = 0;
			int size;
			while (sent < buffer.getBufferSize())
			{
				size = ::send(_socket, (const char*)buffer.getBuffer() + sent, buffer.getBufferSize(), 0);
				if (size == WSAEWOULDBLOCK)
					Sleep(100);
				else if (size < 0)
					throw SocketException(WSAGetLastError());
				sent += size;
			}
			return sent;
		}
		return 0;
	}

	void TCPSocketClient::close() {
		LockerCS locker(_critical);

		if (_state == stDisconnected)
			return;

		if (_socket && _socket != INVALID_SOCKET)
		{
			LINGER lin = {0};
			lin.l_linger = 0;
			lin.l_onoff = 1;
			setsockopt(_socket, SOL_SOCKET, SO_LINGER, (const char*)&lin, sizeof(lin));
			_state = stDisconnected;
			closesocket(_socket);
			_socket = NULL;
		}
	}
}