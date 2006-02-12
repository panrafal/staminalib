#ifndef __STAMINA_TCPSOCKET_H__
#define __STAMINA_TCPSOCKET_H__

#include "Socket.h"


#include <Winsock2.h>
#pragma comment(lib, "Ws2_32")

namespace Stamina {
	class TCPSocket:
		public Socket {
		friend class TCPSocket;
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::TCPSocket, Socket, Version(0,1,0,0));
	public:
		/**
		* @param major - winsock's major ver
		* @param minor - winsock's minor ver
		*/
		TCPSocket(int major = 2, int minor = 2);
		TCPSocket(SOCKET socket, const StringRef& host, unsigned int port);
		TCPSocket(const TCPSocket& socket);

		~TCPSocket();

		/** Closes a connection.
		*/
		virtual bool close();

		/** Sends data through socket.
		*/
		virtual void send(const ByteBuffer& data);

		/** Starts listening for an incoming connection.
		*/
		virtual void listen(unsigned int port);

		TCPSocket& operator=(const TCPSocket& right);

	protected:
		void onConnected();
		void onWrite();
		void onReceived();
		void onClose();
		void onAccept();
	private:
		SOCKET _socket;
		HANDLE _event;	/// WSAEvent
		HANDLE _thread;
		HANDLE _endEvent;
		static bool _wsa;	/// wsa was initialized
	private:
		virtual unsigned int loop();
		virtual unsigned int connecting();
	};

	typedef SharedPtr<TCPSocket> oTCPSocket;
};

#endif	// __STAMINA_TCPSOCKET_H_