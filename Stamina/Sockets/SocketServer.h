/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */

#ifndef __STAMINA_SOCKETSERVER_H__
#define __STAMINA_SOCKETSERVER_H__

#include "iSocket.h"

#include <Stamina/Object.h>
#include <Stamina/String.h>

namespace Stamina {
	/** Base class for every server socket
	 *
	 * Support:
	 *	- proxy interface
	 *
	 * @author Krzysztof G³ogocki
	 */
	class SocketServer : public SharedObject<iSocket> {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::SocketServer, iSocket, Version(0,1,0,0));

		/** State of socket.
		 *
		 */
		enum State {
			stOffline,
			stOpenning,
			stListening
		};

		/** Creates socket and start listening on given port.
		 * @param port Port number to listening on.
		 */
		virtual void listen(unsigned short port) = 0;

		/** Gets state of socket.
		 *
		 */
		inline State getState() const {
			return _state;
		}

		/** Gets port number.
		 *
		 */
		inline unsigned short getPort() const {
			return _port;
		}

	protected:
		State _state;
		unsigned short _port;
	};
}

#endif	// __STAMINA_SOCKETSERVER_H__