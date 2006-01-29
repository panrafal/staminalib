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


#pragma once
#ifndef __STAMINA_LIB__
#define __STAMINA_LIB__

#include "Version.h"

namespace Stamina { 

	/**Basic library information*/
	namespace Lib {
		const char* const name = "Stamina.Lib";

		const ModuleVersion version = ModuleVersion(versionAPI, name, Version(0, 3, 0, 0));

		const char* const copyright = "Copyright (C)2005 Stamina";
		const char* const moreCopyright = "Copyright (C)2005 Rafa³ Lindemann";

		const bool isStatic = true;

	} 
}

#endif