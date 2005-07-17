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
#include "UniqueImpl.h"

namespace Stamina { namespace Unique {

	oDomainList domainList;

	iDomainList* instance() {
		if (!domainList) {
			domainList = new DomainList();

			oDomain domains = new Domain(domainDomain, "domain");
			oDomain ranges = new Domain(domainRange, "range");

			domainList->registerDomain(domains);
			domainList->registerDomain(ranges);

			domains->addRange(new Range(rangeMain, iRange::typeBoth, 0, 1, 0x7FFFFFFF, commonUniqueStart), true);

			ranges->addRange(new Range(rangeMain, iRange::typeBoth, 0, 1, 0x7FFFFFFF, commonUniqueStart), true);
		}
		return domainList.get();
	}


	oRange basicRange(tRangeId rangeId, iRange::enType type, unsigned int priority,  unsigned int minimum, unsigned int maximum, unsigned int uniqueStart) {
		return new Range(rangeId, type, priority, minimum, maximum, uniqueStart);
	}

	oDomain domainBasic(tDomainId id, const StringRef& name) {
		return new Domain(id, name);
	}

	oDomainList domainListBasic() {
		return new DomainList();
	}

// RANGE ----------------------------------------------------------------


// DOMAIN --------------------------------------------------------------

	tId Domain::getId(const StringRef& name) const {
		tIdMap::const_iterator it = this->findId(name);
		return it != this->_idMap.end() ? it->second : idNotFound;
	}

	String Domain::getName(tId id) const {
		tIdMap::const_iterator it = this->findId(id);
		return it != this->_idMap.end() ? it->first : "";
	}

	oRange Domain::inRange(tId id, Range::enType check) const {
		oRange found;
		for (tRanges::const_iterator it = _ranges.begin(); it != _ranges.end(); it++) {
			if ((it->second->getType() & check) && (!found || found->getPriority() < it->second->getPriority())
				&& it->second->containsId(id)) 
			{
				found = it->second;
			}
		}
		return found;
	}

	Domain::tIdMap::iterator Domain::findId(tId id) {
		tIdMap::iterator it;
		for (it = _idMap.begin(); it != _idMap.end(); it++) {
			if (it->second == id) break;
		}
		return it;
	}

	Domain::tIdMap::iterator Domain::findId(const StringRef& name) {
		return this->_idMap.find(name);
	}

	Domain::tIdMap::const_iterator Domain::findId(tId id) const {
		tIdMap::const_iterator it;
		for (it = _idMap.begin(); it != _idMap.end(); it++) {
			if (it->second == id) break;
		}
		return it;
	}

	Domain::tIdMap::const_iterator Domain::findId(const StringRef& name) const {
		return this->_idMap.find(name);
	}

	bool Domain::registerId(tId id, const StringRef& name) {
		if (id == idNotFound || this->idExists(id) || this->nameExists(name))
			return false;
		this->_idMap[name] = id;
		return true;
	}

	tId Domain::registerName(const StringRef& name, const oRange& range) {
		tId id = 0;
		if ((id = this->getId(name)) != idNotFound) return id;
		if (!range || !range->canRegisterName()) return idNotFound;
		while ((id=range->createUniqueId()) != idNotFound) {
			if (!this->idExists(id)) {
				this->_idMap[name] = id;
				break;
			}
		}
		return id;
	}

	bool Domain::unregister(const StringRef& name) {
		if (!this->nameExists(name)) return false;
		this->_idMap.erase(name);
		return true;
	}

	bool Domain::addRange(const oRange& range, bool setAsDefault) {
		if (range->getRangeId()==rangeNotFound || this->rangeExists(range)) return false;
		this->_ranges[range->getRangeId()] = range;
		if (setAsDefault && range->getType() != Range::typeStatic)
			this->_defaultRange = range;
		return true;
	}

	oRange Domain::getRange(tRangeId rangeId) {
		if (rangeId == rangeDefault)
			return this->_defaultRange;
		tRanges::iterator it = this->_ranges.find(rangeId);
		return it == this->_ranges.end() ? oRange() : it->second;
	}

	bool Domain::removeRange(const oRange& range) {
		if (!this->rangeExists(range)) 
			return false;
		this->_ranges.erase(range->getRangeId());
		return true;
	}




};};