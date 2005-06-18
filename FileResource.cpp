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

#include "FileResource.h"
#include "Helpers.h"

namespace Stamina {


	FileVersion::FileVersion(const std::string& fileName) {
		int size = GetFileVersionInfoSize(fileName.c_str(),0);
		if (size > 0) {
			_buffer = new char [size];
			GetFileVersionInfo(fileName.c_str(), 0, size , _buffer);
		} else
			_buffer = 0;
	}

	FileVersion::~FileVersion() {
		if (_buffer != 0) {
			delete [] _buffer;
		}
	}

	std::string FileVersion::getString(const std::string& name) {
		if (_stringBlock.empty()) {
			if (! this->chooseTranslation())
				return "";
		}
		LPTSTR buff;
		unsigned int size = 0;
		if ((size = queryValue((_stringBlock + name).c_str(), (void**)&buff)) != 0) {
			std::string s;
			s.assign(buff, size);
			return s;
		} else {
			return "";
		}
	}


	VS_FIXEDFILEINFO* FileVersion::getFixedFileInfo() {
		VS_FIXEDFILEINFO * vi;
		this->queryValue("\\" , (void **)&vi);
		return vi;
	}

	std::string FileVersion::VersionInfo::getString(char elements) {
		std::string s;
		if (elements > 0 || major || minor || release || build)
			s += inttostr(major);
		if (elements > 1 || minor || release || build)
			s += "." + inttostr(minor);
		if (elements > 2 || release || build)
			s += "." + inttostr(minor);
		if (elements > 3 || build)
			s += "." + inttostr(minor);
		return s;
	}
	
	bool FileVersion::chooseTranslation(int index) {
        struct LANGANDCODEPAGE {
			WORD wLanguage;
			WORD wCodePage;
        } *lpTranslate;
		unsigned int size;
		size = queryValue("\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate);
        if (size == 0 || index * sizeof(lpTranslate) >= size)
			return false;

		this->_stringBlock = stringf("\\StringFileInfo\\%04X%04X\\", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
		return true;
	}



};