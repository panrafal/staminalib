#include "stdafx.h"
#include "Socket.h"

namespace Stamina {

	bool Socket::connect(const StringRef& host, unsigned int port) {
		if (_state != stOffline)
			return false;
	
		_host = host;
		_port = port;
		_state = stConnecting;

		if (!_threads.runEx(boost::bind(&Socket::connecting, this), "Socket::connecting")) {
			throw ExceptionSocket("Cannot create connecting thread.");
		}
		return true;
	}

}