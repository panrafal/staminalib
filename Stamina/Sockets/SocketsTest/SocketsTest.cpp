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

void onReceived(const ByteBuffer& buff)
{
	std::cout << (char*)buff.getString() << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		oTCPSocket sock = new TCPSocket;

		sock->evtOnConnected.connect(boost::bind(onConnected));
		sock->evtOnError.connect(boost::bind(onError, _1));
		sock->evtOnReceived.connect(boost::bind(onReceived, _1));
		sock->connect("localhost", 6588);

		char b[] = "GET /index.xml HTTP/1.1\r\nHost: www.kplugins.net\r\n\r\n";
		ByteBuffer data;
		data.assign((const unsigned char*)b, strlen(b));
		while(1)
		{
			if (sock->getState() == TCPSocket::stConnected && connected)
			{
				Sleep(1000);
				sock->send(data);
				connected = false;
			}
			Sleep(1);
		}
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

