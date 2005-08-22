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

namespace Stamina { namespace DT {

	class DataTable;


	class DefaultRow: public SharedObject<iRow> {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
	public:

		DefaultRow(DataTable* t) {
			_id = rowIdDefault;
			_flag = rflagNone;
			_size = 0;
			_table = table;
		}

		~DefaultRow() {
			freeData();
		}

		inline void lock() {
		}
		inline void unlock() {
		}
		inline bool canAccess()  {
			return true;
		}
		inline Lock& getCS() {
			static Lock_blank blank;
			return blank;
		}

		const DataEntry getData (unsigned int colIndex) throw(...); // Pobiera wartosc kolumny
		/**Sets the column value (without conversion and without locking and column lookup)*/
		bool setData (unsigned int colIndex, DataEntry val) throw(...);

		inline bool hasColumnSlot(unsigned int colIndex) {
			return (colIndex >= 0 && colIndex < _data->size());
		}

	protected:

		bool freeData();

		virtual void prepareSlot(unsigned int colIndex);

		typedef std::vector<DataEntry> tData;
		tData _data;

	};

	class DataRow: public SharedObject<iRow> {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
	public:
		friend class FileBin;
	public:

		STAMINA_OBJECT_CLASS(DT::DataRow, iRow);

		DataRow (DataTable* t):DefaultRow(t) {
			_id = rowIdDefault;
		}

		inline void lock() {
			_cs.lock();
		}
		inline void unlock() {
			_cs.unlock();
		}
		inline bool canAccess()  {
			return _cs.canAccess();
		}

		inline Lock& getCS() {
			return _cs;
		}
	

		inline void setId(tRowId id) {
			_id = flagId(id);
		}

	protected:

		virtual void prepareSlot(unsigned int colIndex);


	protected:

		unsigned int _filePos;
		CriticalSection_blank _cs; // blokada
	};


} }

#endif