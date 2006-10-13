// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../TCPClient_W32.h"
#include "../TCPServer_W32.h"
#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
using namespace Stamina;
using namespace std;

class TestClient
{
public:
	TestClient()
	{
		socket = new TCPClient_W32();
	}
	~TestClient()
	{
		socket->close();
		delete socket;
	}

	void start()
	{
		socket->evtOnConnected.connect(boost::bind(&TestClient::onConnected, this));
		socket->evtOnReceived.connect(boost::bind(&TestClient::onReceived, this));
		socket->evtOnError.connect(boost::bind(&TestClient::onError, this, _1));
		socket->connect("www.konnekt.info", 80);
		
	}
private:
	void onError(unsigned int error )
	{
		DWORD dw;
		dw = WSAGetLastError();
		std::cout << error << std::endl;
	}

	void onConnected() 
	{
		std::cout << "Connected!!!!" << std::endl;

		char b[] = "GET / HTTP/1.1\r\nHost: konnekt.info\r\n\r\n";
		socket->write(b, strlen(b));
	}

	void onReceived() 
	{
		char buff[2048];
		int rcv = socket->read(buff, 2048);
		buff[rcv] = '\0'; 
		std::cout << buff << std::endl;
	}
private:
	SocketClient* socket;
};


class TestServer {
public:
	TestServer() {
		socket = new TCPServer_W32();
	}

	void start() {
		socket->evtOnAccept.connect(boost::bind(&TestServer::onAccept, this, _1));
		socket->evtOnError.connect(boost::bind(&TestServer::onError, this, _1));
		socket->listen(4567);
	}
private:
	void onAccept(SocketClient* socket) {
		socket->write("zonk zonk zonk", 14);
	}
	void onError(unsigned int error) {
		std::cout << error << std::endl;
	}
private:
	SocketServer* socket;
};

int _tmain(int argc, _TCHAR* argv[])
{
	TestClient t;
	TestServer s;
	s.start();
	Sleep(3000);
	t.start();
	char c;
	std::cin >> c;
	return 0;
}

