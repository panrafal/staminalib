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
#ifndef __STAMINA_LIB__
#define __STAMINA_LIB__

#include "Version.h"

namespace Stamina { 

	/**Basic library information*/
	namespace Lib {
		const char* const name = "Stamina.Lib";

		const ModuleVersion version = ModuleVersion(versionAPI, name, Version(0, 1, 0, 0));

		const char* const copyright = "Copyright (C)2005 Stamina";
		const char* const moreCopyright = "Copyright (C)2005 Rafa³ Lindemann";

	} 
}

#endif