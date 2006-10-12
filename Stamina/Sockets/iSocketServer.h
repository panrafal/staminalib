/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#ifndef __STAMINA_ISOCKETSERVER_H__
#define __STAMINA_ISOCKETSERVER_H__

#include <Stamina/Object.h>

namespace Stamina {
	typedef unsigned short Port;
	
	class iSocketServer : public iSharedObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::iSocketServer, iObject, Version(0,1,0,0));

		/** Creates socket and start listening on given port.
		 * @param port Port number to listening on.
		 */
		virtual void listen(Port port) = 0;

		/** Closes socket. No more operation would be possible.
		 *
		 */
		virtual void close() = 0;
	};
}

#endif	// __STAMINA_ISOCKETSERVER_H__