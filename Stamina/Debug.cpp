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
			ObjectClassInfo& info = obj->getClass();
			ObjectClassInfo* base = info.getBaseInfo();
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
				, "%s[%d]::%s sz=%d uid=%x lib=%x"
				, info.getName()
				, useCount
				, base ? base->getName() : ""
				, info.getSize()	
				, info.getUID()
				, info.getLibInstance()
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
