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
		MD5Context ctx;
		ctx.update(_digest, 16);
		ctx.update(inttostr(salt, 16, 8));
		setDigest(ctx.getDigest());
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