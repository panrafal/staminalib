/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */


#include "stdafx.h"
#include "SocketServer.h"

namespace Stamina {
	SocketServer::SocketServer(const oSocket& socket)
		: SocketController(socket) {
	}
}