// SocketsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\TCPClient.h"
using namespace Stamina;

#pragma comment(lib, "../debug/sockets.lib")

void onConnected()
{
	std::cout << "connected!" << std::endl;
}
void onError(int err)
{
	std::cout << "Error = " << err << std::endl;
}

void onReceived(char* txt, int len)
{
	std::cout << txt << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	TCPClient sock;
	sock.evtOnConnected.connect(boost::bind(onConnected));
	sock.evtOnError.connect(boost::bind(onError, _1));
	sock.evtOnReceived.connect(boost::bind(onReceived, _1, _2));
	sock.connectSync("www.kplugins.net", 80);
	char b[] = "GET /index.xml HTTP/1.1\r\nHost: www.kplugins.net\r\n\r\n";
	sock.send(b, strlen(b)+1);
	while(1)
		Sleep(1);
	return 0;
}

