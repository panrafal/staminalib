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

		static const ObjectClassInfo none;

		ObjectClassInfo(const char* name, short size, const ObjectClassInfo& base, const Version& version = Version()):_name(name),_size(size),_base(base),_version(version) {
		}
		inline const char * getName() const {
			return _name;
		}
		inline short getSize() const {
			return _size;
		}
		inline const ObjectClassInfo& getBaseInfo() const {
			return _base;
		}
		//unsigned int getUID();

		inline const Version& getVersion() const {
			return _version;
		}

		inline ModuleVersion getModuleVersion() const {
			return ModuleVersion(versionClass, _name, _version);
		}

		inline bool isValid() const {
			return _size != -1;
		}

		inline bool operator == (const ObjectClassInfo& b) const  {
			if (this->_name == b._name) return true;
			return strcmp(this->getName(), b.getName()) == 0;
		}

		bool inheritsFrom (const ObjectClassInfo& b) const  {
			if (*this == b) {
				return true;
			} else if (this->getBaseInfo().isValid() == false) {
				return false;
			} else {
				return this->getBaseInfo() >= b;
			}
		}


		/** Returns true if this object inherits from @a b */
		bool operator >= (const ObjectClassInfo& b) const  {
			return this->inheritsFrom(b);
		}

		inline bool isBaseOf (const ObjectClassInfo& b) const {
			return b >= *this;
		}

		/** Returns true if this object is a base class for @a b */
		inline bool operator <= (const ObjectClassInfo& b) const {
			return this->isBaseOf(b);
		}

		/** Looks for @a b class information */
		const ObjectClassInfo& getParentInfo (const ObjectClassInfo& b) const {
			if (this->isValid() == false || *this == b) {
				return *this;
			} else {
				return this->getBaseInfo().getParentInfo(b);
			}
		}

		template <class TYPE> const ObjectClassInfo& getParentInfo () const {
			return getParentInfo(TYPE::staticClassInfo());
		}

	private:
//		unsigned int _uid;
		const char * const _name;
		const short _size;
		const ObjectClassInfo& _base;
		const Version _version;
	};

	__declspec(selectany)
	const ObjectClassInfo ObjectClassInfo::none("", -1, ObjectClassInfo::none);

#define STAMINA_OBJECT_CLASS_DEFINE_STATICPART(TYPE, NAME, BASE, VERSION) \
	typedef TYPE ObjectClass;\
	typedef BASE BaseClass;\
	static const ::Stamina::ObjectClassInfo& staticClassInfo() {\
		static const ::Stamina::ObjectClassInfo oci = ::Stamina::ObjectClassInfo(NAME, sizeof(TYPE), BASE::staticClassInfo(), VERSION);\
		return oci;\
	}

#define STAMINA_OBJECT_CLASS_DEFINE_DYNAMICPART \
	const ::Stamina::ObjectClassInfo& getClass() const {\
		return staticClassInfo();\
	}

#define STAMINA_OBJECT_CLASS_DEFINE(TYPE, NAME, BASE, VERSION) STAMINA_OBJECT_CLASS_DEFINE_STATICPART(TYPE, NAME, BASE, VERSION) STAMINA_OBJECT_CLASS_DEFINE_DYNAMICPART

#define STAMINA_OBJECT_CLASS(TYPE, BASE) STAMINA_OBJECT_CLASS_DEFINE(TYPE, #TYPE, BASE, ::Stamina::Version())
	
#define STAMINA_OBJECT_CLASS_VERSION(TYPE, BASE, VERSION) STAMINA_OBJECT_CLASS_DEFINE(TYPE, #TYPE, BASE, VERSION)

	template<typename TYPE> inline const ObjectClassInfo& getTypeClassInfo() {
		__if_exists( TYPE::staticClassInfo ) {
			return TYPE::staticClassInfo();
		} 
		__if_not_exists ( TYPE::staticClassInfo ) {
			return ObjectClassInfo::none;
		}
	}



};