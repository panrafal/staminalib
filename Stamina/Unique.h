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

#include <string>
#include <Stamina\Object.h>
#include <Stamina\ObjectPtr.h>
#include <Stamina\String.h>


namespace Stamina { namespace Unique {

	enum enDomainId {
		domainOther,
		domainDomain,
		domainRange,
		domainTable,
		domainIMessage,
		domainMessageType,
		domainAction,
		domainNet,
		domainIcon,
		domainNotFound = -1
	};
	enum enRangeId {
		rangeMain = 0,
		rangeStatic,
		rangeUnique,
		rangeUniqueLow,
		rangeDefault = -1,
		rangeNotFound = -1
	};

	typedef enDomainId tDomainId;
	typedef enRangeId tRangeId;
	typedef unsigned int tId;

	const tId idNotFound = -1;
	const tId commonUniqueStart = 0x00800000;

	class iRange: public iSharedObject {
	public:

		STAMINA_OBJECT_CLASS_VERSION(Stamina::Unique::iRange, iSharedObject, Version(1,0,0,0));

		/** Rodzaj zakresu		
		*/
		enum enType {
			typeBoth = 3,
			// Zakres tylko dla identyfikatorów statycznych
			typeStatic = 1,
			// Zakres tylko dla identyfikatorów unikalnych
			typeUnique = 2
		};

		virtual tId createUniqueId() =0;
		virtual bool canRegisterId(tId id) const =0;
		virtual bool canRegisterName() const =0;
		virtual bool containsId(tId id) const =0;

		virtual tRangeId getRangeId() const =0;
		virtual unsigned int getPriority() const =0;
		virtual enType getType() const =0;
		virtual unsigned int getMinimum() const =0;
		virtual unsigned int getMaximum() const =0;
		virtual unsigned int getNextUnique() const =0;

	protected:
	};

	typedef SharedPtr<iRange> oRange;

	/** Tworzy podstawowy zakres identyfikatorów
	*
	* \param id Unikalny identyfikator zakresu
	* \param priority Priorytet zakresu okreœlaj¹cy jego szczegó³owoœæ. Bardziej ogólne zakresy powinny mieæ ni¿szy priorytet. Zakres domyœlny powinien mieæ priorytet 0.
	* \param type Rodzaj zakresu
	* \param minimum Minimalny identyfikator
	* \param maximum Maxymalny identyfikator
	* \param uniqueStart Pierwszy identyfikator do automatycznego nadawania. -1 ustawia ten parametr na wartoœæ @a minimum.
	* \return 
	*/
	oRange basicRange(tRangeId rangeId, iRange::enType type, unsigned int priority,  unsigned int minimum = 1, unsigned int maximum = 0x7FFFFFFF, unsigned int uniqueStart=-1);


	class iDomain: public iSharedObject {
	public:

		STAMINA_OBJECT_CLASS_VERSION(Stamina::Unique::iDomain, iSharedObject, Version(1,0,0,0));


		virtual tDomainId getDomainId() const =0;
		virtual StringRef getDomainName() const =0;

		virtual tId getId(const StringRef& name) const =0;
		virtual StringRef getName(tId id) const =0;
		virtual oRange inRange(tId id, iRange::enType check = iRange::typeBoth) const =0;
		bool idExists(tId id) const {
			return getName(id).empty() == false;
		}
		bool nameExists(const StringRef& name) const {
			return getId(name) != idNotFound;
		}

		virtual bool registerId(tId id, const StringRef& name) =0;
		virtual tId registerName(const StringRef& name, const oRange& range) =0;
		tId registerName(const StringRef& name, tRangeId range = rangeDefault) {
			return registerName(name, getRange(range));
		}
		virtual bool unregister(tId id) =0;
		virtual bool unregister(const StringRef& name) =0;

		virtual bool addRange(const oRange& range, bool setAsDefault = false) =0;
		bool rangeExists(tRangeId rangeId) {
			return getRange(rangeId) == true;
		}
		bool rangeExists(const oRange& range) {
			return rangeExists(range->getRangeId());
		}
		virtual oRange getRange(tRangeId rangeId) =0;
        virtual bool removeRange(const oRange& range) =0;

		bool idInRange(tRangeId rangeId, tId id) {
			oRange range = getRange(rangeId);
			if (range == false) return false;
			return range->containsId(id);
		}

	};


	typedef SharedPtr<iDomain> oDomain;

	oDomain basicDomain(tDomainId id, const StringRef& name);


	class iDomainList: public iSharedObject {
	public:

		STAMINA_OBJECT_CLASS_VERSION(Stamina::Unique::iDomainList, iSharedObject, Version(1,0,0,0));


		virtual oDomain getDomain(tDomainId id) const =0;
		virtual oDomain getDomain(const StringRef& name) const =0;
		virtual void registerDomain(const oDomain& domain) =0;
		virtual void unregisterDomain(const oDomain& domain) =0;

		bool domainExists(tDomainId id) const {
			return getDomain(id) == true;
		}

	};

	typedef SharedPtr<iDomainList> oDomainList;

	oDomainList basicDomainList();

	iDomainList* instance();
	void setInstance(const oDomainList& list);


	inline tId getId(tDomainId domainId, const StringRef& name) {
		oDomain domain = instance()->getDomain(domainId);
		if (domain == false) return idNotFound;
		return domain->getId(name);
	}
	inline StringRef getName(tDomainId domainId, tId id) {
		oDomain domain = instance()->getDomain(domainId);
		if (domain == false) return "";
		return domain->getName(id);
	}

	inline bool registerId(tDomainId domainId, tId id, const StringRef& name) {
		oDomain domain = instance()->getDomain(domainId);
		if (domain == false) return idNotFound;
		return domain->registerId(id, name);
	}
	inline tId registerName(tDomainId domainId, const StringRef& name, tRangeId range = rangeDefault) {
		oDomain domain = instance()->getDomain(domainId);
		if (domain == false) return idNotFound;
		return domain->registerName(name, range);
	}

	inline oDomain getDomain(tDomainId domainId) {
		return instance()->getDomain(domainId);
	}

	STAMINA_REGISTER_CLASS_VERSION(iRange);
	STAMINA_REGISTER_CLASS_VERSION(iDomain);
	STAMINA_REGISTER_CLASS_VERSION(iDomainList);

};};