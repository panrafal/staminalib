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

#include <map>
#include <Stamina\ObjectImpl.h>
#include <Stamina\String.h>

#include "Unique.h"

namespace Stamina { namespace Unique {

	class eIdNotFound {
	};

	class Range:public SharedObject<iRange> {
	public:

		STAMINA_OBJECT_CLASS(Unique::Range, iRange);


		Range(tRangeId rangeId, enType type, unsigned int priority,  unsigned int minimum = 1, unsigned int maximum = 0x7FFFFFFF, unsigned int uniqueStart=-1)
			:_rangeId(rangeId), _priority(priority), _type(type), _minimum(minimum), _maximum(maximum)
		{
			this->_nextUnique = (uniqueStart == -1)? minimum : uniqueStart;
		}


		virtual tId createUniqueId() {
			return this->_nextUnique++;
		}
		virtual bool canRegisterId(tId id) const {
			if (this->_type == typeUnique)
				return false;
			if (!this->containsId(id))
				return false;
			if (this->_type == typeBoth && this->_nextUnique <= id)
				return false;
			return true;
		}
		virtual bool canRegisterName() const {
			return (this->_type != typeStatic);
		}
		virtual bool containsId(tId id) const {
			return id <= this->_maximum && id >= this->_minimum;
		}

		virtual tRangeId getRangeId() const {
			return _rangeId;
		}
		virtual unsigned int getPriority() const {
			return _priority;
		}
		virtual enType getType() const {
			return _type;
		}
		virtual unsigned int getMinimum() const {
			return _minimum;
		}
		virtual unsigned int getMaximum() const {
			return _maximum;
		}
		virtual unsigned int getNextUnique() const {
			return _nextUnique;
		}

	protected:
		tRangeId _rangeId;
		unsigned int _priority;
		enType _type;
		unsigned int _minimum;
		unsigned int _maximum;
		unsigned int _nextUnique;

	};


	class Domain : public SharedObject<iDomain> {
	public:
		typedef std::map<String, tId> tIdMap;
		typedef std::map<tRangeId, oRange> tRanges;
	public:

		STAMINA_OBJECT_CLASS(Unique::Domain, iDomain);

		Domain(tDomainId domainId, const StringRef& domainName):_domainId(domainId),_domainName(domainName) {}

		virtual tDomainId getDomainId() const {return _domainId;}
		virtual StringRef getDomainName() const {return _domainName;}

		virtual tId getId(const StringRef& name) const;
		virtual StringRef getName(tId id) const;
		virtual oRange inRange(tId id, Range::enType check = Range::typeBoth) const;
		tIdMap::iterator findId(tId id);
		tIdMap::iterator findId(const StringRef& name);
		tIdMap::const_iterator findId(tId id) const;
		tIdMap::const_iterator findId(const StringRef& name) const;

		virtual bool registerId(tId id, const StringRef& name);
		virtual tId registerName(const StringRef& name, const oRange& range);
		tId registerName(const StringRef& name, tRangeId range) {
			return registerName(name, getRange(range));
		}

		virtual bool unregister(tId id) {
			return unregister(getName(id));
		}
		virtual bool unregister(const StringRef& name);

		tRanges & getRanges() {return _ranges;}
		virtual bool addRange(const oRange& range, bool setAsDefault = false);
		virtual oRange getRange(tRangeId rangeId);
        virtual bool removeRange(const oRange& range);

	private:
		tDomainId _domainId;
		String _domainName;
		//RangePtr _defaultStaticRange;
		oRange _defaultRange;
		tIdMap _idMap;
		tRanges _ranges;
	};


	class DomainList : public SharedObject<iDomainList> {
	public:
		typedef std::map<tDomainId, oDomain> tMap;
	public:

		STAMINA_OBJECT_CLASS(Unique::DomainList, iDomainList);

		virtual oDomain getDomain(tDomainId id) const {
			ObjLocker lock(this, lockRead);
			tMap::const_iterator found = _map.find(id);
			if (found == _map.end()) return oDomain();
			return found->second;
		}

		virtual oDomain getDomain(const StringRef& name) const {
			ObjLocker lock(this, lockRead);
			for (tMap::const_iterator it = _map.begin(); it != _map.end(); ++it) {
				if (it->second->getDomainName() == name) return it->second;
			}
			return oDomain();
		}

		virtual void registerDomain(const oDomain& domain) {
			ObjLocker lock(this, lockWrite);
			if (domain->getDomainId() == domainNotFound) return;
			if (domainExists(domain->getDomainId())) return;
			_map[domain->getDomainId()] = domain;
		}
		virtual void unregisterDomain(const oDomain& domain) {
			ObjLocker lock(this, lockWrite);
			_map.erase(domain->getDomainId());
		}

	private:

		tMap _map;

	};


};};