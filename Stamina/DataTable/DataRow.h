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
#ifndef __DT_DATAROW__
#define __DT_DATAROW__

#include "iRow.h"
#include <Stamina/ObjectImpl.h>


namespace Stamina { namespace DT {

	class DataTable;


	class DataRow: public SharedObject<iRow> {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
		friend class ColumnsDesc;
		friend class FileBin;
	public:

		STAMINA_OBJECT_CLASS(DT::DataRow, iRow)

		DataRow(DataTable* table) {
			_id = 0;
			_flag = rflagNone;
			_table = table;
		}

		~DataRow() {
			freeData();
		}
/*
		inline void lockRow() {
			_cs.lock();
		}
		inline void unlockRow() {
			_cs.unlock();
		}
		inline bool canAccessRow()  {
			return _cs.canAccess();
		}
*/
		inline Lock& getCS() {
			return CS();
		}
	

		inline void setId(tRowId id) {
			_id = flagId(id);
		}

		const DataEntry getData (unsigned int colIndex) const throw(...); // Pobiera wartosc kolumny
		/**Sets the column value (without conversion and without locking and column lookup)*/
		bool setData (unsigned int colIndex, DataEntry val) throw(...);

		inline bool hasColumnSlot(unsigned int colIndex) const {
			return (colIndex >= 0 && colIndex < _data.size());
		}

		virtual void removeFromTable();


	protected:

		bool freeData();

		virtual void prepareSlot(unsigned int colIndex);

		typedef std::vector<DataEntry> tData;
		tData _data;
		unsigned int _filePos;
		//CriticalSection_blank _cs; // blokada

	};


	typedef oRowT<DataRow> oDataRow;


} }

#endif