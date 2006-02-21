#include "stdafx.h"
#include "Socket.h"

namespace Stamina {

	bool Socket::connect(const StringRef& host, unsigned int port) {
		if (_state != stOffline)
			return false;
	
		_host = host;
		_port = port;
		_state = stConnecting;

		_threads->run(boost::bind(&Socket::connecting, this), "Socket::connecting");
		return true;
	}

	bool Socket::setEvtOnError(fUnsigned f) {
		if (f.empty())
			return false;
		_evtOnError.connect(f);
		return true;
	}

	bool Socket::setEvtOnAccept(fSocket f) {
		if (f.empty())
			return false;
		_evtOnAccept.connect(f);
		return true;
	}

	bool Socket::setEvtOnConnected(fVoid f) {
        if (f.empty())
			return false;
		_evtOnConnected.connect(f);
		return true;
	}

	bool Socket::setEvtOnReceived(fBuffer f) {
		if (f.empty())
			return false;
		_evtOnReceived.connect(f);
		if (!_buffer.isEmpty()) {
			_evtOnReceived(_buffer);
			_buffer.reset();
		}
		return true;
	}

	bool Socket::setEvtOnClose(fVoid f) {
		if (f.empty())
			return false;
		_evtOnClose.connect(f);
		return true;
	}

}