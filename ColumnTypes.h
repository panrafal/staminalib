/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: iColumn.h 93 2005-08-22 00:23:52Z hao $
 */
#pragma once
#ifndef __DT_COLUMNTYPES__
#define __DT_COLUMNTYPES__

#include <boost/function.hpp>
#include <boost/signal.hpp>

#include "Column.h"
#include "Value.h"
#include "DataRow.h"

namespace Stamina { namespace DT {


	class Column_unknown: public Column {
	public:

		STAMINA_OBJECT_CLASS(DT::Column_unknown, Column);
		STAMINA_OBJECT_CLONEABLE();

		virtual void reset(iRow* row) const {}

	protected:


	};

	template <typename TO, typename FROM> 
	TO convert(FROM v);

	class ColumnType_int {
	public:
		const static int type = ctypeInt;
		typedef int tData;
		typedef int tType;
		typedef int tCRef;
		typedef int tConvert;
		typedef int tSetTypeData;
		typedef int tGetTypeData;
		typedef Value_int tValue;
	};

	class ColumnType_int64 {
	public:
		const static int type = ctypeInt64;
		typedef __int64* tData;
		typedef __int64 tType;
		typedef __int64 tCRef;
		typedef __int64 tConvert;
		typedef __int64 tSetTypeData;
		typedef __int64 tGetTypeData;
		typedef Value_int64 tValue;
	};

	class ColumnType_double {
	public:
		const static int type = ctypeDouble;
		typedef double* tData;
		typedef double tType;
		typedef double tCRef;
		typedef double tConvert;
		typedef double tSetTypeData;
		typedef double tGetTypeData;
		typedef Value_double tValue;
	};


	class ColumnType_string {
	public:
		const static int type = ctypeString;
		typedef String* tData;
		typedef String tType;
		typedef const StringRef& tCRef;
		typedef PassStringRef tConvert;
		typedef const StringRef& tSetTypeData;
		typedef PassStringRef tGetTypeData;
		typedef Value_string tValue;
	};

	class ColumnType_bin {
	public:
		const static int type = ctypeBin;
		typedef ByteBuffer* tData;
		typedef ByteBuffer tType;
		typedef const ByteBuffer& tCRef;
		typedef ByteBuffer tConvert;
		typedef const ByteBuffer& tSetTypeData;
		typedef ByteBuffer tGetTypeData;
		typedef Value_bin tValue;
	};

	template<class TYPE>
	class Column_template: public Column, public TYPE {
	public:

		typedef typename TYPE::tData tData;
		typedef typename TYPE::tType tType;
		typedef typename TYPE::tConvert tConvert;
		typedef typename TYPE::tSetTypeData tSetTypeData;
		typedef typename TYPE::tGetTypeData tGetTypeData;
		typedef typename TYPE::tValue tValue;


		virtual void reset(iRow* row) const {
			DataEntry data = (DataEntry)this->getData(row, false);
			if (data) {
				this->dataDispose(data);
			}
			this->setData(row, 0);
		}


		virtual int getInt(const iRow* row, GetSet flags) const {
			return get<ColumnType_int>(row, flags);
		}
		virtual __int64 getInt64(const iRow* row, GetSet flags) const { 
			return get<ColumnType_int64>(row, flags);
		}
		virtual double getDouble(const iRow* row, GetSet flags) const { 
			return get<ColumnType_double>(row, flags);
		}
		virtual String getString(const iRow* row, GetSet flags) const {
			ObjLocker l (row);
			String s = get<ColumnType_string>(row, flags);
			if (flags & getCopy) {
				s.makeUnique();
			}
			return PassStringRef(s);
		}
		virtual ByteBuffer getBin(const iRow* row, GetSet flags) const { 
			ObjLocker l (row);
			ByteBuffer b = get<ColumnType_bin>(row, flags);
			if (flags & getCopy) {
				b.makeUnique();
			}
			return b;
		}


