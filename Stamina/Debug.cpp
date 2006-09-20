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

#include <map>

#include "Debug.h"
#include "Object.h"
#include "Logger.h"




namespace Stamina {

	void debugDumpObjects(const oLogger logger, LogLevel level) {
		S_ASSERT(logger.isValid());
#ifdef STAMINA_DEBUG
		if (!logger || !debugObjects || !debugObjectsCS) return;
		Locker locker ( *debugObjectsCS );

		std::map <std::string, int> counter;

		logger->log(level, "Stamina", "debugDumpObjects", "Debug: Objects list");
		for (std::list<iObject*>::iterator it = debugObjects->begin(); it != debugObjects->end(); it++) {
			iObject* obj = *it;
			const ObjectClassInfo& info = obj->getClass();
			const ObjectClassInfo& base = info.getBaseInfo();
			if (counter.find(info.getName()) == counter.end()) {
				counter[info.getName()] = 1;
			} else {
				counter[info.getName()] += 1;
			}
			int useCount = -1;
			if (info >= iSharedObject::staticClassInfo()) {
				useCount = obj->castObject<iSharedObject>()->getUseCount();
			}
			logger->log(level, "Stamina", "debugDumpObjects"
				, "%s[%d]::%s sz=%d"
				, info.getName()
				, useCount
				, base.getName()
				, info.getSize()	
				);
		}
		logger->log(level, "Stamina", "debugDumpObjects", "--------------------");
		logger->log(level, "Stamina", "debugDumpObjects", "Objects sum up:");

		for (std::map <std::string, int>::iterator it = counter.begin(); it != counter.end(); ++it) {
			logger->log(level, "Stamina", "debugDumpObjects"
				, "%s = %d"
				, it->first.c_str()
				, it->second
				);

		}

		logger->log(level, "Stamina", "debugDumpObjects", "-----");

		logger->log(level, "Stamina", "debugDumpObjects"
			, "ObjectsCount = %d", debugObjectsCount);

		logger->log(level, "Stamina", "debugDumpObjects", "--------------------");
		logger->log(level, "Stamina", "debugDumpObjects", "--------------------");

#else
		logger->log(logError, "Stamina", "debugDumpObjects", "Debug mode is turned off!");
#endif
	}

}
