#include "stdafx.h"
#include "TCPServer.h"

namespace Stamina {
	bool TCPServer::listen(unsigned int port) {
		this->_socket =  WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);
		if (this->_socket == INVALID_SOCKET) {
			onError(WSAGetLastError());
			return false;
		}

		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port for the socket that is being bound.
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = INADDR_ANY;
		service.sin_port = htons(port);

		if (bind(this->_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
			onError(WSAGetLastError());
			closesocket(this->_socket);
			return false;
		}

		//----------------------
		// Listen for incoming connection requests 
		// on the created socket
		if (::listen(this->_socket, 1) == SOCKET_ERROR)
		{
			onError(WSAGetLastError());
			closesocket(this->_socket);
			return false;
		}
		return true;
	}
};