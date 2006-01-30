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
		TCPSocket(int major = 2, int minor = 2);
		~TCPSocket();

		/** Establishes asynchronously connection to another socket application.
		* @param host Hostname.
		* @param port Port number.
		*/
		bool connect(const StringRef& host, unsigned port);

		/** Closes a connection.
		*/
		bool close();

		/** Sends data through socket.
		*/
		virtual void send(const ByteBuffer& data);

		/** Starts listening for an incoming connection.
		*/
		virtual void listen(unsigned int port);

		/** Gets hostname where connection is established to.
		*/
		inline String getHost() const {
			LockerCS locker(_critical);
			return _host;
		}
		/** Gets port number which connection is established on.
		*/
		inline int getPort() const {
			LockerCS locker(_critical);
			return _port;
		}

		/** Gets connection state.
		*/
		inline State getState() const {
			LockerCS locker(_critical);
			return _state;
		}
	public:
		/** Signal fires when error occures.
		*/
		boost::signal<void (unsigned)> evtOnError;
		/** Signal fires when connection is incoming.
		* @param SOCKET Socket of incoming connection.
		* @param sockaddr_in Filled sockaddr_in with infos about incoming connection.
		* @param int Size of sockaddr_in struct.
		*/
		boost::signal<void (SOCKET, const sockaddr_in&, int)> evtOnAccept;
		/** Signal fires when data has been received.
		*/
		boost::signal<void (const ByteBuffer&)> evtOnReceived;
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
		String _host;
		unsigned int _port;
	private:
		/** Main loop for sockets.
		*/
		unsigned int loop();
		unsigned int connecting();
		void onReceived();
		void onAccept();
	};
};

#endif	// __STAMINA_TCPSOCKET_H_