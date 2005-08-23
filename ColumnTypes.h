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

namespace Stamina { namespace DT {

	class Column_base: public Column {
	public:

		STAMINA_OBJECT_CLASS_VERSION(DT::Column_base, Column);

	protected:

		void setData(iRow* row, DataEntry val) const {
			if (row == rowDefault) {
				const_cast<Column_base*>(this)->_default = val;
			} else if (row != 0) {
				row->castStaticObject<DataRow>()->setData(this->_index, val);
			} else {
				throw DTException(DT::enError::errNoRow);
			}
		}

		void DataEntry getData(const iRow* row, bool useDefault) const {
			if (row == rowDefault) {
				return _default;
			} else if (row != 0) {
				DataEntry data = row->castStaticObject<DataRow>()->getData(this->_index);
				if (useDefault && this->isStaticType() == false && data == 0) {
					data = _default;
				}
				return data;
			} else {
				throw DTException(DT::enError::errNoRow);
			}
		}

		virtual cloneMembers(const iObject* a) {
			BaseClass::cloneMembers(a);
		}

        DataEntry _default;

	};



	class Column_unknown: public Column {
	public:

		STAMINA_OBJECT_CLASS_VERSION(DT::Column_unknown, Column);
		STAMINA_OBJECT_CLONEABLE();

		virtual void reset(iRow* row) const {}

	protected:


	};



	// ------------------------------------------

	class Column_int: public Column_base {
	public:

		STAMINA_OBJECT_CLASS_VERSION(DT::Column_int, Column_base);
		STAMINA_OBJECT_CLONEABLE();

		virtual void reset(iRow* row) const {
			this->setData(row, 0);
		}

		virtual int getInt(const iRow* row) const { 
			return (int)this->getData(row, false); 
		}
		virtual __int64 getInt64(const iRow* row) const { 
			return (int)this->getData(row, false); 
		}
		virtual double getDouble(const iRow* row) const { 
			return (int)this->getData(row, false); 
		}
		virtual String getString(const iRow* row, bool copy = true) const { 
			return inttostr( (int)this->getData(row, false) ); 
		}

		virtual bool setInt(iRow* row, int) const { return false; }
		virtual bool setInt64(iRow* row, __int64) const { return false; }
		virtual bool setDouble(iRow* row, double) const { return false; }
		virtual bool setString(iRow* row, const StringRef&) const { return false; }

		virtual bool convertible(enColumnType type, bool from) const {
			return (type != ctypeBin);
		}

		virtual DataEntry getDefaultStaticData() const {
			return _default;
		}
		virtual void dataDispose(DataEntry data) const {
		}

	protected:

		virtual cloneMembers(const iObject* a) {
			BaseClass::cloneMembers(a);
			this->_default = a->castStaticObject<Column_int>()->_default;
		}

	};



} }

#endif