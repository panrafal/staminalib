/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */

#ifndef __STAMINA_TCPSOCKETCLIENT_H__
#define __STAMINA_TCPSOCKETCLIENT_H__

#include "SocketClient.h"
#include "WinSocket.h"

#include <Stamina/ThreadRun.h>

namespace Stamina {
	class TCPSocketClient : public SocketClient, WinSocket {
	public:
		TCPSocketClient() : WinSocket(1,1) {
			_threads = new ThreadRunnerStore();
		}

		virtual bool connect(const StringRef& host, unsigned short port);

		virtual int send(const ByteBuffer& buffer);
		virtual void close();
	protected:
		virtual void onReceived();
		virtual void onConnected();
		virtual void onWrite();
		virtual void onClose();
	private:
		unsigned int connecting();
		unsigned int loop();
	private:
		oThreadRunnerStore _threads;
		CriticalSection_w32 _critical;
		HANDLE _event;
	};
}

#endif	// __STAMINA_TCPSOCKETCLIENT_H__