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

#include "Debug.h"
#include "Object.h"
#include "Logger.h"




namespace Stamina {

	void debugDumpObjects(const oLogger logger, LogLevel level) {
		S_ASSERT(logger);
#ifdef STAMINA_DEBUG
		if (!logger || !debugObjects || !debugObjectsCS) return;
		Locker locker ( *debugObjectsCS );
		logger->log(level, "Stamina", "debugDumpObjects", "Debug: Objects list");
		for (std::list<iObject*>::iterator it = debugObjects->begin(); it != debugObjects->end(); it++) {
			iObject* obj = *it;
			ObjectClassInfo& info = obj->getClass();
			ObjectClassInfo* base = info.getBaseInfo();
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

#else
		logger->log(logError, "Stamina", "debugDumpObjects", "Debug mode is turned off!");
#endif
	}

}
