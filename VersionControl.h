/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#ifndef __STAMINA_VERSIONCONTROL__
#define __STAMINA_VERSIONCONTROL__

#pragma once

#include <list>
#include <algorithm>
#include "Object.h"
#include "Version.h"

namespace Stamina {

	class VersionControl: public iObject {
	public:
		typedef std::list<ModuleVersion> tModuleList;

	public:

		STAMINA_OBJECT_CLASS_VERSION(Stamina::VersionControl, iObject, Version(1,0,0,0));

		static VersionControl* instance() {
			static VersionControl vcInstance;
			return &vcInstance;		
		}

		bool registerModule(const ModuleVersion& module) {
			OutputDebugString("registerModule: ");
			OutputDebugString(module.getName());
			OutputDebugString("\n");

			if (std::find(_list.begin(), _list.end(), module) != _list.end()) return false;
			_list.push_back(module);
			return true;
		}

		void registerModule(const ObjectClassInfo& info) {
			const ObjectClassInfo* it = &info;
			do {
				if (registerModule(it->getModuleVersion()) == false)
					break;
				it = it->getBaseInfo();
			} while (it);
		}

		template <class CLASS>
		void registerClass() {
			registerModule(CLASS::staticClassInfo());
		}


		Version getVersion(enVersionCategory category, const char* name) const {
			for (tModuleList::const_iterator it = _list.begin(); it != _list.end(); ++it) {
				if (it->getCategory() == category && !stricmp(it->getName(), name)) {
					return it->getVersion();
				}
			}
			return Version();
		}

		tModuleList::const_iterator begin() const {
			return _list.begin();
		}

		tModuleList::const_iterator end() const {
			return _list.begin();
		}

		unsigned int size() const {
			return _list.size();
		}

		void clear() {
			return _list.clear();
		}

	private:
		VersionControl() {
		}
	private:
		tModuleList _list;        

	};

	inline void registerVersion(const ModuleVersion& module) {
		VersionControl::instance()->registerModule(module);
	}

	inline void registerVersion(const ObjectClassInfo& info) {
		VersionControl::instance()->registerModule(info);
	}

	template <class CLASS> inline 
	void registerVersion() {
		registerVersion(CLASS::staticClassInfo());
	}

	inline Version getVersion(enVersionCategory category, const char* name) {
		return VersionControl::instance()->getVersion(category, name);
	}

	template <class CLASS> 
	inline Version getVersion() {
		return VersionControl::instance()->getVersion(versionClass, CLASS::staticClassInfo().getName());
	}

	class VersionControlRegistrar {
	public:
		VersionControlRegistrar(const ModuleVersion& module) {
			registerVersion(module);
		}
		VersionControlRegistrar(const ObjectClassInfo& info) {
			registerVersion(info);
		}
		VersionControlRegistrar() {}
	};



#define STAMINA_REGISTER_VERSION(NAME, MODULE) \
	const ::Stamina::VersionControlRegistrar __version__##NAME (MODULE);

#define STAMINA_REGISTER_CLASS_VERSION(CLASS) \
	STAMINA_REGISTER_VERSION(CLASS, CLASS::staticClassInfo())


	STAMINA_REGISTER_CLASS_VERSION(iSharedObject); // + iLockableObject, +iObject - w object.h nie ma mo¿liwoœci rejestracji...

	STAMINA_REGISTER_CLASS_VERSION(VersionControl);

};




#endif