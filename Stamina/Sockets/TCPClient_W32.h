/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#ifndef __STAMINA_TCPSOCKET_W32_H__
#define __STAMINA_TCPSOCKET_W32_H__

#include "SocketClient.h"
#include "WinSocket.h"

#include <Stamina/ThreadRun.h>

namespace Stamina {
	class TCPClient_W32 : public SocketClient, WinSocket {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::TCPClient_W32, SocketClient, Version(0,1,0,0));

		TCPClient_W32() : WinSocket(1,1) {
			_threads = new ThreadRunnerStore();
		}
		TCPClient_W32(SOCKET socket, sockaddr_in service);

		bool connect(const StringRef& host, Port port);
		void close();

		int write(const char* data, Size size);
		int read(char* data, Size size);
	protected:
		oThreadRunnerStore _threads;
	private:
		unsigned int connecting();
		unsigned int loop();
	};
}

#endif	// __STAMINA_TCPSOCKET_W32_H__