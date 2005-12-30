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
#include "String.h"
#include "Version.h"
#include "Lib.h"

namespace Stamina {

	class VersionControl: public iObject {
	public:
		typedef std::list<ModuleVersion> tModuleList;

	public:

		STAMINA_OBJECT_CLASS_VERSION(Stamina::VersionControl, iObject, Version(1,1,0,0));

		VersionControl() {}

		virtual String toString(iStringFormatter* format=0) const {
			return "";
		}

		static __declspec(dllexport) __declspec(noinline) VersionControl* instance() {
			static VersionControl vc;
			return &vc;
		}


		bool registerModule(const ModuleVersion& module) {
			//OutputDebugString("registerModule: ");
			//OutputDebugString(module.getName());

			if (std::find(_list.begin(), _list.end(), module) != _list.end()) {
				//OutputDebugString(" !!!! \n");
				return false;
			}
			//OutputDebugString("\n");
			_list.push_back(module);
			return true;
		}

		void registerModule(const ObjectClassInfo& info) {
			const ObjectClassInfo* it = &info;
			do {
				if (registerModule(it->getModuleVersion()) == false)
					break;
				it = &it->getBaseInfo();
				//break;
			} while (it->isValid());
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
			return _list.end();
		}

		unsigned int size() const {
			return _list.size();
		}

		void clear() {
			return _list.clear();
		}

	private:
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
		VersionControlRegistrar(const ModuleVersion& module):version(module) {
			registerVersion(module);
		}
		VersionControlRegistrar(const ObjectClassInfo& info):version(info.getModuleVersion()) {
			registerVersion(info);
		}
		ModuleVersion version;
	};

#undef STAMINA_REGISTER_VERSION
#define STAMINA_REGISTER_VERSION(NAME, MODULE) \
	extern __declspec(dllexport) __declspec(selectany) const ::Stamina::VersionControlRegistrar __version__##NAME (MODULE);

#undef STAMINA_REGISTER_CLASS_VERSION
#define STAMINA_REGISTER_CLASS_VERSION(CLASS) \
	STAMINA_REGISTER_VERSION(CLASS, CLASS::staticClassInfo())


	STAMINA_REGISTER_CLASS_VERSION(iObject);
	STAMINA_REGISTER_CLASS_VERSION(iLockableObject);
	STAMINA_REGISTER_CLASS_VERSION(iSharedObject); // w object.h nie ma mo¿liwoœci rejestracji...
	STAMINA_REGISTER_CLASS_VERSION(VersionControl);
	STAMINA_REGISTER_VERSION(StaminaLib, Lib::version);
	STAMINA_REGISTER_VERSION(Buffer, Buffer<char>::version);

	STAMINA_REGISTER_CLASS_VERSION(iString);

};



#endif