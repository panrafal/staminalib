/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: $
 *
 *
 *  Based on code from http://www.createwindow.com/programming/crc32/
 */

#pragma once

#ifndef __STAMINA_CRC32__
#define __STAMINA_CRC32__

namespace Stamina {

	class CRC32 {

	public:
		CRC32();

		unsigned long calculate(const char * text);  // Creates a CRC from a text string 
		unsigned long add(const char * buffer , unsigned int size);  // Continues a CRC from a buffer 
		unsigned long getState();
		void reset();

	private:
		unsigned long _lookup [256];
		unsigned long _state;

		void buildLookup();  // Builds lookup table array 
		unsigned long reflect(unsigned long ref, char ch);  // Reflects CRC bits in the lookup table 

	};

}
#endif