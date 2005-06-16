/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: $
 */
#pragma once
#ifndef __DT_DATAROW__
#define __DT_DATAROW__


namespace Stamina { namespace DT {

	typedef void* DataEntry;

	class DataTable;

	class DataRow {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
	public:
		int allocData();
		int freeData();
		const DataEntry get (tColId id); // Pobiera wartosc kolumny
		int set (tColId id , DataEntry val); // ustawia wartosc kolumny

		const DataEntry getByIndex (int index); // Pobiera wartosc kolumny

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
		//unsigned int _pos;
		//int _index;
		tRowId _id;    // identyfikator wiersza
		enRowFlag _flag;
		CriticalSection_blank _cs; // blokada
	};


} }

#endif