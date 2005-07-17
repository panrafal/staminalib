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
		unsigned int size = 0;
		const char* buff = getChar(name.c_str(), &size);
		if (size != 0) {
			std::string s = buff;
			//s.assign(buff, size);
			return s;
		} else {
			return "";
		}
	}

	const char* FileVersion::getChar(const char* name, unsigned int* sizePtr) {
		if (_stringBlock.empty()) {
			if (! this->chooseTranslation())
				return "";
		}
		LPTSTR buff;
		unsigned int size = 0;
		if ((size = queryValue((_stringBlock + name).c_str(), (void**)&buff)) != 0) {
			if (sizePtr) *sizePtr = size;
			return buff;
		} else {
			if (sizePtr) *sizePtr = 0;
			return "";
		}
	}


	VS_FIXEDFILEINFO* FileVersion::getFixedFileInfo() const {
		VS_FIXEDFILEINFO * vi;
		if (this->queryValue("\\" , (void **)&vi) > 0)
			return vi;
		else
			return 0;
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

// -----------------------------------



};