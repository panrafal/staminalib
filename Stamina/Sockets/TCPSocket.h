#ifndef __STAMINA_TCPSOCKET_H__
#define __STAMINA_TCPSOCKET_H__

#include <Stamina/Object.h>
#include <Stamina/String.h>
#include <Stamina/ThreadRun.h>
#include "iSocket.h"

#include <Winsock2.h>
#pragma comment(lib, "Ws2_32")

#include <boost/bind.hpp>

namespace Stamina {
	class TCPSocket: public iSocket {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::TCPSocket, iSocket, Version(0,1,0,0));
		enum Error {
			erNone,
			erInvalidThread		/// Can't create thread
		};
	public:
		/**
		* @param major - winsock's major ver
		* @param minor - winsock's minor ver
		*/
		TCPSocket(int major = 2, int minor = 2);
		~TCPSocket();

		/** Overloaded function from iSocket.
		*/
		virtual void send(char* data, unsigned size);

		/** Closes a connection.
		*/
		bool close();
	protected:
		HANDLE _event;	/// WSAEvent
		ThreadRunnerStore* _threads;
		SOCKET _socket;
	protected:
		/** Socket invokes function when has got something on input.
		*/
		virtual void onReceived();
		/** Socket invokes function when client is establishing the connection.
		*/
		virtual void onAccept() = 0;
		/** Socket invokes function when connection is established.
		*/
		virtual void onConnected() = 0;
		/** Socket invokes function when connection has been closed.
		*/
		virtual void onClose() = 0;
		/** Socket invokes function when an error occurs.
		@param err Error code.
		*/
		virtual void onError(int err) = 0;

		/** Main loop for sockets.
		*/
		unsigned loop();
	};
};

#endif	// __STAMINA_TCPSOCKET_H_