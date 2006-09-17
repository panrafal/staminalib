/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */

#ifndef __STAMINA_IPROXY_H__
#define __STAMINA_IPROXY_H__

#include <Stamina/Object.h>
#include <Stamina/String.h>
#include "SocketClient.h"

namespace Stamina {
	/** Proxy interface.
	 *
	 */
	class iProxy {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::iSocket, iObject, Version(0,1,0,0));
		
		/** Return true if any proxy transaction is needed.
		 *
		 */
		virtual bool use() = 0;

		/** Invokes when socket has established connection to host.
		 *
		 */
		virtual ByteBuffer onConnected() = 0;

		/** Invokes when data should be sent.
		 *
		 */
		virtual void onSend(ByteBuffer& buffer) = 0;

		/**
		 *
		 */
		virtual void onReceive(const SocketClient& socket, const ByteBuffer& buffer) = 0;
		virtual void onAccept(const SocketClient& socket) = 0;
		virtual void onDisconnect(const SocketClient& socket) = 0;
	};
}

#endif	// __STAMINA_IPROXY_H__