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

		inline String& getName() const {
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
			flag &= ~ctypeMask;
			if (setting)
				_type = _type | flag;
			else
				_type = (enColumnType) (_type & (~flag));
		}

		// get / set

		/** Resets the value */
		virtual void reset(iRow& row) const = 0;

		virtual int getInt(iRow& row) const = 0;
		virtual __int64 getInt64(iRow& row) const = 0;
		virtual String getString(iRow& row) const = 0;
		virtual ByteBuffer getBin(iRow& row) const = 0;

		virtual void setInt(iRow& row, int) const = 0;
		virtual void setInt64(iRow& row, __int64) const = 0;
		virtual void setString(iRow& row, String) const = 0;
		virtual void setBin(iRow& row, ByteBuffer) const = 0;

	protected:
		enColumnType _type;
		tColId _id;
		String _name;  // tekstowy identyfikator
        unsigned int _index;


	};


	typedef SharedPtr<iColumn> oColumn;

} }

#endif