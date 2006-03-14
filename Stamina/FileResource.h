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

#include "Version.h"
#include "Helpers.h"
#include "WinHelper.h"

namespace Stamina {

	inline void * loadResourceData(HMODULE inst, const char* name, const char* type, HGLOBAL& rsrc, int* size) {
		HRSRC found = FindResource(inst, name, type);
		rsrc = 0;
		if (!found) return 0;
		rsrc = LoadResource(inst, found);
		if (!rsrc) return 0;
		if (size)
			*size = SizeofResource(inst, found);
		return LockResource(rsrc);
	}

	inline std::string loadString(UINT id, HINSTANCE inst = 0) {
		std::string buff;
		unsigned int size = LoadString( inst ? inst : Stamina::getInstance(), id, stringBuffer(buff, 2047), 2048);
		stringRelease(buff, size);
		return buff;
	}


	// File version

	class FileVersion {
	public:

		FileVersion(HMODULE module) {
			init(getModuleFileName(module));
		}

		FileVersion(const std::string& fileName) {
			init(fileName);
		}

		~FileVersion();

		bool empty() const {
			return _buffer == 0;
		}

		VS_FIXEDFILEINFO* getFixedFileInfo() const;

		Version getFileVersion() const {
			VS_FIXEDFILEINFO * vi = getFixedFileInfo();
			if (!vi) return Version(0);
			return Version(vi->dwFileVersionMS, vi->dwFileVersionLS);
		}

		Version getProductVersion() const {
			VS_FIXEDFILEINFO * vi = getFixedFileInfo();
			if (!vi) return Version(0);
			return Version(vi->dwProductVersionMS, vi->dwProductVersionLS);
		}
		
		bool chooseTranslation(int index = 0);

		std::string getString(const std::string& name);

		const char* getChar(const char* name, unsigned int* sizePtr = 0);

		unsigned int queryValue(const TCHAR* subBlock, LPVOID * buffer) const {
			if (_buffer == 0) return 0;
			unsigned int length;
			if (VerQueryValue(_buffer, (TCHAR*)subBlock, buffer, &length))
				return length;
			else
				return 0;
		}



	private:

		void init(const std::string& fileName);

		char* _buffer;
		std::string _stringBlock;
	};



};