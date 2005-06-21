/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#include "stdafx.h"
#include "Crypt.h"

namespace Stamina { namespace DT {

	char * xor1_key(unsigned char * key , int type) {
		if (!type) return (char*) key;
		char * bck = (char*)key;
		while (*key) {
			switch (type) {
				case 1:
					if (*key < 32) *key = 32;
					if (*key > 127) *key = 127;
					*key+=125;
					break;
				case 2:
					*key-=31;
					break;
			}
			//    *key-=58;
			//    if (!*key) *key = 53;
			key++;
		}
		return bck;
	}

	void xor1_encrypt(const unsigned char * key , unsigned char * data , unsigned int size) {
		unsigned int ki=0;
		if (!size) size = strlen((char *)data);
		unsigned int ksize = strlen((char *)key);
		int j = 0;
		for (unsigned int p=0;p<size;p++) {
			*data = (*data ^ key[ki]) + (unsigned char)((j) &  0xFF);// | (j*2);
			//    *data = *data;
			data++;
			ki++;
			if (ki>=ksize) ki=0;
			j++;
		}
	}

	void xor1_decrypt(const unsigned char * key , unsigned char * data , unsigned int size) {
		unsigned int ki=0;
		unsigned int ksize = strlen((char *)key);

		int j = 0;
		for (unsigned int p=0;p<size;p++) {
			*data = (*data - (unsigned char)((j) & 0xFF))  ^ key[ki];// | (j*2);
			data++;
			ki++;
			if (ki>=ksize) ki=0;
			j++;
		}

	}


	// -------------------------------------------------------------
	// nowy XOR 

	const int xor2KeySize = 16;
	const int xor2Key32Size = xor2KeySize / 4;
	const int xor2SaltMod = 1024;
	const int xor2SaltMin = 255;

	void xor2_encrypt(const unsigned char* key, unsigned char * data , unsigned int size, unsigned int salt) {
		unsigned int* key32 = (unsigned int*)key;
		unsigned int* data32 = (unsigned int*)data;
		salt = (salt % xor2SaltMod) + xor2SaltMin;
		unsigned int i = size + salt;
		// liczymy 32bit
		while (size >= 4) {
			i--;
			*data32 = (*data32 ^ (key32[i % xor2Key32Size] * (i)));
			data32++;
			size -= 4;
		}
		// dokañczamy 8bit
		while (size) {
			i--;
			*data = (*data ^ unsigned char((key[i % xor2KeySize] * (i)) & 0xFF));
			data++;
			size --;
		}
	}

	void xor2_decrypt(const unsigned char* key, unsigned char * data , unsigned int size, unsigned int salt) {
		xor2_encrypt(key, data, size, salt);
	}



} }