/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */

#ifndef __STAMINA_WINSOCKET_H__
#define __STAMINA_WINSOCKET_H__

#include <Stamina/Object.h>

#include <Winsock2.h>
#pragma comment(lib, "Ws2_32")

#include "SocketException.h"

namespace Stamina {
	/** Wrapper for Winsock.
	 *
	 * @author Krzysztof G³ogocki
	 */
	class WinSocket : public iObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::WinSocket, iObject, Version(0,1,0,0));

		/**
		 * @param major Winsock major version
		 * @param minor Winsock minor version
		 */
		WinSocket(int major, int minor);
		~WinSocket();


	protected:
		SOCKET _socket;
	private:
		static int _ref;
	};
}

#endif	// __STAMINA_WINSOCKET_H__