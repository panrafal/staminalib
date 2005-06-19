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
#ifndef __DT_CRYPT__
#define __DT_CRYPT__

#include <Stamina\MD5.h>

namespace Stamina { namespace DT {


	char * xor1_key(unsigned char * key , int type);

	void xor1_encrypt(const unsigned char * key , unsigned char * data , unsigned int size);

	void xor1_decrypt(const unsigned char * key , unsigned char * data , unsigned int size);

	/**Alias for xor2_encrypt()	
	*/
	void xor2_decrypt(const unsigned char* key, unsigned char * data , unsigned int size, unsigned int salt);

	/**Bi-directional xor based encryption function.



	@param key - bidirectional key used for en/decryption (16 bytes)
	@param data - data to be crypted (data is overwritten!)
	@param size - size of data bytes
	@param salt - additional value to make key more "inconsistent". Must be the same for both ways.
	*/
	void xor2_encrypt(const unsigned char* key, unsigned char * data , unsigned int size, unsigned int salt);


} }

#endif