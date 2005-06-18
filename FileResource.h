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

	void * loadResourceData(HMODULE inst, const char* name, const char* type, HGLOBAL& rsrc, int* size) {
		HRSRC found = FindResource(inst, name, type);
		rsrc = 0;
		if (!found) return 0;
		rsrc = LoadResource(inst, found);
		if (!rsrc) return 0;
		if (size)
			*size = SizeofResource(inst, found);
		return LockResource(rsrc);
	}


	// File version

	class FileVersion {
	public:

		class VersionInfo {
		public:

			VersionInfo(int version = 0) {
				this->major = ((version)>>28)&0xF;
				this->minor = ((version)>>20)&0xFF;
				this->release = ((version)>>12)&0xFF;
				this->build = ((version))&0xFFF;
			}
			VersionInfo(int high, int low) {
				this->major = HIWORD(high);
				this->minor = LOWORD(high);
				this->release = HIWORD(low);
				this->build = LOWORD(low);
			}
			VersionInfo(short major, short minor, short release, short build) {
				this->major = major;
				this->minor = minor;
				this->release = release;
				this->build = build;
			}

			bool empty() {
				return !major && !minor && !release && !build;
			}

			int getInt() {
				return ((((major)&0xF)<<28) | (((minor)&0xFF)<<20) | (((release)&0xFF)<<12) | ((build)&0xFFF));
			}

			/**Returns version string (x.x.x.x).
			@param Minimum number of version parts to return
			*/
			std::string getString(char elements = 2);
		public:
			short major, minor, release, build;
		};

		FileVersion(const std::string& fileName);

		~FileVersion();

		bool empty() {
			return _buffer == 0;
		}

		VS_FIXEDFILEINFO* getFixedFileInfo();

		VersionInfo getFileVersion() {
			VS_FIXEDFILEINFO * vi = getFixedFileInfo();
			if (!vi) return VersionInfo(0);
			return VersionInfo(vi->dwFileVersionMS, vi->dwFileVersionLS);
		}

		VersionInfo getProductVersion() {
			VS_FIXEDFILEINFO * vi = getFixedFileInfo();
			if (!vi) return VersionInfo(0);
			return VersionInfo(vi->dwProductVersionMS, vi->dwProductVersionLS);
		}
		
		bool chooseTranslation(int index = 0);

		std::string getString(const std::string& name);

		unsigned int queryValue(const TCHAR* subBlock, LPVOID * buffer) {
			if (_buffer == 0) return 0;
			unsigned int length;
			if (VerQueryValue(_buffer, (TCHAR*)subBlock, buffer, &length))
				return length;
			else
				return 0;
		}



	private:
		char* _buffer;
		std::string _stringBlock;
	};

	struct FILEVERSIONINFO {
	    VS_FIXEDFILEINFO * ffi;
	    WORD major , minor , release , build;
        bool readInfo;
	    std::string Comments , InternalName , ProductName ,
		    CompanyName , LegalCopyright , ProductVersion ,
		    FileDescription , LegalTrademarks , PrivateBuild ,
		    FileVersion , OriginalFilename , SpecialBuild ,
		    URL;
        FILEVERSIONINFO(bool readInfo=false) {major=minor=release=build=0;ffi=0;this->readInfo=readInfo;}
	};

	// DEFINES

	bool FileVersionInfo(const char * fn , char * fmt=0 , FILEVERSIONINFO * res = 0);


};