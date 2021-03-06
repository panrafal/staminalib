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

The Initial Developer of the Original Code is "STAMINA" - Rafa� Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa� Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

*/

#pragma once

#include "Stamina.h"
#include "String.h"
#include <md5/md5.h>
#include <memory.h>

namespace Stamina {

	class MD5Digest {
	public:
		typedef unsigned char tDigest [16];

		friend class MD5Context;
	public:
		inline MD5Digest() {
			this->reset();
		}

		inline MD5Digest(tDigest digest) {
			memcpy(_digest, digest, 16);
		}

		inline MD5Digest(const StringRef& string) {
			calculate(string);
		}

		inline MD5Digest(const MD5Digest& b) {
			memcpy(_digest, b._digest, 16);
		}

		inline MD5Digest& operator = (const MD5Digest& b) {
			this->setDigest(b);
			return *this;
		}

		inline bool operator == (const MD5Digest& b) {
			return this->isDigestEqual(b._digest);
		}

		inline bool operator != (const MD5Digest& b) {
			return (*this == b) == false;
		}

		MD5Digest operator + (const MD5Digest& b);

		inline void setDigest(const tDigest digest) {
			memcpy(_digest, digest, 16);
		}

		inline void setDigest(const MD5Digest& digest) {
			this->setDigest(digest.getDigest());
		}

		inline bool isDigestEqual(const tDigest digest) {
			return memcmp(_digest, digest, 16) == 0;
		}

		inline void calculate(const void* buffer, unsigned int size);

		inline void calculate(const StringRef& string) {
			calculate(string.getData(), string.getDataSize());
		}

		void calculateForFile(const char* filename);

		const unsigned char* getDigest() const {
			return _digest;
		}

		void getDigest(unsigned char* digest) const {
			memcpy(digest, _digest, 16);
		}

		String getHex() const {
			char b [33];
			this->getHex(b);
			return b;
		}

		void getHex(char* buffer) const;

		__int64 getInt64() const;

		void addSalt(int salt);

		inline void reset() {
			memset(_digest, 0, 16);
		}

		bool empty() const {
			for (int i = 0; i < 4; i++) {
				if (((int*)_digest)[i] != 0) return false;
			}
			return true;
		}

	private:
		tDigest _digest;
	};


	class MD5Context {
	public:

		inline MD5Context() {
	        MD5Init (&_context);
		}
		inline void update(const void* buff, unsigned int size) {
	        MD5Update (&_context, (unsigned char*)buff, size);
		}
		inline void update(const StringRef& string) {
			this->update(string.getData(), string.getDataSize());
		}

		
		inline void getDigest(unsigned char digest [16]) {
	        MD5Final (digest, &_context);
		}

		inline MD5Digest getDigest() {
			MD5Digest md5;
			this->getDigest(md5._digest);
			return md5;
		}

	private:
        MD5_CTX _context;
	};


	inline void MD5Digest::calculate(const void* buffer, unsigned int size) {
		MD5Context ctx;
		ctx.update(buffer, size);
		ctx.getDigest(_digest);
	}


	inline unsigned char * MD5(const StringRef& string , unsigned char * digest) {
		MD5Context ctx;
		ctx.update(string);
		ctx.getDigest(digest);
        return digest;
    }

	inline unsigned char * MD5Salted(const StringRef& string , unsigned char * digest, int salt) {
		MD5Digest md5(string);
		md5.addSalt(salt);
		md5.getDigest(digest);
		return digest;
	}

	inline char * MD5Hex(const StringRef& string , char * digest) {
		MD5Digest md5(string);
		md5.getHex(digest);
		return digest;
	}

	inline __int64 MD5_64(const StringRef& string) {
		MD5Digest md5(string);
		return md5.getInt64();
	}

#ifdef _STRING_
	inline String MD5Hex(const StringRef& string) {
		MD5Digest md5(string);
		return md5.getHex();
	}

	inline String MD5FileHex(const StringRef& fileName) {
		MD5Digest md5;
		md5.calculateForFile(fileName.c_str());
		return md5.getHex();
	}
#endif


}