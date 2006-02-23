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

#include "SocketController.h"

namespace Stamina {
	class SocketServer
		: public SocketController {
	public:
		SocketServer(const oSocket& socket);

		inline void listen(unsigned int port) {
			_socket->listen(port);
		}

		inline bool setEvtOnAccept(fSocket f) {
			return this->_socket->setEvtOnAccept(f);
		}
		inline const sOnAccept& getEvtOnAccept() const {
			return this->_socket->getEvtOnAccpet();
		}
	};

	typedef SharedPtr<SocketServer> oSocketServer;
};

#endif	// __STAMINA_SOCKETSERVER_H__