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

#include "iSocketServer.h"
#include "SocketClient.h"

#include <Stamina/Object.h>
#include <Stamina/String.h>

#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace Stamina {
	/** Base class for every server socket
	 *
	 * Support:
	 *	- proxy interface
	 *
	 * @author Krzysztof G³ogocki
	 */
	class SocketServer : public SharedObject<iSocketServer> {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::SocketServer, iSocketServer, Version(0,1,0,0));

		SocketServer() : _proxyType(ptDirect) {
		}

		/** State of socket.
		 *
		 */
		enum State {
			stOffline,
			stListen
		};

		/** Type of proxy.
		 *
		 */
		enum ProxyType {
			ptDirect,		// Direct connection
			ptSOCKS5		// Proxy SOCKS ver 5
		};

		/** Gets state of socket.
		 *
		 */
		inline State getState() const {
			return _state;
		}

		/** Gets port number.
		 *
		 */
		inline Port getPort() const {
			return _port;
		}

		inline void setProxyType(ProxyType proxyType) {
			_proxyType = proxyType;
		}
		inline ProxyType getProxyType() const {
			return _proxyType;
		}
	public:
		/** Fires when socket has established connection.
		 */
		boost::signal<void (SocketClient*)> evtOnAccept;
		
		/** Fires when connection is beeing closed.
		 */
		boost::signal<void ()> evtOnClose;

		/** Fires when error occures.
		 */
		boost::signal<void (const SocketError&)> evtOnError;

	protected:
		ProxyType _proxyType;
		State _state;
		Port _port;
	};

	typedef SharedPtr<SocketServer> oSocketServer;
}

#endif	// __STAMINA_SOCKETSERVER_H__