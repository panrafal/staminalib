/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: Object.h 134 2005-12-09 11:02:59Z hao $
 */

#pragma once


namespace Stamina {

	class ObjectClassInfo {
	public:
		ObjectClassInfo(const char* name, short size, ObjectClassInfo* base, const Version& version = Version()):_name(name),_size(size),_base(base),_uid(0),_libInstance(LibInstance::get()), _version(version) {
		}
		inline const char * getName() const {
			return _name;
		}
		inline short getSize() const {
			return _size;
		}
		inline ObjectClassInfo* getBaseInfo() const {
			return _base;
		}
		unsigned int getUID();

		inline LibInstance& getLibInstance() const {
			return _libInstance;
		}

		inline const Version& getVersion() const {
			return _version;
		}

		inline ModuleVersion getModuleVersion() const {
			return ModuleVersion(versionClass, _name, _version);
		}

		inline bool operator == (ObjectClassInfo& b) {
			return this->getUID() == b.getUID();
		}
		/** Returns true if this object inherits from @a b */
		bool operator >= (ObjectClassInfo& b) {
			if (*this == b) {
				return true;
			} else if (this->getBaseInfo() == 0) {
				return false;
			} else {
				return *this->getBaseInfo() >= b;
			}
		}
		/** Returns true if this object is a base class for @a b */
		inline bool operator <= (ObjectClassInfo& b) {
			return b >= *this;
		}

		/** Looks for @a b class information */
		ObjectClassInfo* getParentInfo (ObjectClassInfo& b) {
			if (*this == b) {
				return this;
			} else if (this->getBaseInfo() == 0) {
				return 0;
			} else {
				return this->getBaseInfo()->getParentInfo(b);
			}
		}

		template <class TYPE> ObjectClassInfo* getParentInfo () {
			return getParentInfo(TYPE::staticClassInfo());
		}

	private:
		unsigned int _uid;
		const char * const _name;
		const short _size;
		ObjectClassInfo* const _base;
		LibInstance& _libInstance;
		const Version _version;
	};



#define STAMINA_OBJECT_CLASS_DEFINE_STATICPART(TYPE, NAME, BASE, VERSION) \
	typedef TYPE ObjectClass;\
	typedef BASE BaseClass;\
	static ::Stamina::ObjectClassInfo& staticClassInfo() {\
		static ::Stamina::ObjectClassInfo oci = ::Stamina::ObjectClassInfo(NAME, sizeof(TYPE), &BASE::staticClassInfo(), VERSION);\
			return oci;\
	}

#define STAMINA_OBJECT_CLASS_DEFINE_DYNAMICPART \
	::Stamina::ObjectClassInfo& getClass() const {\
		return staticClassInfo();\
	}

#define STAMINA_OBJECT_CLASS_DEFINE(TYPE, NAME, BASE, VERSION) STAMINA_OBJECT_CLASS_DEFINE_STATICPART(TYPE, NAME, BASE, VERSION) STAMINA_OBJECT_CLASS_DEFINE_DYNAMICPART

#define STAMINA_OBJECT_CLASS(TYPE, BASE) STAMINA_OBJECT_CLASS_DEFINE(TYPE, #TYPE, BASE, ::Stamina::Version())
	
#define STAMINA_OBJECT_CLASS_VERSION(TYPE, BASE, VERSION) STAMINA_OBJECT_CLASS_DEFINE(TYPE, #TYPE, BASE, VERSION)

	template<typename TYPE> inline ObjectClassInfo* getTypeClassInfo() {
		__if_exists( TYPE::staticClassInfo ) {
			return &TYPE::staticClassInfo();
		} 
		__if_not_exists ( TYPE::staticClassInfo ) {
			return 0;
		}
	}



};