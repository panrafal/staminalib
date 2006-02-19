#ifndef __STAMINA_SOCKETSERVER_H__
#define __STAMINA_SOCKETSERVER_H__

#include "TCPSocket.h"

namespace Stamina {
	class SocketServer
		: public SharedObject<iSharedObject> {
	public:
		SocketServer(const oTCPSocket& socket);

		inline Socket::State getState() const {
			return _socket->getState();
		}

		inline void listen(unsigned int port) {
			_socket->listen(port);
		}

		inline bool close() {
			return _socket->close();
		}
	public:
		/** Signal fires when error occures.
		*/
		boost::signal<void (unsigned)> evtOnError;
		/** Signal fires when connection is incoming.
		* @param oSocket Socket of incoming connection.
		*/
		boost::signal<void (const oSocket&)> evtOnAccept;
		/** Signal fires when connection has been closed.
		*/
		boost::signal<void ()> evtOnClose;
	protected:
		virtual void onError(unsigned int err);
		virtual void onAccept(const oSocket& socket);
		virtual void onClose();
	protected:
		oTCPSocket _socket;
	};

	typedef SharedPtr<SocketServer> oSocketServer;
};

#endif	// __STAMINA_SOCKETSERVER_H__