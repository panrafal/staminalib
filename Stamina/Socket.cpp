/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#include "stdafx.h"
#include "Socket.h"

namespace Stamina {

	Socket::Socket(int major, int minor) 
		: _threads(new ThreadRunner) {
		WSAData data;
		DWORD wVersionRequested;

		wVersionRequested = MAKEWORD(major, minor);
		WSAStartup(wVersionRequested, &wsaData);
	}

	String Socket::getHost() {
		LockerCS lock(_critical);
		return _host;
	}

	int Socket::getPort() {
		LockerCS lock(_critical);
		return _port;
	}

	State Socket::getState() {
		LockerCS lock(_critical);
		return _state;
	}

	bool Socket::Connect(const StringRef& host, int port) {
		if (_state != stOffline)
			return false;
	
		_host = host;
		_port = port;
		_state = stConnecting;

		if ( _threads->runEx(boost::bind(&Socket::connecting, this)) ) {
			onError(erInvalidThread);
			return false
		}
		return true;
	}

	unsigned Socket::connecting() {
		hostent *hp;
		unsigned long addr;
		struct sockaddr_in server;
		unsigned long ul = 1;

		_state = stConnecting;

		//_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);
		createSocket();

		if (_socket != INVALID_SOCKET)
		{
			if (inet_addr(_host.c_str()) == INADDR_NONE)
				hp = gethostbyname(_host.c_str());
			else {
				addr = inet_addr(_host.c_str());
				hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
			}

			if (hp == NULL) {
				closesocket(_socket);
				onError(WSAGetLastError());
			}
			else {
				server.sin_addr.s_addr =*((unsigned long*)hp->h_addr);
				server.sin_family = AF_INET;
				server.sin_port = htons( _port );

				/*setsockopt(_socket,SOL_SOCKET,SO_KEEPALIVE,(const char*)&option,sizeof(option));
				long option = 60*1000;
				setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&option, sizeof(option));
				setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO,(char*)&option, sizeof(option));
				*/
				setSocketOption();
				
				if ((_event = WSACreateEvent()) == (HANDLE)-1 ||
                    WSAEventSelect(_socket, _event, FD_ACCEPT|FD_CONNECT|FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR ||
                    connect(_socket, (const sockaddr_in*)&server, sizeof(sockaddr_in)) != SOCKET_ERROR ||
					WSAGetLastError() != WSAEWOULDBLOCK ||
					_threads->runEx(boost::bind(&Socket::loop, this)) != 0) {
					closesocket(_socket);
					onError(WSAGetLastError());
				}
			}
		}
		else
			onError(WSAGetLastError());
		return 0;
	}
	
	unsigned Socket::loop() {
		DWORD wr;
		WSANETWORKEVENTS nev = {0};
		while(_state != stOffile && _state != stDisconnecting) {
			wr = WaitForSingleObject(_event, -1);
			
			if(wr!=WAIT_OBJECT_0)
				return 0;

			WSAEnumNetworkEvents(_socket, _event, &nev);
			if (nev.lNetworkEvents & FD_READ)
				onRead();
			else if (nev.lNetworkEvents & FD_ACCEPT)
				onAccept();
			else if (nev.lNetworkEvents & FD_CONNECT) {
				_state = stConnected;
				onConnected();
			}
			else if (nev.lNetworkEvents & FD_WRITE)
				onWrite();
			else if (nev.lNetworkEvents & FD_CLOSE) {
				_state = stOffline;
				onClose();
			}
			else
				onError(nev.iErrorCode);
		}
	}
};