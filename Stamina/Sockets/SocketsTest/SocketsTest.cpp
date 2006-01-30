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
	TCPSocket sock;
	sock.evtOnConnected.connect(boost::bind(onConnected));
	sock.evtOnError.connect(boost::bind(onError, _1));
	sock.evtOnReceived.connect(boost::bind(onReceived, _1));
	sock.connect("www.kplugins.net", 80);
	char b[] = "GET /index.xml HTTP/1.1\r\nHost: www.kplugins.net\r\n\r\n";
	ByteBuffer data;
	data.assign((const unsigned char*)b, strlen(b));
	while(1)
	{
		if (connected)
		{
			sock.send(data);
			connected = false;
		}
		Sleep(1);
	}
	return 0;
}

