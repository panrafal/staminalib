#include "stdafx.h"
#include "SocketServer.h"

namespace Stamina {
	SocketServer::SocketServer(const oTCPSocket &socket)
		: _socket(socket) {
		_socket->evtOnError.connect(boost::bind(&SocketServer::onError, this, _1));
		_socket->evtOnClose.connect(boost::bind(&SocketServer::onClose, this));
		_socket->evtOnAccept.connect(boost::bind(&SocketServer::onAccept, this, _1));
	}

	void SocketServer::onError(unsigned int err) {
		evtOnError(err);
	}

	void SocketServer::onClose() {
		evtOnClose();
	}

	void SocketServer::onAccept(const oSocket& socket) {
		evtOnAccept(socket);
	}
}