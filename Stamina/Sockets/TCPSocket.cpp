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

		if (!_threads->runEx(boost::bind(&TCPClient::connecting, this), "TCPSocket::connecting")) {
			throw ExceptionSocket("Cannot create connecting thread");
		}
		return true;
	}

	unsigned int TCPClient::connecting() {
		hostent *hp;
		unsigned long addr;
		struct sockaddr_in server;
		unsigned long ul = 1;

		_state = stConnecting;

		_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);

		if (_socket == INVALID_SOCKET)
			throw ExceptionSocket(WSAGetLastError());

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
				// set to send keep-alives.
				setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE,(char*)&option, sizeof(option));
				// set receives time-out in milliseconds
				setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&option, sizeof(option));
				// set send time-out in milliseconds
				setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO,(char*)&option, sizeof(option));

				//
				if ((_event = WSACreateEvent()) == (HANDLE)-1 || 
					WSAEventSelect(_socket, _event, FD_ACCEPT|FD_CONNECT|FD_WRITE|FD_READ|FD_CLOSE) == SOCKET_ERROR ||
					!_threads->runEx(boost::bind(&TCPClient::loop, this), "TCPClient::loop") ||
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

		while (this->_state != iSocket::stOffline &&
			this->_state != this->stDisconnecting) {

			wr = WaitForSingleObject(_event, -1);
			if(wr != WAIT_OBJECT_0)
				return 0;

			WSAEnumNetworkEvents(_socket, _event, &nev);
			if (nev.lNetworkEvents & FD_READ)
				onReceived();
			else if (nev.lNetworkEvents & FD_ACCEPT)
				onAccept();
			else if (nev.lNetworkEvents & FD_CONNECT) {
				this->_state = stConnected;
				onConnected();
			}
			else if (nev.lNetworkEvents & FD_WRITE)	// pozniej moze sie przydac
			{
				onConnected();
			}
			else if (nev.lNetworkEvents & FD_CLOSE) {
				this->_state = this->stOffline;
				onClose();
			}
		}
		return 0;
	}

	void TCPSocket::send(char* data, unsigned int size) {
		if (data && size) {
			int sent;
			while ((sent = ::send(_socket, data, size, 0)) < size)
				if (sent == WSAEWOULDBLOCK)
					Sleep(100);
				else if (sent < 0)
				{
					onError(WSAGetLastError());
					return;
				}
		}
	}

	void TCPSocket::onReceived() {
		vector<char> theVector;
		char buff;
		char* data = 0;

		// pobieramy kolejne bajty i wrzucamy do tablicy
		while (recv(_socket, &buff, 1, 0) != SOCKET_ERROR)
			theVector.push_back(buff);

		data = new char[theVector.size()];
		/// kopiujemy dane
		for (size_t i = 0; i < theVector.size(); i++)
			data[i] = theVector[i];

		// wysylamy sygnal
		this->evtOnReceived(data, theVector.size());
		delete[] data;
	}
};