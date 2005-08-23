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

#include "iRow.h"
#include "..\ObjectPtr.h"

namespace Stamina { namespace DT {

	class DataTable;

	class iRow: public iSharedObject {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
	public:

		STAMINA_OBJECT_CLASS_VERSION(DT::iRow, iSharedObject, Version(1, 0, 0, 0));

		virtual void lock() = 0;
		virtual void unlock() = 0;

		virtual bool canAccess() = 0;

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

		DataTable* getOwner() {
			return _table;
		}


	protected:
		enRowFlag _flag;
		DataTable * _table; ///< Parent table
		tRowId _id;    // identyfikator wiersza

	};

	typedef SharedPtr<iRow> oRow;

	const iRow* rowDefault = (iRow*)-1;


} }

#endif