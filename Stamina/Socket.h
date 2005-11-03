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

	/** Interface for socket*/
	class iSocket: public iObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::iSocket, iObject, Version(0,1,0,0));
		
		virtual bool Send(const void* data, int size) = 0;
	};

	/** Main socket object */
	class Socket: public iSocket {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::Socket, iSocket, Version(0,1,0,0));

		/**
		@param major - winsock's major ver
		@param minor - winsock's minor ver
		*/
		Socket(int major = 2, int minor = 2);

		virtual ~Socket() {
			WSACleanup();
		}
		
		/** Connection state */
		enum State {
			stOffline,
			stConnecting,
			stConnected,
			stDisconnecting
		};

		enum Error {
			erNone,
			erInvalidThread		/// Can't create thread
		};
	public:
		/** Tries to establish connection
		@param host - host's address
		@param port - host's port
		*/
		bool Connect(const StringRef& host, int port);

		String getHost() const;
		int getPort() const;
		State getState() const;
	protected:
		/**
		*/
		virtual void createSocket() = 0;
		virtual void setSocketOption() = 0;

		virtual void onError(int error) = 0;
		virtual void onRead() = 0;
		virtual void onWrite() = 0;
		virtual void onAccept() = 0;
		virtual void onClose() = 0;
		virtual void onConnected() = 0;
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