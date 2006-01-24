#ifndef __STAMINA_TCPCLIENT_H__
#define __STAMINA_TCPCLIENT_H__

#include "TCPSocket.h"

namespace Stamina {
	/** TCPClient represent functionality for clients.
	*/
	class TCPClient : public TCPSocket
	{
	public:
		/**
		@param major Major version of winsock.
		@param minor Minor version of winsock.
		*/
		TCPClient(int major = 2, int minor = 2) : TCPSocket(major, minor), _connEvent(NULL) {}
		~TCPClient();

		/** Establishes asynchronously connection to another socket application.
		* @param host Hostname.
		* @param port Port number.
		*/
		virtual bool connectAsync(const StringRef& host, unsigned port);

		/** Establishes synchronosouly connection.
		* @param host Hostname.
		* @param port Port number.
		*/
		virtual bool connectSync(const StringRef& host, unsigned int port);

		/** Gets hostname where connection is established to.
		*/
		inline String getHost() const {
			LockerCS(this->_critical);
			return _host;
		}
		/** Gets port number which connection is established on.
		*/
		inline int getPort() const {
			LockerCS(this->_critical);
			return _port;
		}

	protected:
		/** Socket invokes function when client is establishing the connection.
		*/
		virtual void onAccept();
		/** Socket invokes function when connection is established.
		*/
		virtual void onConnected();
		/** Socket invokes function when connection has been closed.
		*/
		virtual void onClose();
		/** Socket invokes function when an error occurs.
		@param err Error code.
		*/
		virtual void onError(int err);

	private:
		String _host;
		unsigned _port;
		HANDLE _connEvent;

		unsigned connecting();
	};
}

#endif	// __STAMINA_TCPCLIENT_H__