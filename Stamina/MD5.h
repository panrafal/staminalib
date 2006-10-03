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

#include <MD5\md5.h>
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

		inline MD5Digest(const char* string) {
			calculate(string);
		}

		inline MD5Digest(const std::string& string) {
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

		inline void calculate(const char* string) {
			calculate(string, strlen(string));
		}

#ifdef _STRING_
		inline void calculate(const std::string& string) {
			calculate(string.c_str(), string.size());
		}
#endif

		void calculateForFile(const char* filename);

		const unsigned char* getDigest() const {
			return _digest;
		}

		void getDigest(unsigned char* digest) const {
			memcpy(digest, _digest, 16);
		}

#ifdef _STRING_
		std::string getHex() const {
			char b [33];
			this->getHex(b);
			return b;
		}
#endif

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
		inline void update(const char* string) {
			this->update(string, strlen(string));
		}
#ifdef _STRING_
		inline void update(const std::string& string) {
			this->update(string.c_str(), string.length());
		}
#endif
		
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


	inline unsigned char * MD5(const char * string , unsigned char * digest) {
		MD5Context ctx;
		ctx.update(string);
		ctx.getDigest(digest);
        return digest;
    }

	inline unsigned char * MD5Salted(const char * string , unsigned char * digest, int salt) {
		MD5Digest md5(string);
		md5.addSalt(salt);
		md5.getDigest(digest);
		return digest;
	}

	inline char * MD5Hex(const char * string , char * digest) {
		MD5Digest md5(string);
		md5.getHex(digest);
		return digest;
	}

	inline __int64 MD5_64(const char * string) {
		MD5Digest md5(string);
		return md5.getInt64();
	}

#ifdef _STRING_
	inline std::string MD5Hex(const char * string) {
		MD5Digest md5(string);
		return md5.getHex();
	}

	inline std::string MD5FileHex(const std::string& fileName) {
		MD5Digest md5;
		md5.calculateForFile(fileName.c_str());
		return md5.getHex();
	}
#endif


}