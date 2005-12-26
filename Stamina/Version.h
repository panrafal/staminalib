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

#include <string.h>

namespace Stamina {

	class Version {
	public:

		inline Version(int version = 0) {
			this->major = (short)( ((version)>>28)&0xF );
			this->minor = (short)( ((version)>>20)&0xFF );
			this->release = (short)( ((version)>>12)&0xFF );
			this->build = (short)( ((version))&0xFFF );
		}
		inline Version(int high, int low) {
			this->major = (short)( high >> 16 );
			this->minor = (short)( high & 0xFFFF );
			this->release = (short)( low >> 16 );
			this->build = (short)( low & 0xFFFF );
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

		inline unsigned int getInt() const {
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
				s += itoa(release, buff, 10);
			}
			if (elements > 3 || build) {
				s += ".";
				s += itoa(build, buff, 10);
			}
			return s;
		}
#endif

	public:
		short major, minor, release, build;
	};


	enum enVersionCategory {
		versionUnknown,
		versionClass,
		versionModule,
		versionAPI
	};


	/** Defines version information about class/module/API. */
	class ModuleVersion {
	public:
		/**
		@warn name @b must be a static string! Don't use std::string, String, or anything like that in here!
		*/
		ModuleVersion(enVersionCategory category, const char* name, const Version& version):_category(category), _name(name), _version(version) {
		}

		bool operator == (const ModuleVersion&b) {
			return _category == b._category && stricmp(_name, b._name) == 0 && _version == b._version;
		}

		enVersionCategory getCategory() const {
			return _category;
		}

		const char* getName() const {
			return _name;
		}

		const Version& getVersion() const {
			return _version;
		}

	private:
		enVersionCategory _category;
		const char* _name;
		const Version _version;
	};

};

#ifndef __STAMINA_VERSIONCONTROL__

#define STAMINA_REGISTER_VERSION(NAME, MODULE)

#define STAMINA_REGISTER_CLASS_VERSION(CLASS)

#endif