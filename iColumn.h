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
#ifndef __DT_ICOLUMN__
#define __DT_ICOLUMN__

#include "iRow.h"
#include "..\ObjectPtr.h"
#include "..\StringSTL.h"

namespace Stamina { namespace DT {

	class iColumn: public iSharedObject {
	public:

		STAMINA_OBJECT_CLASS_VERSION(DT::iColumn, iSharedObject, Version(1, 0, 0, 0));

		iColumn* cloneColumn() {
			return (iColumn*)this->cloneObject();
		}

		inline enColumnType getType() const {
			return (enColumnType) (_type & ctypeMask);
		}
		inline enColumnFlag getFlags() const {
			return (enColumnFlag) _type;
		}

		inline tColId getId() const {
			return _id;
		}

		inline unsigned int getIndex() const {
			return _index;
		}

		inline const AString& getName() const {
			return _name;
		}

		inline bool isIdUnique() const {
			return (getId() & colIdUniqueFlag) != 0;
		}

		/** Returns true if type occupies only 4 bytes and can be stored directly (without creating objects)
		*/
		inline bool isStaticType() const {
			return this->getType() == ctypeInt;
		}


		bool hasFlag(enColumnFlag flag) const {
			return (_type & flag) != 0;
		}

		virtual void setFlag(enColumnFlag flag, bool setting) {
			flag = (enColumnFlag)(flag & ~ctypeMask);
			if (setting)
				_type = _type | flag;
			else
				_type = (enColumnType) (_type & (~flag));
		}

		bool isUndefined() const {
			return _type == ctypeUnknown;
		}

		// get / set

		/** Resets the value */
		virtual void reset(iRow* row) const = 0;

		virtual int getInt(const iRow* row) const { return 0; }
		virtual __int64 getInt64(const iRow* row) const { return 0; }
		virtual double getDouble(const iRow* row) const { return 0; }
		virtual String getString(const iRow* row, bool copy = true) const { return String(); }
		virtual ByteBuffer getBin(const iRow* row, bool copy = true) const { return ByteBuffer(); }

		virtual bool setInt(iRow* row, int) const { return false; }
		virtual bool setInt64(iRow* row, __int64) const { return false; }
		virtual bool setDouble(iRow* row, double) const { return false; }
		virtual bool setString(iRow* row, const StringRef&) const { return false; }
		virtual bool setBin(iRow* row, const ByteBuffer&) const { return false; }

		virtual bool convertible(enColumnType type, bool from) const {return false;}

	protected:

		virtual cloneMembers(const iObject* a) {
			BaseClass::cloneMembers(a);
			iColumn* b = static_cast<iColumn*>(a);
			this->_type = b->_type;
			this->_id = b->_id;
			this->_name = b->_name;
			this->_index = b->_index;
		}

		enColumnType _type;
		tColId _id;
		AString _name;  // tekstowy identyfikator
        unsigned int _index;

		

	};


	typedef SharedPtr<iColumn> oColumn;

} }

#endif