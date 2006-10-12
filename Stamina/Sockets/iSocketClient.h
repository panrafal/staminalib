/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2005-2006 Krzysztof G³ogocki
 *
 *  $Id: $
 */

#ifndef __ISOCKETCLIENT_H__
#define __ISOCKETCLIENT_H__

#include <Stamina/Object.h>
#include <Stamina/String.h>

namespace Stamina {
	typedef unsigned short Port;
	typedef unsigned int Size;

	class iSocketClient : public iSharedObject {
	public:
		STAMINA_OBJECT_CLASS_VERSION(Stamina::iSocketClient, iObject, Version(0,1,0,0));
		/** Establishes connection to host @a host on port @a port.
		 * @param host Hostname.
		 * @param port Port number.
		 */
		virtual bool connect(const StringRef& host, Port port) = 0;

		virtual void close() = 0;

		virtual int write(const char* data, Size size) = 0;
		
		int write(const ByteBuffer& data) {
			return write((const char*)data.getBuffer(), data.getLength());
		}

		virtual int read(char* data, Size size) = 0;
		
		int read(ByteBuffer& data) {
			int bytes, overall = 0;
			char buff[512];
			while ((bytes = read(buff, 512)) > 0) {
				data.append((const unsigned char*)buff, bytes);
				overall += bytes;
			}
			return overall;
		}
	};
}

#endif	// __ISOCKETCLIENT_H__