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
#ifndef __DT_IROW__
#define __DT_IROW__

#include "DT.h"
#include "..\ObjectPtr.h"
#include "..\Lock.h"

namespace Stamina { namespace DT {

	class DataTable;

	class iRow: public iSharedObject {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
	public:

		STAMINA_OBJECT_CLASS_VERSION(DT::iRow, iSharedObject, Version(1, 0, 0, 0));
/*
		virtual void lockRow() = 0;
		virtual void unlockRow() = 0;

		virtual bool canAccessRow() = 0;
*/
		virtual Lock& getCS() = 0;
	
		enRowFlag getFlags() const {
			return _flag;
		}
		void setFlag(enRowFlag flag, bool setting) {
			if (setting)
				_flag = (enRowFlag) (_flag | flag);
			else
				_flag = (enRowFlag) (_flag & (~flag));
		}
		bool hasFlag(enRowFlag flag) const {
			return (_flag & flag) != 0;
		}

		inline tRowId getId() const {
			return _id;
		}

		inline static tRowId flagId(tRowId row) {
			return (tRowId)((row) | DT::rowIdFlag);
		}
		/** Removes flag from the row Id */
		inline static tRowId unflagId(tRowId row) {
			return (tRowId)( row&(~DT::rowIdFlag) );
		}

		virtual void removeFromTable()=0;

		DataTable* getOwner() const {
			return _table;
		}


	protected:
		enRowFlag _flag;
		DataTable * _table; ///< Parent table
		tRowId _id;    // identyfikator wiersza

	};

	template <class TYPE>
	class oRowT: public SharedPtr<TYPE> {
	public:
		oRowT(TYPE * obj = 0):SharedPtr<TYPE>() {
			this->set(obj);
		}
		oRowT(TYPE & obj):SharedPtr<TYPE>() {
			this->set(obj);
		}
		oRowT(const oRowT & b):SharedPtr<TYPE>() {
			this->set(&*b);
		}
		oRowT& operator = (const oRowT & b) {
			this->set(&*b);
			return *this;
		}

		operator tRowId() const {
			return getId();
		}

		operator int() const {
			return getId();
		}

		tRowId getId() const {
			if (this->get()) {
				return this->get()->getId();
			} else {
				return rowNotFound;
			}
		}
		
		bool isFound() const {
			return getId() != rowNotFound;
		}

	};

	typedef oRowT<iRow> oRow;

	iRow* const rowDefault = (iRow*)-1;


} }

#endif