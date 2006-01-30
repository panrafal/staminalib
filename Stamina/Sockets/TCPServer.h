#ifndef __STAMINA_TCPSERVER_H__
#define __STAMINA_TCPSERVER_H__

#include "TCPSocket.h"

namespace Stamina {
	/** Server functionality.
	*/
	class TCPServer : public TCPSocket
	{
	public:
		TCPServer(int major = 2, int minor = 0) : TCPSocket(major, minor) {}

		/** Permits an incoming connection attempt on a socket.
		*/
		virtual void accept();
		
	protected:
		/** Socket invokes function when has got something on input.
		*/
		virtual void onReceived();
		/** Socket invokes function when client is establishing the connection.
		*/
		virtual void onAccept();
		/** Socket invokes function when connection is established.
		*/
		virtual void onConnected();
		/** Socket invokes function when connection has been closed.
		*/
		virtual void onClose();
		/** Socket invokes function when an error occurs.
		@param err Error code.
		*/
		virtual void onError(int err);
	};
}

#endif	// __STAMINA_TCPSERVER_H__