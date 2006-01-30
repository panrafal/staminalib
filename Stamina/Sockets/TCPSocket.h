#ifndef __STAMINA_TCPSOCKET_H__
#define __STAMINA_TCPSOCKET_H__

#include <Stamina/Object.h>
#include <Stamina/String.h>
#include <Stamina/ThreadRun.h>

#include "iSocket.h"
#include "Exception.h"

#include <Winsock2.h>
#pragma comment(lib, "Ws2_32")

#include <boost/signal.hpp>
#include <boost/bind.hpp>

namespace Stamina {
	class TCPSocket:
		public SharedObject<iSocket> {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::TCPSocket, iSocket, Version(0,1,0,0));
		/** Connection state */
		enum State {
			stOffline,
			stConnecting,
			stConnected,
			stListen,
			stDisconnecting
		};

		enum Error {
			erNone,
			erInvalidThread		/// Can't create thread
		};
	public:
		/**
		* @param major - winsock's major ver
		* @param minor - winsock's minor ver
		*/
		TCPSocket(int major = 2, int minor = 2) throw(ExceptionSocket);
		~TCPSocket();

		/** Establishes asynchronously connection to another socket application.
		* @param host Hostname.
		* @param port Port number.
		*/
		bool connect(const StringRef& host, unsigned port) throw(ExceptionSocket);

		/** Places a socket in a state in which it is listening for an incoming connection.
		*/
		virtual bool listen(unsigned int port);

		/** Sends data through socket.
		*/
		virtual void send(char* data, unsigned size);

		/** Closes a connection.
		*/
		bool close();

		/** Gets hostname where connection is established to.
		*/
		inline String getHost() const {
			LockerCS(_critical);
			return _host;
		}
		/** Gets port number which connection is established on.
		*/
		inline int getPort() const {
			LockerCS(_critical);
			return _port;
		}

		/** Gets connection state.
		*/
		inline State getState() const {
			LockerCS lock(_critical);
			return _state;
		}
	public:
		/** Signal fires when error occures.
		*/
		boost::signal<void (unsigned)> evtOnError;
		/** Signal fires when connection is incoming.
		*/
		boost::signal<void ()> evtOnAccept;
		/** Signal fires when data has been received.
		*/
		boost::signal<void (char*, unsigned)> evtOnReceived;
		/** Signal fires when connection has been established.
		*/
		boost::signal<void ()> evtOnConnected;
		/** Signal fires when connection has been closed.
		*/
		boost::signal<void ()> evtOnClosed;
	protected:
		State _state;
		CriticalSection_w32 _critical;
	private:
		SOCKET _socket;
		HANDLE _event;	/// WSAEvent
		ThreadRunnerStore* _threads;
	private:
		/** Main loop for sockets.
		*/
		unsigned int loop();
		unsigned int connecting();
	};
};

#endif	// __STAMINA_TCPSOCKET_H_