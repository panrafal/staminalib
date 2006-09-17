/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */

#ifndef __STAMINA_SOCKETCLIENT_H__
#define __STAMINA_SOCKETCLIENT_H__

#include "iSocket.h"

#include <Stamina/Object.h>
#include <Stamina/ObjectImpl.h>
#include <Stamina/String.h>

#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace Stamina {
	/** Base class to every client sockets.
	 *
	 * @author Krzysztof G³ogocki
	 */
	class SocketClient : public SharedObject<iSocket> {
	public:
		/** Connection state.
		 */
		enum State {
			stDisconnected,
			stDisconnecting,
			stConnecting,
			stConnected
		};

		STAMINA_OBJECT_CLASS_VERSION(Stamina::SocketClient, iSocket, Version(0,1,0,0));

		SocketClient() : _state(stDisconnected), _port(0) {}

		/** Establishes connection to host @a host on port @a port.
		 * @param host Hostname.
		 * @param port Port number.
		 */
		virtual bool connect(const StringRef& host, unsigned short port) = 0;
		/** Sends data to connected host.
		 * @return Number of bytes sent.
		 */
		virtual int send(const ByteBuffer& buffer) = 0;

		inline String getHost() const {
			return _host;
		}
		inline unsigned short getPort() const {
			return _port;
		}

		inline State getState() const {
			return _state;
		}
	public:
		/** Fires when socket has established connection.
		 */
		boost::signal<void ()> evtOnConnected;

		/** Fires when data has been received.
		 */
		boost::signal<void (const ByteBuffer&)> evtOnReceived;

		/** Fires when data is beeing send.
		 */
		boost::signal<void (const ByteBuffer&)> evtOnSend;
		
		/** Fires when connection is beeing closed.
		 */
		boost::signal<void ()> evtOnClose;

		/** Fires when error occures.
		 */
		boost::signal<void (unsigned int)> evtOnError;
	protected:
		String _host;
		unsigned short _port;
		State _state;
	};
}

#endif	// __STAMINA_SOCKETCLIENT_H__