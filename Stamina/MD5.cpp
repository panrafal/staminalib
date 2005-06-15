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

#include "MD5.h"
#include <md5.h>
#include "helpers.h"


using namespace std;

namespace Stamina {


	unsigned char * MD5(const char * string , unsigned char * digest) {
        MD5_CTX context;
        unsigned int len = strlen (string);
        MD5Init (&context);
        MD5Update (&context, (unsigned char*)string, len);
        MD5Final (digest, &context);
        return digest;
    }

	__int64 MD5_64(const char * string) {
		unsigned char c [16];
        MD5(string , c);

		unsigned char d [8];

		for (int i=0; i < 8; i++) {
			d[i] = c[i*2] + c[i*2+1];
		}

        return *((__int64*)d);
    }

	char * MD5Hex(const char * string , char * digest) {
		unsigned char c [16];
        MD5(string , c);
        for (int i = 0; i<16; i++)
			inttoch(c[i] , &digest[i*2] , 16 , 2 , 0);
        return digest;
    }
    std::string MD5Hex(const char * string) {
        char digest[33];
		return MD5Hex(string , digest);
	}

    string MD5FileHex(const string& fileName) {
        FILE *file;
        MD5_CTX context;
        int len;
        unsigned char buffer[1024], digest[16+32+1];

        if ((file = fopen (fileName.c_str(), "rb")) == NULL) {
            return "";
        } else {
            MD5Init (&context);
            while (len = fread (buffer, 1, 1024, file))
                MD5Update (&context, buffer, len);
            MD5Final (digest, &context);
            fclose (file);
        }
		char buff [3];
		buff[2] = 0;
        for (int i = 0; i<16; i++)
            strcpy((char*)&digest[i*2 + 16] , inttoch(digest[i] , buff, 16 , 2 , 0));
        digest[16+32] = 0;
        return (char*)(digest + 16);
    }


}