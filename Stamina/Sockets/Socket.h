#ifndef __STAMINA_SOCKET_H__
#define __STAMINA_SOCKET_H__

#include <Stamina/Object.h>
#include <Stamina/String.h>
#include <Stamina/ThreadRun.h>

#include "iSocket.h"
#include "Exception.h"

#include <boost/signal.hpp>
#include <boost/bind.hpp>

namespace Stamina {
	typedef SharedPtr<class Socket> oSocket;

	class Socket:
		public SharedObject<iSocket> {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::Socket, iSocket, Version(0,1,0,0));
		/** Connection state */
		enum State {
			stOffline,
			stConnecting,
			stConnected,
			stListen,
			stDisconnecting
		};
	public:
		Socket() : _threads(new ThreadRunnerStore) {}
		/** Establishes asynchronously connection to another socket application.
		* @param host Hostname.
		* @param port Port number.
		*
		* @remark Creates new thread which establish connection with server.
		*/
		bool connect(const StringRef& host, unsigned port);

		/** Disconnects and closes the socket.
		*/
		virtual bool close() = 0;

		/** Starts listening for an incoming connection.
		*/
		virtual void listen(unsigned int port) = 0;

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
		* @param Socket Socket of incoming connection.
		* @param sockaddr_in Filled sockaddr_in with infos about incoming connection.
		* @param int Size of sockaddr_in struct.
		*/
		boost::signal<void (const oSocket&)> evtOnAccept;
		/** Signal fires when data has been received.
		*/
		boost::signal<void (const ByteBuffer&)> evtOnReceived;
		/** Signal fires when connection has been established.
		*/
		boost::signal<void ()> evtOnConnected;
		/** Signal fires when connection has been closed.
		*/
		boost::signal<void ()> evtOnClose;

	protected:
		/** Connecting thread.
		*/
		virtual unsigned int connecting() = 0;

		/** Main loop.
		*/
		virtual unsigned int loop() = 0;

	protected:
		oThreadRunnerStore _threads;
		State _state;
		CriticalSection_w32 _critical;
		String _host;
		unsigned int _port;
	};
}

#endif	// __STAMINA_SOCKET_H__