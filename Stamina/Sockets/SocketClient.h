/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#ifndef __STAMINA_SOCKETCLIENT_H__
#define __STAMINA_SOCKETCLIENT_H__

#include "iSocketClient.h"
#include "SocketException.h"

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
	class SocketClient : public SharedObject<iSocketClient> {
	public:
		/** Connection state.
		 */
		enum State {
			stDisconnected,
			stDisconnecting,
			stConnecting,
			stConnected
		};

		STAMINA_OBJECT_CLASS_VERSION(Stamina::SocketClient, iSocketClient, Version(0,1,0,0));

		SocketClient() : _state(stDisconnected), _port(0) {}

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
		boost::signal<void ()> evtOnReceived;
		
		/** Fires when connection is beeing closed.
		 */
		boost::signal<void ()> evtOnClose;

		/** Fires when error occures.
		 */
		boost::signal<void (unsigned int)> evtOnError;
	protected:
		String _host;
		Port _port;
		State _state;
		CriticalSection_w32 _critical;
	};
}

#endif	// __STAMINA_SOCKETCLIENT_H__