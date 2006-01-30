#include "stdafx.h"
#include "TCPClient.h"

namespace Stamina {	
	TCPClient::~TCPClient() {
		if (_connEvent)
			CloseHandle(_connEvent);
	}
	

	bool TCPClient::connectSync(const StringRef& host, unsigned int port) {
		if (_state != this->stOffline)
			return false;

		_host = host;
		_port = port;
		_state = this->stConnecting;

		if (connecting())
			return false;
		return true;
	}

	unsigned TCPClient::connecting() {
		hostent *hp;
		unsigned long addr;
		struct sockaddr_in server;
		unsigned long ul = 1;

		_state = State::stConnecting;

		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);

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
				this->evtOnError(WSAGetLastError());
				return (-1);
			}
			else {
				server.sin_addr.s_addr =*((unsigned long*)hp->h_addr);
				server.sin_family = AF_INET;
				server.sin_port = htons( _port );

				long option = 60*1000;
				setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE,(const char*)&option,sizeof(option));
				setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&option, sizeof(option));
				setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO,(char*)&option, sizeof(option));

				if ((_event = WSACreateEvent()) == (HANDLE)-1 || !(_connEvent = CreateEvent(NULL, TRUE, FALSE, "Connection")) ||
					WSAEventSelect(_socket, _event, FD_ACCEPT|FD_CONNECT|FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR ||
					!_threads->runEx(boost::bind(&TCPClient::loop, this), "TCPClient::loop") ||
					::connect(_socket, (const sockaddr*)&server, sizeof(sockaddr_in)) != SOCKET_ERROR ||
					WSAGetLastError() != WSAEWOULDBLOCK) {
						closesocket(_socket);
						this->evtOnError(WSAGetLastError());
						return (-1);
					}
				else {
					ResetEvent(_connEvent);
					WaitForSingleObject(_connEvent, INFINITE);
				}
			}
		}
		else
		{
			this->evtOnError(WSAGetLastError());
			return (-1);
		}
		return 0;
	}

	void TCPClient::onAccept()
	{
	}

	void TCPClient::onConnected()
	{
		this->evtOnConnected();
		SetEvent(_connEvent);
	}

	void TCPClient::onClose()
	{
		this->evtOnClosed();
	}

	void TCPClient::onError(int err)
	{
		this->evtOnError(err);
	}
}