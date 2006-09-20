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