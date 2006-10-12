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
#include "WinSocket.h"

namespace Stamina {

	int WinSocket::_ref = 0;

	WinSocket::WinSocket(int major, int minor) {
		WSAData wsaData;
		WORD wVersionRequested;

		if (_ref == 0) {
			wVersionRequested = MAKEWORD(major, minor);
			if (WSAStartup(wVersionRequested, &wsaData))
				throw WSASocketException(WSAGetLastError());
		}
		_ref++;
	}

	WinSocket::~WinSocket() {
		if (_ref == 1)
            WSACleanup();
		_ref--;
	}
}