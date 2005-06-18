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


namespace Stamina { namespace DT {

	typedef void* DataEntry;

	class DataTable;

	class DataRow {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
	public:
		friend class FileBin;
	public:
		bool allocData();
		bool freeData();
		const DataEntry get (tColId id); // Pobiera wartosc kolumny
		const DataEntry getByIndex (unsigned int colIndex); // Pobiera wartosc kolumny
		/**Sets the column value (without conversion!)*/
		bool set (tColId id, DataEntry val);
		/**Sets the column value (without conversion and without locking and column lookup)*/
		bool setByIndex (unsigned int colIndex, DataEntry val);


		inline bool hasColumnData(unsigned int colIndex) {
			return (colIndex >= 0 && colIndex < this->_slots);
		}

		DataRow (const DataRow & v);

		DataRow (DataTable* t, char allocate=1);
		~DataRow ();

		inline void lock() {
			_cs.lock();
		}
		inline void unlock() {
			_cs.unlock();
		}
		inline bool canAccess() {
			_cs.canAccess();
		}

		inline Lock& getCS() {
			return _cs;
		}
	
		enRowFlag getFlags() {
			return _flag;
		}
		void setFlag(enRowFlag flag, bool setting) {
			if (setting)
				_flag = (enRowFlag) (_flag | flag);
			else
				_flag = (enRowFlag) (_flag & (~flag));
		}
		bool hasFlag(enRowFlag flag) {
			return (_flag & flag) != 0;
		}

		inline void setId(tRowId id) {
			_id = id;
		}

		inline tRowId getId() {
			return _id;
		}

	private:
		class DataTable * _table; ///< Parent table
		int _size;  ///< Data slots count
		DataEntry * _data; ///< Data slots
		unsigned int _filePos;
		//int _index;
		tRowId _id;    // identyfikator wiersza
		enRowFlag _flag;
		CriticalSection_blank _cs; // blokada
	};


} }

#endif