/*

The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License from
/LICENSE.HTML in this package or at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is "Stamina.lib" library code, released Feb 1, 2006.

The Initial Developer of the Original Code is "STAMINA" - Rafa³ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa³ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

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
	void setInstance(const oDomainList& list) {
		domainList = list;
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
		ObjLocker lock(this, lockRead);
		tIdMap::const_iterator it = this->findId(name);
		return it != this->_idMap.end() ? it->second : idNotFound;
	}

	StringRef Domain::getName(tId id) const {
		ObjLocker lock(this, lockRead);
		tIdMap::const_iterator it = this->findId(id);
		if (it != this->_idMap.end())
			return it->first;
		else
			return "";
	}

	oRange Domain::inRange(tId id, Range::enType check) const {
		ObjLocker lock(this, lockRead);
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
		ObjLocker lock(this, lockRead);
		tIdMap::iterator it;
		for (it = _idMap.begin(); it != _idMap.end(); it++) {
			if (it->second == id) break;
		}
		return it;
	}

	Domain::tIdMap::iterator Domain::findId(const StringRef& name) {
		ObjLocker lock(this, lockRead);
		return this->_idMap.find(name);
	}

	Domain::tIdMap::const_iterator Domain::findId(tId id) const {
		ObjLocker lock(this, lockRead);
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
		ObjLocker lock(this, lockWrite);
		if (id == idNotFound || this->idExists(id) || this->nameExists(name))
			return false;
		this->_idMap[name] = id;
		return true;
	}

	tId Domain::registerName(const StringRef& name, const oRange& range) {
		ObjLocker lock(this, lockWrite);
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
		ObjLocker lock(this, lockWrite);
		if (!this->nameExists(name)) return false;
		this->_idMap.erase(name);
		return true;
	}

	bool Domain::addRange(const oRange& range, bool setAsDefault) {
		ObjLocker lock(this, lockWrite);
		if (range->getRangeId()==rangeNotFound || this->rangeExists(range)) return false;
		this->_ranges[range->getRangeId()] = range;
		if (setAsDefault && range->getType() != Range::typeStatic)
			this->_defaultRange = range;
		return true;
	}

	oRange Domain::getRange(tRangeId rangeId) {
		ObjLocker lock(this, lockRead);
		if (rangeId == rangeDefault)
			return this->_defaultRange;
		tRanges::iterator it = this->_ranges.find(rangeId);
		return it == this->_ranges.end() ? oRange() : it->second;
	}

	bool Domain::removeRange(const oRange& range) {
		ObjLocker lock(this, lockWrite);
		if (!this->rangeExists(range)) 
			return false;
		this->_ranges.erase(range->getRangeId());
		return true;
	}




};};