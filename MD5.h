/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once

namespace Stamina {

	unsigned char * MD5(const char * string , unsigned char * digest);
	char * MD5Hex(const char * string , char * digest);
	__int64 MD5_64(const char * string);

#ifdef _STRING_
	std::string MD5Hex(const char * string);
	std::string MD5FileHex(const std::string& fileName);
#endif


}