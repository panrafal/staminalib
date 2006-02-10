// SocketsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\TCPSocket.h"
using namespace Stamina;

#pragma comment(lib, "../debug/sockets.lib")
bool connected = false;
void onConnected()
{
	connected = true;
	std::cout << "connected!" << std::endl;
}

void onError(int err)
{
	std::cout << "Error = " << err << std::endl;
}

void onReceived1(const ByteBuffer& buff)
{
	std::cout << "server " << (char*)buff.getString() << std::endl;
}
void onReceived2(const ByteBuffer& buff)
{
	std::cout << (char*)buff.getString() << std::endl;
}

void onAccept(const Socket& sock)
{
	std::cout << sock.getHost() << ":" << sock.getPort() << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		TCPSocket sock;
		TCPSocket server;
		server.evtOnError.connect(boost::bind(onError, _1));
		server.evtOnReceived.connect(boost::bind(onReceived1, _1));
		server.evtOnAccept.connect(boost::bind(onAccept, _1));
		server.listen(6588);
		sock.evtOnConnected.connect(boost::bind(onConnected));
		sock.evtOnError.connect(boost::bind(onError, _1));
		sock.evtOnReceived.connect(boost::bind(onReceived2, _1));
		sock.connect("localhost", 6588);
		char b[] = "GET /index.xml HTTP/1.1\r\nHost: www.kplugins.net\r\n\r\n";
		ByteBuffer data;
		data.assign((const unsigned char*)b, strlen(b));
		while(1)
		{
			if (sock.getState() == TCPSocket::stConnected && connected)
			{
				sock.send(data);
				connected = false;
			}
			Sleep(1);
		}
		//SOCKET _socket =  WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);
		//
		//if (_socket == INVALID_SOCKET)
		//	throw ExceptionSocket(WSAGetLastError());

		////----------------------
		//// The sockaddr_in structure specifies the address family,
		//// IP address, and port for the socket that is being bound.
		//sockaddr_in service;
		//service.sin_family = AF_INET;
		//service.sin_addr.s_addr = INADDR_ANY;
		//service.sin_port = htons(6588);

		//if (bind(_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		//	closesocket(_socket);
		//	throw ExceptionSocket(WSAGetLastError());
		//}

		//HANDLE _event = WSACreateEvent();
		//// if wsa event has been created and
		//// thread has started 
		//if (WSAEventSelect(_socket, _event, FD_ACCEPT|FD_CLOSE) == SOCKET_ERROR /*||
		//	!_threads.runEx(boost::bind(&TCPSocket::loop, this), "TCPSocket::loop")*/) {
		//		closesocket(_socket);
		//		throw ExceptionSocket(WSAGetLastError());
		//	}

		////----------------------
		//// Listen for incoming connection requests 
		//// on the created socket
		//listen(_socket, 10);
	}
	catch (const ExceptionSocket& e)
	{
		std::cout << e.getReason() << std::endl;
	}
	catch (...)
	{
		std::cout << "byu" << std::endl;
	}
	return 0;
}

