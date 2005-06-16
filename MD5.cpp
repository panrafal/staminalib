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
#include "helpers.h"


using namespace std;

namespace Stamina {





	MD5Digest MD5Digest::operator + (const MD5Digest& b) {
		MD5Context ctx;
		ctx.update(_digest, 16);
		ctx.update(b._digest, 16);
		return ctx.getDigest();
	}

	void MD5Digest::getHex(char* buffer) const {
        for (int i = 0; i<16; i++)
			inttoch(_digest[i] , buffer + (i*2) , 16 , 2 , 0);
	}

	__int64 MD5Digest::getInt64() const {
		unsigned char d [8];
		for (int i=0; i < 8; i++) {
			d[i] = _digest[i*2] + _digest[i*2+1];
		}
        return *((__int64*)d);
	}


	void MD5Digest::addSalt(int salt) {
	}

	void MD5Digest::calculateForFile(const char* filename) {
        FILE *file;
        int len;
        unsigned char buffer[1024];

        if ((file = fopen (filename, "rb")) == NULL) {
			this->reset();
            return;
        } else {
			MD5Context ctx;
            while (len = fread (buffer, 1, 1024, file))
				ctx.update(buffer, len);
			this->setDigest(ctx.getDigest());
            fclose (file);
        }
	}


}