#ifndef __STAMINA_SOCKETCLIENT_H__
#define __STAMINA_SOCKETCLIENT_H__

#include "TCPSocket.h"
#include <Stamina/Event.h>

namespace Stamina {
	class SocketClient 
		: public SharedObject<iSharedObject> {
	public:
		SocketClient(const oSocket& socket);

		/** Establishes a connection.
		* @param timeout Period of time to wait for the connection.
		* @remark Method blocks the process untill connection is established.
		*/
		bool connect(const StringRef& host, unsigned int port, unsigned int timeout);
		
		/** Establishes a connection asynchronously.
		*/
		bool connect(const StringRef& host, unsigned int port);
		
		/** Sends binary data synchronously.
		*/
		SocketClient& operator<<(const ByteBuffer& right);
		/** Sends string synchronously.
		*/
		SocketClient& operator<<(const StringRef& right);
		
		/** Receives binary data synchronously.
		*/
		SocketClient& operator>>(ByteBuffer& right);
		/** Receives string synchronously.
		*/
		SocketClient& operator>>(String& right);
		/** @return Client state.
		*/
		inline Socket::State getState() const {
			ObjLocker locker(_socket);
			return _socket->getState();
		}
		/** @return Host.
		*/
		inline String getHost() const {
			ObjLocker locker(_socket);
			return _socket->getHost();
		}

		/** @return Port number.
		*/
		inline unsigned int getPort() const {
			ObjLocker locker(_socket);
			return _socket->getPort();
		}
	public:
		/** Signal fires when data has been received.
		*/
		boost::signal<void (const ByteBuffer&)> evtOnReceived;
		/** Signal fires when connection has been established.
		*/
		boost::signal<void ()> evtOnConnected;
		/** Signal fires when error occures.
		*/
		boost::signal<void (unsigned)> evtOnError;
		/** Signal fires when connection has been closed.
		*/
		boost::signal<void ()> evtOnClose;
	protected:
		oSocket _socket;
	protected:
		virtual void onConnected();
		virtual void onReceived(const ByteBuffer& buff);
		virtual void onError(unsigned int err);
		virtual void onClose();
	private:
		Event _connected;
		Event _received;

		bool _sync;	// synchronous reading
		ByteBuffer _buff;
	};

	typedef SharedPtr<SocketClient> oSocketClient;

	oSocketClient& operator<<(oSocketClient& client, const ByteBuffer& buff);
	oSocketClient& operator>>(oSocketClient& client, ByteBuffer& buff);
}

#endif	// __STAMINA_SOCKETCLIENT_H__