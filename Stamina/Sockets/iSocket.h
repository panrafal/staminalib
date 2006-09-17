/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id:  $
 */


#ifndef __STAMINA_ISOCKET_H__
#define __STAMINA_ISOCKET_H__

#include <Stamina/Object.h>

namespace Stamina {
	/** Base interface for socket.
	 *
	 * @author Krzysztof G³ogocki
	 */
	class iSocket : public iSharedObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::iSocket, iObject, Version(0,1,0,0));
	public:
		virtual public void close() = 0;
	};
};

#endif	// __STAMINA_ISOCKET_H__