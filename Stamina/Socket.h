/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005 Krzysztof G³ogocki
 *
 *  $Id: $
 */
#ifndef __STAMINA_SOCKET__
#define __STAMINA_SOCKET__

#pragma once
#include <winsock2.h>
#pragma comment(lib, "Ws2_32")
#include <boost/bind.hpp>
#include <boost/signal.hpp>

#include "iObject.h"
#include "ThreadRun.h"
#include "CrtiticalSection.h"
#include "Exception.h"

namespace Stamin {

	class iSocket: public iObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::iSocket, iObject, Version(0,1,0,0));
		
		virtual bool Send(const void* data, int size) = 0;
	};

	class Socket: public iSocket {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::Socket, iSocket, Version(0,1,0,0));

		Socket(int major = 2, int minor = 2);

		virtual ~Socket() {
			WSACleanup();
		}
		
		enum State {
			stOffline,
			stConnecting,
			stConnected,
			stDisconnecting
		};

		enum Error {
			erNone,
			erInvalidThread		// Can't create thread
		};
	public:
		bool Connect(const StringRef& host, int port);

		String getHost() const;
		int getPort() const;
		State getState() const;
	public:
		boost::signal1<void, int> onError;
		boost::signal0<void> onRead;
		boost::signal0<void> onWrite;
		boost::signal0<void> onAccept;
		boost::signal0<void> onClose;
		boost::signal0<void> onConnected;
	protected:
		virtual void createSocket() = 0;
		virtual void setSocketOption() = 0;
	protected:
		HANDLE _event;
		ThreadRunnerStore* _threads;
		State _state;
		CriticalSection_w32 _critical;
		SOCKET _socket; 
	private:
		unsigned connecting();
		unsigned loop();
	private:
		String _host;
		int _port;
	};
};

#endif	// __STAMINA_SOCKET__