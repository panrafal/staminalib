/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */


#ifndef __STAMINA_SOCKETCONTROLLER_H__
#define __STAMINA_SOCKETCONTROLLER_H__

#include "TCPSocket.h"

namespace Stamina {
	/** Base class for helper socket controller classes.
	*/
	class SocketController
		: public SharedObject<iSharedObject> {
	public:
		SocketController(const oSocket& socket) : _socket(socket) {}

		/** Gets connection state.
		*/
		inline Socket::State getState() const {
			return _socket->getState();
		}

		/** Closes connection.
		*/
		inline bool close() {
			return _socket->close();
		}

		/*** onError ***/
		inline bool setEvtOnError(fUnsigned f) {
			return _socket->setEvtOnError(f);
		}
		inline const sOnError& getEvtOnError() const {
			return _socket->getEvtOnError();
		}

		/*** onClose ***/
		inline bool setEvtOnClose(fVoid f) {
			return _socket->setEvtOnClose(f);
		}
		inline const sOnClose& getEvtOnClose() const {
			return _socket->getEvtOnClose();
		}
	protected:
		oSocket _socket;
	};
}

#endif	// __STAMINA_SOCKETCONTROLLER_H__