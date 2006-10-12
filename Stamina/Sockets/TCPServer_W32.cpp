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
#include "TCPClient_W32.h"
#include "TCPServer_W32.h"

namespace Stamina {
	void TCPServer_W32::listen(Port port) {

		_port = port;

		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket == INVALID_SOCKET) {
			throw WSASocketException(WSAGetLastError());
		}

		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port for the socket that is being bound.
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = inet_addr("127.0.0.1");
		service.sin_port = htons(port);

		if (bind(_socket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
				closesocket(_socket);
				throw WSASocketException(WSAGetLastError());
		}

		//----------------------
		// Listen for incoming connection requests 
		// on the created socket
		if (::listen(_socket, _maxConnection) == SOCKET_ERROR)
			throw WSASocketException(WSAGetLastError());
		
		_state = stListen;

		_threads->run(boost::bind(&TCPServer_W32::loop, this), "TCPServer_W32::loop");
	}

	u_int TCPServer_W32::loop() {
		fd_set readfd;
		int ret;
		timeval time;

		time.tv_sec = 1;
		time.tv_usec = 0;
		
		while (_state == stListen) {
			FD_ZERO(&readfd); FD_SET(_socket, &readfd);
			
			ret = select(_socket, &readfd, NULL, NULL, &time);
			if (ret == SOCKET_ERROR) {
				evtOnError(WSAGetLastError());
			} else if (ret > 0) {
				if (FD_ISSET(_socket, &readfd)) {
					sockaddr_in service;
					int addrsize = sizeof(service);
					SOCKET sock = accept(_socket, (SOCKADDR*)&service, &addrsize);
					if (sock == INVALID_SOCKET) {
						evtOnError(WSAGetLastError());
					} else {
						evtOnAccept(createSocketObject(sock, service));
					}
				}
			}
			Sleep(100);
		}

		closesocket(_socket);
		_socket = NULL;
		return NULL;
	}

	SocketClient* TCPServer_W32::createSocketObject(SOCKET socket, sockaddr_in service) {
		
		switch (_proxyType) {
			case ptDirect:
				return new TCPClient_W32(socket, service);
			case ptSOCKS5:
				return NULL;
		}
		return NULL;
	}

	void TCPServer_W32::close() {
		// reszta zrobi siê sama
		_state = stOffline;
	}
}