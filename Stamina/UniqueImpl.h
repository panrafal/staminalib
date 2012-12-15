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
#include "ObjectImpl.h"
#include "String.h"

#include "Unique.h"

namespace Stamina { namespace Unique {

	class eIdNotFound {
	};

	class Range:public SharedObject<iRange> {
	public:

		STAMINA_OBJECT_CLASS(Stamina::Unique::Range, iRange);


		Range(tRangeId rangeId, enType type, unsigned int priority,  unsigned int minimum = 1, unsigned int maximum = 0x7FFFFFFF, unsigned int uniqueStart=-1)
			:_rangeId(rangeId), _priority(priority), _type(type), _minimum(minimum), _maximum(maximum)
		{
			this->_nextUnique = (uniqueStart == -1)? minimum : uniqueStart;
		}


		virtual tId __stdcall createUniqueId() {
			return this->_nextUnique++;
		}
		virtual bool __stdcall canRegisterId(tId id) const {
			if (this->_type == typeUnique)
				return false;
			if (!this->containsId(id))
				return false;
			if (this->_type == typeBoth && this->_nextUnique <= id)
				return false;
			return true;
		}
		virtual bool __stdcall canRegisterName() const {
			return (this->_type != typeStatic);
		}
		virtual bool __stdcall containsId(tId id) const {
			return id <= this->_maximum && id >= this->_minimum;
		}

		virtual tRangeId __stdcall getRangeId() const {
			return _rangeId;
		}
		virtual unsigned int __stdcall getPriority() const {
			return _priority;
		}
		virtual enType __stdcall getType() const {
			return _type;
		}
		virtual unsigned int __stdcall getMinimum() const {
			return _minimum;
		}
		virtual unsigned int __stdcall getMaximum() const {
			return _maximum;
		}
		virtual unsigned int __stdcall getNextUnique() const {
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

		STAMINA_OBJECT_CLASS(Stamina::Unique::Domain, iDomain);

		Domain(tDomainId domainId, const StringRef& domainName):_domainId(domainId),_domainName(domainName) {}

		virtual tDomainId __stdcall getDomainId() const {return _domainId;}
		virtual StringRef __stdcall getDomainName() const {return _domainName;}

		virtual tId __stdcall getId(const StringRef& name) const;
		virtual StringRef __stdcall getName(tId id) const;
		virtual oRange __stdcall inRange(tId id, Range::enType check = Range::typeBoth) const;
		tIdMap::iterator findId(tId id);
		tIdMap::iterator findId(const StringRef& name);
		tIdMap::const_iterator findId(tId id) const;
		tIdMap::const_iterator findId(const StringRef& name) const;

		virtual bool __stdcall registerId(tId id, const StringRef& name);
		virtual tId __stdcall registerName(const StringRef& name, const oRange& range);
		tId registerName(const StringRef& name, tRangeId range) {
			return registerName(name, getRange(range));
		}

		virtual bool __stdcall unregister(tId id) {
			return unregister(getName(id));
		}
		virtual bool __stdcall unregister(const StringRef& name);

		tRanges & getRanges() {return _ranges;}
		virtual bool __stdcall addRange(const oRange& range, bool setAsDefault = false);
		virtual oRange __stdcall getRange(tRangeId rangeId);
        virtual bool __stdcall removeRange(const oRange& range);

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

		STAMINA_OBJECT_CLASS(Stamina::Unique::DomainList, iDomainList);

		virtual oDomain __stdcall getDomain(tDomainId id) const {
			ObjLocker lock(this);
			tMap::const_iterator found = _map.find(id);
			if (found == _map.end()) return oDomain();
			return found->second;
		}

		virtual oDomain __stdcall getDomain(const StringRef& name) const {
			ObjLocker lock(this);
			for (tMap::const_iterator it = _map.begin(); it != _map.end(); ++it) {
				if (it->second->getDomainName() == name) return it->second;
			}
			return oDomain();
		}

		virtual void __stdcall registerDomain(const oDomain& domain) {
			ObjLocker lock(this);
			if (domain->getDomainId() == domainNotFound) return;
			if (domainExists(domain->getDomainId())) return;
			_map[domain->getDomainId()] = domain;
		}
		virtual void __stdcall unregisterDomain(const oDomain& domain) {
			ObjLocker lock(this);
			_map.erase(domain->getDomainId());
		}

	private:

		tMap _map;

	};


};};