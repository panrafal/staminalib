// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Stamina/Sockets/TCPSocketClient.h>
#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
using namespace Stamina;
using namespace std;

class Test
{
public:
	void start()
	{
		socket.evtOnConnected.connect(boost::bind(&Test::onConnected, this));
		socket.evtOnReceived.connect(boost::bind(&Test::onReceived, this, _1));
		socket.connect("www.onet.pl", 80);
		
	}
private:
	void onConnected() 
	{
		std::cout << "Connected!!!!" << std::endl;

		char b[] = "GET / HTTP/1.1\r\nHost: www.konnekt.info\r\n\r\n";
		ByteBuffer data;
		data.assign((const unsigned char*)b, strlen(b));

		socket.send(data);
	}

	void onReceived(const ByteBuffer& data) 
	{
		std::cout << (char*)data.getString() << std::endl;
	}
private:
	TCPSocketClient socket;
};

int _tmain(int argc, _TCHAR* argv[])
{
	Test t;
	t.start();
	char c;
	std::cin >> c;
	return 0;
}

