#include "stdafx.h"
#include "SocketServer.h"

namespace Stamina {
	SocketServer::SocketServer(const oSocket& socket)
		: SocketController(socket) {
	}
}