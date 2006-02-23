/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */


#ifndef __STAMINA_SOCKETCLIENT_H__
#define __STAMINA_SOCKETCLIENT_H__

#include "SocketController.h"
#include <Stamina/Event.h>

namespace Stamina {
	class SocketClient 
		: public SocketController {
	public:
		SocketClient(const oSocket& socket);

		/** Establishes a connection.
		* @param timeout Period of time to wait for the connection.
		* @remark Method blocks the process untill connection is established.
		*/
		bool connect(const StringRef& host, unsigned int port, unsigned int timeout);
		
		/** Establishes a connection asynchronously.
		*/
		bool connect(const StringRef& host, unsigned int port);
		
		/** Sends binary data synchronously.
		*/
		SocketClient& operator<<(const ByteBuffer& right);
		/** Sends string synchronously.
		*/
		SocketClient& operator<<(const StringRef& right);
		
		/** Receives binary data synchronously.
		*/
		SocketClient& operator>>(ByteBuffer& right);
		/** Receives string synchronously.
		*/
		SocketClient& operator>>(String& right);
		
		/** @return Host.
		*/
		inline String getHost() const {
			ObjLocker locker(_socket);
			return _socket->getHost();
		}

		/** @return Port number.
		*/
		inline unsigned int getPort() const {
			ObjLocker locker(_socket);
			return _socket->getPort();
		}

		/** onReceived **/
		inline bool setEvtOnReceived(fBuffer f) {
			return this->_socket->setEvtOnReceived(f);
		}
		inline const sOnReceived& getEvtOnReceived() const {
			return this->_socket->getEvtOnReceived();
		}

		/** onConnected **/
		inline bool setEvtOnConnected(fVoid f) {
			return this->_socket->setEvtOnConnected(f);
		}
		inline const sOnConnected& getEvtOnConnected() const {
			return this->_socket->getEvtOnConnected();
		}
	protected:
		virtual void onConnected();
		virtual void onReceived(const ByteBuffer& buff);
	protected:
		Event _connected;
		Event _received;

		bool _sync;	// synchronous reading
		ByteBuffer _buff;
	};

	typedef SharedPtr<SocketClient> oSocketClient;

	oSocketClient& operator<<(oSocketClient& client, const ByteBuffer& buff);
	oSocketClient& operator<<(oSocketClient& client, const String& txt);
	oSocketClient& operator>>(oSocketClient& client, ByteBuffer& buff);
	oSocketClient& operator>>(oSocketClient& client, String& txt);
}

#endif	// __STAMINA_SOCKETCLIENT_H__