		virtual bool setInt(iRow* row, int val, GetSet flags) const { 
			return set<ColumnType_int>(row, val, flags);
		}
		virtual bool setInt64(iRow* row, __int64 val, GetSet flags) const { 
			return set<ColumnType_int64>(row, val, flags);
		}
		virtual bool setDouble(iRow* row, double val, GetSet flags) const { 
			return set<ColumnType_double>(row, val, flags);
		}
		virtual bool setString(iRow* row, const StringRef& val, GetSet flags) const { 
			return set<ColumnType_string>(row, val, flags);
		}
		virtual bool setBin(iRow* row, const ByteBuffer& val, GetSet flags) const { 
			return set<ColumnType_bin>(row, val, flags);
		}



	protected:

		Column_template() {
			_default = 0;
		}

		void setData(iRow* row, tData val) const {
			if (row == rowDefault) {
				const_cast<Column_template*>(this)->_default = val;
			} else if (row != 0) {
				row->castStaticObject<DataRow>()->setData(this->_index, (DataEntry) val);
			} else {
				throw DTException(errNoRow);
			}
		}

		tData getData(const iRow* row, bool useDefault) const {
			if (row == rowDefault) {
				return _default;
			} else if (row != 0) {
				tData data = (tData) row->castStaticObject<const DataRow>()->getData(this->_index);
				if (useDefault && this->isStaticType() == false && data == 0) {
					data = _default;
				}
				return data;
			}
			throw DTException(errNoRow);
		}

		// ---------------------------

		template <class TO>
		typename TO::tConvert get(const iRow* row, GetFlags flags) const {
			ObjLocker l (row);
			tType val = this->getTypeData(row, true);
			if (this->_getHandler.empty() == false) {
				tValue current (val);
				current.disableRefCount();
				oValue v = &current;
				this->_getHandler(v, this, row, flags | getHandler);
				return convert<TO::tConvert>(v);
			}
			return convert<TO::tConvert>(val);
		}

		template <typename FROM>
		bool set(iRow* row, typename FROM::tCRef val, SetFlags flags) const {
			ObjLocker l (row);
			if (_setHandler.empty() == false) {
				FROM::tValue current (val);
				current.disableRefCount();
				oValue v = &current;
				if (_setHandler(v, this, row, flags | setHandler) == success) {
					if (this->_preTrigger.empty() == false) {
						this->_preTrigger(this, row, flags | setHandler);
					}
					this->setTypeData(row, convert<tConvert>(v));
					if (this->_postTrigger.empty() == false) {
						this->_postTrigger(this, row, flags | setHandler);
					}
				} else {
					return false;
				}
			}
			if (this->_preTrigger.empty() == false) {
				this->_preTrigger(this, row, flags);
			}
			this->setTypeData(row, convert<tConvert>(val));
			if (this->_postTrigger.empty() == false) {
				this->_postTrigger(this, row, flags);
			}
			return true;
		}

		void setTypeData(iRow* row, tSetTypeData val) const;
		tGetTypeData getTypeData(const iRow* row, bool useDefault) const;


		// ----------------------------------------------------------


		virtual void cloneMembers(const iObject* a) {
			BaseClass::cloneMembers(a);
			this->setTypeData( rowDefault, a->castStaticObject<const Column_template<TYPE> >()->getTypeData(rowDefault, true) );
		}

        tData _default;

		// enResult getHandler (oValue&, const iColumn*, iRow*, GetFlags)
		// enResult setHandler (oValue&, const iColumn*, iRow*, SetFlags)
		// void trigger(const iColumn*, iRow*, SetFlags)

		boost::function4< enResult, oValue&, const iColumn*, const iRow*, GetFlags > _getHandler;
		boost::function4< enResult, oValue&, const iColumn*, iRow*, SetFlags > _setHandler;
		boost::signal3<void, const iColumn*, iRow*, SetFlags> _preTrigger;
		boost::signal3<void, const iColumn*, iRow*, SetFlags> _postTrigger;


	};






	// ------------------------------------------

	class Column_int: public Column_template<ColumnType_int> {
	public:

		STAMINA_OBJECT_CLASS(DT::Column_int, Column_template<ColumnType_int>);
		STAMINA_OBJECT_CLONEABLE();

		virtual bool convertible(enColumnType type, bool from) const {
			return (type != ctypeBin);
		}

		virtual DataEntry getDefaultStaticData() const {
			return (DataEntry)_default;
		}

	};



} }

#endif