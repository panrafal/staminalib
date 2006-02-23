#ifndef __STAMINA_SOCKET_H__
#define __STAMINA_SOCKET_H__

#include <Stamina/Object.h>
#include <Stamina/String.h>
#include <Stamina/ThreadRun.h>

#include "iSocket.h"
#include "Exception.h"

#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace Stamina {
	typedef SharedPtr<class Socket> oSocket;

	/** 
	*/
	typedef boost::function<void (unsigned)> fUnsigned;
	typedef boost::function<void (const oSocket&)> fSocket;
	typedef boost::function<void (const ByteBuffer&)> fBuffer;
	typedef boost::function<void ()> fVoid;

	typedef boost::signal<void (unsigned)> sOnError;
	typedef boost::signal<void (const oSocket&)> sOnAccept;
	typedef boost::signal<void (const ByteBuffer&)> sOnReceived;
	typedef boost::signal<void ()> sOnConnected;
	typedef boost::signal<void ()> sOnClose;

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

		/** Proxy type*/
		enum Proxy {
			proxyNone,
			proxySOCKS5
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
		inline unsigned int getPort() const {
			LockerCS locker(_critical);
			return _port;
		}

		/** Gets connection state.
		*/
		inline State getState() const {
			LockerCS locker(_critical);
			return _state;
		}

		/** Gets data which has been sent immediately after connection was established.
		*/
		inline const ByteBuffer& getData() const {
			ObjLocker locker(this);
			return _buffer;
		}
	public:

		bool setEvtOnError(fUnsigned f);
		const sOnError& getEvtOnError() const {
			return _evtOnError;
		}

		bool setEvtOnAccept(fSocket f);
		const sOnAccept& getEvtOnAccpet() const {
			return _evtOnAccept;
		}

		bool setEvtOnReceived(fBuffer f);
		const sOnReceived& getEvtOnReceived() const {
			return _evtOnReceived;
		}

		bool setEvtOnConnected(fVoid f);
		const sOnConnected& getEvtOnConnected() const {
			return _evtOnConnected;
		}

		bool setEvtOnClose(fVoid f);
		const sOnClose& getEvtOnClose() const {
			return _evtOnClose;
		}

	
	protected:
		/** Signal fires when error occures.
		*/
		sOnError _evtOnError;
		/** Signal fires when connection is incoming.
		* @param oSocket Socket of incoming connection.
		*/
		sOnAccept _evtOnAccept;
		/** Signal fires when data has been received.
		*/
		sOnReceived _evtOnReceived;
		/** Signal fires when connection has been established.
		*/
		sOnConnected _evtOnConnected;
		/** Signal fires when connection has been closed.
		*/
		sOnClose _evtOnClose;

	protected:
		/** Connecting thread.
		*/
		virtual unsigned int connecting() = 0;

		/** Main loop.
		*/
		virtual unsigned int loop() = 0;

		/** Sends proxy handshake. */
		bool sendProxyHandshake();

		/** Handles proxy handshake reply. */
		bool handleProxyHandshakeReply(const ByteBuffer& reply);

	protected:
		oThreadRunnerStore _threads;
		State _state;
		CriticalSection_w32 _critical;
		String _host;
		unsigned int _port;
		ByteBuffer _buffer;
		Proxy _proxy;
	};
}

#endif	// __STAMINA_SOCKET_H__