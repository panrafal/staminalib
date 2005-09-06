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

#include "Column.h"
#include "Value.h"
#include "DataRow.h"

#ifdef __DT_COLUMN__CHEAP
#error Include Column.h or ColumnTypes.h BEFORE DataTable.h to use this header
#endif

namespace Stamina { namespace DT {


	class Column_undefined: public Column {
	public:

		STAMINA_OBJECT_CLASS(DT::Column_undefined, Column);

		Column_undefined() {
			_type = ctypeUnknown;
			this->init(0, colNotFound, cflagNone, "");
		}

		virtual iObject* cloneObject() const {
			return const_cast<Column_undefined*>( this );
		}

		virtual void reset(iRow* row) const {}

	protected:


	};

	template <typename TO, typename FROM> 
	TO convert(FROM v);

	template <typename TO> 
	inline TO convertValue(const oValue& v) {

		S_ASSERT(v.isValid());

		switch (v->getType()) {
			case ctypeInt: return convert<TO, int>( v->castStaticObject<Value_int>()->getInt() );
			case ctypeInt64: return convert<TO, __int64>( v->castStaticObject<Value_int64>()->getInt64() );
			case ctypeDouble: return convert<TO, double>( v->castStaticObject<Value_double>()->getDouble() );
			case ctypeString: return convert<TO, const StringRef&>( v->castStaticObject<Value_string>()->getString() );
			case ctypeBin: return convert<TO, const ByteBuffer&>( v->castStaticObject<Value_bin>()->getBin() );
		}
		return convert<TO>( (int) 0 );
	}


	class ColumnType_int {
	public:
		const static enColumnType type = ctypeInt;
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
		const static enColumnType type = ctypeInt64;
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
		const static enColumnType type = ctypeDouble;
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
		const static enColumnType type = ctypeString;
		typedef String* tData;
		typedef String tType;
		typedef const StringRef& tCRef;
		typedef PassStringRef tConvert;
		typedef const StringRef& tSetTypeData;
		typedef StringRef tGetTypeData;
		typedef Value_string tValue;
	};

	class ColumnType_bin {
	public:
		const static enColumnType type = ctypeBin;
		typedef ByteBuffer* tData;
		typedef ByteBuffer tType;
		typedef const ByteBuffer& tCRef;
		typedef ByteBuffer::PassBuffer tConvert;
		typedef const ByteBuffer& tSetTypeData;
		typedef ByteBuffer::BufferRef tGetTypeData;
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

		//template<typename TYPE, class COLUMN, typename VALUE>
		//friend void setAllocatedData(const COLUMN* col, iRow* row, VALUE val);
        
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
			if (flags & getCopy) {
				ObjLocker l (getRowLocker(row));
				String s = get<ColumnType_string>(row, flags);
				s.makeUnique();
				return PassStringRef(s);
			} else {
				return get<ColumnType_string>(row, flags);
			}
		}
		virtual ByteBuffer getBin(const iRow* row, GetSet flags) const { 
			if (flags & getCopy) {
				ObjLocker l (getRowLocker(row));
				ByteBuffer b = get<ColumnType_bin>(row, flags);
				b.makeUnique();
				return b;
			} else {
				return get<ColumnType_bin>(row, flags);
			}
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
			_type = TYPE::type;
			_default = 0;
		}

		const iLockableObject* getRowLocker(const iRow* row) const {
			if (row == rowDefault) {
				return this;
			} else {
				return row;
			}
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
				tData data;
				if (this->isStaticType()) {
					if (useDefault && row->castStaticObject<const DataRow>()->hasColumnSlot(this->_index) == false) {
						data = _default;
					} else {
						data = (tData) row->castStaticObject<const DataRow>()->getData(this->_index);
					}
				} else {
					data = (tData) row->castStaticObject<const DataRow>()->getData(this->_index);
					if (useDefault && data == 0) {
						data = _default;
					}
				}
				return data;
			}
			throw DTException(errNoRow);
		}

		// ---------------------------

		template <class TO>
		typename TO::tConvert get(const iRow* row, GetFlags flags) const {
			ObjLocker l (getRowLocker(row));
			tGetTypeData val = this->getTypeData(row, true);
			if (this->_getHandler.empty() == false) {
				tValue current (val);
				current.disableRefCount();
				oValue v = &current;
				this->_getHandler(v, this, row, flags | getHandler);
				return convertValue<TO::tConvert>(v);
			}
			return convert<TO::tConvert, tCRef>(val);
		}

		template <typename FROM>
		bool set(iRow* row, typename FROM::tCRef val, SetFlags flags) const {
			ObjLocker l (getRowLocker(row));
			if (_setHandler.empty() == false) {
				FROM::tValue current (val);
				current.disableRefCount();
				oValue v = &current;
				if (_setHandler(v, this, row, flags | setHandler) == success) {
					if (this->_preTrigger.empty() == false) {
						this->_preTrigger(this, row, flags | setHandler);
					}
					this->setTypeData(row, convertValue<tConvert>(v));
					if (this->_postTrigger.empty() == false) {
						this->_postTrigger(this, row, flags | setHandler);
					}
				} else {
					return false;
				}
			}
			if (!this->convertible(FROM::type, true)) return false;
			if (this->_preTrigger.empty() == false) {
				this->_preTrigger(this, row, flags);
			}
			this->setTypeData(row, convert<tConvert, FROM::tCRef>(val));
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



	};



	template<class TYPE> 
	class Column_allocated: public Column_template<TYPE> {
	public:

		virtual void dataDispose(DataEntry data) const {
			if (data) {
				delete reinterpret_cast< Column_template<TYPE>::tData >( data );
			}
		}

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


	// ------------------------------------------

	class Column_int64: public Column_allocated<ColumnType_int64> {
	public:

		STAMINA_OBJECT_CLASS(DT::Column_int64, Column_allocated<ColumnType_int64>);
		STAMINA_OBJECT_CLONEABLE();

		virtual bool convertible(enColumnType type, bool from) const {
			return (type != ctypeBin);
		}



	};


	// ------------------------------------------

	class Column_double: public Column_allocated<ColumnType_double> {
	public:

		STAMINA_OBJECT_CLASS(DT::Column_double, Column_allocated<ColumnType_double>);
		STAMINA_OBJECT_CLONEABLE();

		virtual bool convertible(enColumnType type, bool from) const {
			return (type != ctypeBin);
		}

	};

	// ------------------------------------------

	class Column_string: public Column_allocated<ColumnType_string> {
	public:

		STAMINA_OBJECT_CLASS(DT::Column_string, Column_allocated<ColumnType_string>);
		STAMINA_OBJECT_CLONEABLE();

		virtual bool convertible(enColumnType type, bool from) const {
			return true;
		}

	};


	// ------------------------------------------

	class Column_bin: public Column_allocated<ColumnType_bin> {
	public:

		STAMINA_OBJECT_CLASS(DT::Column_bin, Column_allocated<ColumnType_bin>);
		STAMINA_OBJECT_CLONEABLE();

		virtual bool convertible(enColumnType type, bool from) const {
			return (type == ctypeBin || type == ctypeString);
		}

	};


} }

#endif