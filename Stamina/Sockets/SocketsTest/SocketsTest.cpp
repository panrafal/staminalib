// SocketsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\SocketClient.h"
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
		oSocketClient client = new SocketClient(new TCPSocket);

		client->evtOnConnected.connect(boost::bind(onConnected));
		client->evtOnError.connect(boost::bind(onError, _1));
		client->evtOnReceived.connect(boost::bind(onReceived, _1));
		client->connect("localhost", 6588, INFINITE);

		char b[] = "GET / HTTP/1.1\r\nHost: www.kplugins.net\r\n\r\n";
		ByteBuffer data;
		data.assign((const unsigned char*)b, strlen(b));
		
		client << data;
		//client >> data;
		//std::cout << data.getString();
		char c;
		std::cin >> c;
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

