/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: String.h 51 2005-07-18 10:13:35Z hao $
 */

#pragma once

#include <list>
#include "Object.h"
#include "Version.h"

namespace Stamina {

	class VersionControl: public iObject {
	public:
		typedef std::list<ModuleVersion> tModuleList;

	public:

		STAMINA_OBJECT_CLASS_VERSION(Stamina::VersionControl, iObject, Version(1,0,0,0));

		static VersionControl* instance();

		void registerModule(const ModuleVersion& module);

		void registerClass(const ObjectClassInfo& info);

		template <class CLASS>
		void registerClass() {
			registerClass(CLASS::staticClassInfo());
		}


		Version getVersion(enVersionCategory category, const char* name) const;

		tModuleList::const_iterator begin() const {
			return _list.begin();
		}

		tModuleList::const_iterator end() const {
			return _list.begin();
		}

		unsigned int size() const {
			return _list.size();
		}

	private:
		VersionControl();
	private:
		tModuleList _list;        

	};

	inline void registerVersion(const ModuleVersion& module) {
		VersionControl::instance()->registerModule(module);
	}

	template <class CLASS> inline 
	void registerVersion() {
		VersionControl::instance()->registerModule(CLASS::staticClassInfo().getModuleVersion());
	}

	inline Version getVersion(enVersionCategory category, const char* name) {
		return VersionControl::instance()->getVersion(category, name);
	}


};