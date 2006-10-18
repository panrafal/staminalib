/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#ifndef __STAMINA_TCPSERVER_W32_H__
#define __STAMINA_TCPSERVER_W32_H__

#include "SocketServer.h"
#include "WinSocket.h"
#include "SocketException.h"

#include <Stamina/ThreadRun.h>

namespace Stamina {
	class TCPServer_W32 : public SocketServer, WinSocket {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::TCPServer_W32, SocketServer, Version(0,1,0,0));

		TCPServer_W32() 
			: WinSocket(1, 1), _maxConnection(1) {
			_threads = new ThreadRunnerStore();
		}
		/** TCPServer_W32 Constructor
		 * @param maxConnection Maximum length of the queue of pending connections. 
		 *			The same as @a backlog in winsock listen function.
		 */
		TCPServer_W32(int maxConnection) 
			: WinSocket(1, 1), _maxConnection(maxConnection) {
			_threads = new ThreadRunnerStore();
		}

		void listen(Port port);
		void close();
	protected:
		const int _maxConnection;
		oThreadRunnerStore _threads;
	private:
		u_int loop();
		SocketClient* createSocketObject(SOCKET socket, sockaddr_in service);
	};
}

#endif	// __STAMINA_TCPSERVER_W32_H__