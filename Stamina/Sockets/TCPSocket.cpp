#include "stdafx.h"
#include "TCPSocket.h"
#include <vector>
using namespace std;

namespace Stamina {
	TCPSocket::TCPSocket(int major, int minor)
		: _threads(new ThreadRunnerStore), _event(NULL) {
		WSAData wsaData;
		WORD wVersionRequested;

		wVersionRequested = MAKEWORD(major, minor);
		WSAStartup(wVersionRequested, &wsaData);

		_state = stOffline;
	}

	TCPSocket::~TCPSocket() {
		if (this->_state != this->stOffline)
			close();
		if (_event)
			WSACloseEvent(_event);
		WSACleanup();
	}

	bool TCPSocket::close()
	{
		if (this->_state == this->stOffline)
			return false;

		if (this->_socket && this->_socket != INVALID_SOCKET)
		{
			LINGER lin = {0};
			lin.l_linger = 0;
			lin.l_onoff = 1;
			setsockopt(this->_socket, SOL_SOCKET, SO_LINGER, (const char*)&lin, 4);
			closesocket(this->_socket);
			this->_socket = NULL;
			return true;
		}
		return false;
	}
	
	unsigned TCPSocket::loop() {
		DWORD wr;
		WSANETWORKEVENTS nev = {0};

		while (this->_state != iSocket::stOffline &&
			this->_state != this->stDisconnecting) {

			wr = WaitForSingleObject(_event, -1);
			if(wr != WAIT_OBJECT_0)
				return 0;

			WSAEnumNetworkEvents(_socket, _event, &nev);
			if (nev.lNetworkEvents & FD_READ)
				onReceived();
			else if (nev.lNetworkEvents & FD_ACCEPT)
				onAccept();
			else if (nev.lNetworkEvents & FD_CONNECT) {
				this->_state = stConnected;
				onConnected();
			}
			else if (nev.lNetworkEvents & FD_WRITE)	// pozniej moze sie przydac
			{
				onConnected();
			}
			else if (nev.lNetworkEvents & FD_CLOSE) {
				this->_state = this->stOffline;
				onClose();
			}
		}
		return 0;
	}

	void TCPSocket::send(char* data, unsigned int size) {
		if (data && size) {
			int sent;
			while ((sent = ::send(_socket, data, size, 0)) < size)
				if (sent == WSAEWOULDBLOCK)
					Sleep(100);
				else if (sent < 0)
				{
					onError(WSAGetLastError());
					return;
				}
		}
	}

	void TCPSocket::onReceived() {
		vector<char> theVector;
		char buff;
		char* data = 0;

		// pobieramy kolejne bajty i wrzucamy do tablicy
		while (recv(_socket, &buff, 1, 0) != SOCKET_ERROR)
			theVector.push_back(buff);

		data = new char[theVector.size()];
		/// kopiujemy dane
		for (size_t i = 0; i < theVector.size(); i++)
			data[i] = theVector[i];

		// wysylamy sygnal
		this->evtOnReceived(data, theVector.size());
		delete[] data;
	}
};