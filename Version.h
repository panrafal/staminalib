#pragma once

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

namespace Stamina {

	class Version {
	public:

		inline Version(int version = 0) {
			this->major = ((version)>>28)&0xF;
			this->minor = ((version)>>20)&0xFF;
			this->release = ((version)>>12)&0xFF;
			this->build = ((version))&0xFFF;
		}
		inline Version(int high, int low) {
			this->major = HIWORD(high);
			this->minor = LOWORD(high);
			this->release = HIWORD(low);
			this->build = LOWORD(low);
		}
		inline Version(short major, short minor, short release, short build) {
			this->major = major;
			this->minor = minor;
			this->release = release;
			this->build = build;
		}
		inline Version(const Version& b) {
			*this = b;
		}
		inline Version(const char* str) {
			minor = release = build = 0;
			char* v = (char*)str;
			this->major = (short)strtoul(v, &v, 10);
			if (*v != '.') return;
			this->minor = (short)strtoul(++v, &v, 10);
			if (*v != '.') return;
			this->release = (short)strtoul(++v, &v, 10);
			if (*v != '.') return;
			this->build = (short)strtoul(++v, &v, 10);
			if (*v != '.') return;
		}
		inline Version& operator = (const Version& b) {
			major = b.major;
			minor = b.minor;
			release = b.release;
			build = b.build;
			return *this;
		}
		inline bool operator == (const Version& b) const {
			return major == b.major && minor == b.minor && release == b.release && build == b.build;
		}
		inline bool operator != (const Version& b) const {
			return !(*this == b);
		}
		inline bool operator > (const Version& b) const {
			return this->getInt64() > b.getInt64();
		}
		inline bool operator < (const Version& b) const {
			return this->getInt64() < b.getInt64();
		}
		inline bool operator >= (const Version& b) const {
			return this->getInt64() >= b.getInt64();
		}
		inline bool operator <= (const Version& b) const {
			return this->getInt64() <= b.getInt64();
		}


		inline bool empty() const {
			return !major && !minor && !release && !build;
		}

		inline int getInt() const {
			return ((((major)&0xF)<<28) | (((minor)&0xFF)<<20) | (((release)&0xFF)<<12) | ((build)&0xFFF));
		}
		inline __int64 getInt64() const {
			return *((__int64*)this);
		}

#ifdef _STRING_
		/**Returns version string (x.x.x.x).
		@param Minimum number of version parts to return
		*/
		std::string getString(char elements = 2) const {
			char buff [10];
			std::string s;
			if (elements > 0 || major || minor || release || build)
				s += itoa(major, buff, 10);
			if (elements > 1 || minor || release || build) {
				s += ".";
				s += itoa(minor, buff, 10);
			}
			if (elements > 2 || release || build) {
				s += ".";
				s += itoa(minor, buff, 10);
			}
			if (elements > 3 || build) {
				s += ".";
				s += itoa(minor, buff, 10);
			}
			return s;
		}
#endif

	public:
		short major, minor, release, build;
	};

};