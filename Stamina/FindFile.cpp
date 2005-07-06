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
#include "FindFileFiltered.h"

namespace Stamina {


	bool FindFile::find() {
		while (1) {
			if (_file == 0) {
				_file = FindFirstFile(_mask.c_str(), _found.getDataRef());
				if (_file == 0) return false;
			} else {
				if (FindNextFile(_file, _found.getDataRef()) == FALSE)
					return false;
			}
			if (this->filter() == true)
				return true;
		};
		return false;
	}

	bool FindFile::filter() {
		if (_includeParent == false && _found.isParentPointer()) return false;
		if ((_found.getAttributes() & _require) != _require) return false;
		if ((_found.getAttributes() & _exclude) != 0) return false;
		return true;
	}


};