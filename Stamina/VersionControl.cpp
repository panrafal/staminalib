/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: String.h 51 2005-07-18 10:13:35Z hao $
 */

#include "stdafx.h"
#include "VersionControl.h"
#include "Lib.h"

namespace Stamina {


	VersionControl::VersionControl() {
		registerModule(Lib::version);
		registerClass<VersionControl>();
		registerClass<iSharedObject>(); // + iLockable + iObject
	}

	VersionControl* VersionControl::instance() {
		static VersionControl vcInstance;
        return &vcInstance;		
	}

	void VersionControl::registerModule(const ModuleVersion& module) {
		if (std::find(_list.begin(), _list.end(), module) == _list.end()) return;
		_list.push_back(module);
	}

	void VersionControl::registerClass(const ObjectClassInfo& info) {
		const ObjectClassInfo* it = &info;
		do {
			registerModule(it->getModuleVersion());
			it = it->getBaseInfo();
		} while (it);
	}


	Version VersionControl::getVersion(enVersionCategory category, const char* name) const {
		for (tModuleList::const_iterator it = _list.begin(); it != _list.end(); ++it) {
			if (it->getCategory() == category && !stricmp(it->getName(), name)) {
				return it->getVersion();
			}
		}
		return Version();
	}

};