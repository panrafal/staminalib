/*

The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License from
/LICENSE.HTML in this package or at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is "Stamina.lib" library code, released Feb 1, 2006.

The Initial Developer of the Original Code is "STAMINA" - Rafa³ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa³ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

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