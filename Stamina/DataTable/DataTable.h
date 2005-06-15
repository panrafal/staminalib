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
#ifndef __DATATABLE__
#define __DATATABLE__

#define _DTABLE_


#include <vector>
#include <Stdstring.h>
#include <Stamina\CriticalSection.h>

#include "DT.h"

#define DT_SETBLANKSBYTYPE
#define DT_CHECKTYPE


namespace Stamina { namespace DT {



	typedef void* DataEntry;

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

		inline CriticalSection& getCS() {
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

	struct Column {
		enColumnFlag type;
		tColId id;
		DataEntry def; // default
		std::string name;  // tekstowy identyfikator

		Column();

		enColumnType getType() {
			return (enColumnType) (type & ctypeMask);
		}
		enColumnFlag getFlags() {
			return type;
		}

		void setFlag(enColumnFlag flag, bool setting) {
			if (setting)
				type = (enRowFlag) (type | flag);
			else
				type = (enRowFlag) (type & (~flag));
		}
		bool hasFlag(enColumnFlag flag) {
			return (type & flag) != 0;
		}

	};

	extern const Column emptyColumn;

	class ColumnsDesc {  // Opis typow kolumn
	public:
		typedef std::vector<Column> tColumns;
	public:
		ColumnsDesc ();
		operator = (ColumnsDesc & x);

		int setColCount (int count, bool expand = false); // ustawia ilosc kolumn
		tColId setCol (tColId id , enColumnFlag type , DataEntry def=0 , const char * name="");  // ustawia rodzaj danych w kolumnie
		tColId setUniqueCol (const char * name , enColumnFlag type , DataEntry def=0);  // ustawia rodzaj danych w kolumnie o podanej nazwie

		inline int getColCount () const {
			return _cols.size();
		}
		inline int size() const {
			return _cols.size();
		}

		const Column& getColumnByIndex(int index) const;
		const Column& getColumn(tColId id) const {
			return getColumnByIndex(colIndex(id));
		}

		int colIndex (tColId id) const; // zwraca index kolumny

		void clear() {
			_cols.clear();
		}

		void optimize();
		int join(const ColumnsDesc& other , bool overwrite); // ³¹czy dwa deskryptory, zwraca liczbê do³¹czonych

		int getNewUniqueId(void);
		int getNameId(const char * name) const;

		bool isLoader() {
			return _loader;
		}
		void setLoader(bool loader) {
			_loader = loader;
		}

	protected:
		tColumns _cols;
		bool _loader;
		//void * table;
		//int _deftype;
	};




	class DataTable {
	public:
		typedef std::vector <DataRow *> tRows;

	public:
		DataTable ();
		~DataTable ();

		// rows
		tRowId DataTable::getRowId(unsigned int row) const {
			if (isRowId(row)) return row;
			if (row >= this->getRowCount()) return -1;
			return flagId(_rows[row]->id);
		}
		tRowId DataTable::getRowPos(tRowId row) const {
			if (!isRowId(row)) return row;
			row = unflagId(row);
			for (unsigned int i=0; i < _rows.size(); i++)
				if (_rows[i]->id == row) return i;
			return -1;
		}

		inline static bool isRowId(unsigned int val) {
			return ((row)!=0xFFFFFFFF)&&((row) & DT::rowIdMask) != 0;
		}

		/** Adds flag to the row Id */
		inline static tRowId flagId(tRowId row) {
			return (tRowId)((row) | DT::rowIdMask);
		}
		/** Removes flag from the row Id */
		inline static tRowId unflagId(tRowId row) {
			return (tRowId)( row&(~DT::rowIdMask) );
		}

		tRowId addRow(tRowId id = rowNotFound); // Dodaje wiersz , mozna podac ID

		tRowId insertRow(unsigned int row , tRowId id = -1); // wstawia wiersz , mozna podac ID

		bool deleteRow(tRowId row);

		inline unsigned int getRowCount() const {
			return _rows.size();
		}

		void clearRows();

		tRowId getNewRowId() {
	        _lastId++;
			if (_lastId > rowIdMax) _lastId = rowIdMin;
			return _lastId;
		}


		/** Znajduje wiersz spe³niaj¹cy podane kryteria.
		@param startPos - numer wiersza od którego zaczynamy szukanie
		@param argCount - liczba przekazywanych struktur Find*, lub -1 je¿eli ostatnie kryterium jest równe 0
		@param ... - kolejne kryteria jako struktury Find*, najbezpieczniej dodaæ na koñcu kryterium 0

		np. Znajduje pierwszy kontakt sieci NET_GG aktywny w ci¹gu ostatniej minuty.
		dt->findRow(0, -1, &Find::EqInt(CNT_NET, NET_GG), &Find(Find::gt, CNT_ACTIVITY, ValueInt64(_time64(0) - 60000)), 0);
		*/
        tRowId findRow(unsigned int startPos, int argCount, ...) const;

		inline tRowId findRow(unsigned int startPos, Find& f1) const {
			return this->findRow(startPos, 1, &f1);
		}
		inline tRowId findRow(unsigned int startPos, Find& f1, Find& f2) const {
			return this->findRow(startPos, 2, &f1, &f2);
		}
		inline tRowId findRow(unsigned int startPos, Find& f1, Find& f2, Find& f3) const {
			return this->findRow(startPos, 3, &f1, &f2, &f3);
		}


		DataRow& getRow(tRowId row) throw {
			row = this->getRowPos(row);
			if (row == rowNotFound) throw DTException(errNoRow);
			return *this->_rows[row];
		}

		DataEntry get(tRowId row , tColId id) const; // zwraca wartosc w wierszu/kolumnie
		bool set(tRowId row , int tColId , DataEntry val);


		// inne
		inline int getInt(tRowId row , tColId id) const {
			Value v = Value(ctypeInt);
			this->getValue(row, id, &v);
			return v.vInt;
		}
		inline int setInt(tRowId row , tColId id , int val) {
			return this->setValue(row, id, ValueInt(val));
		}
		inline const char * getCh(tRowId row , tColId id) const {
			Value v = Value(ctypeString);
			this->getValue(row, id, &v);
			return v.vChar;
		}
		inline int setCh(tRowId row , tColId id , const char * val) {
			return this->setValue(row, id, ValueStr(val));
		}

		inline TypeBin getbin(tRowId row , tColId id) const {
			Value v = Value(ctypeBin);
			this->getValue(row, id, &v);
			return v.vBin;
		}
		inline int setBin(tRowId row , tColId id , void * val , size_t size) {
			return this->setValue(row, id, ValueBin(val, size));
		}
		inline int setBin(tRowId row , tColId id , const TypeBin& val) {
			return this->setValue(row, id, ValueBin(val));
		}

		inline __int64 get64(tRowId row , tColId id) const {
			Value v = Value(ctypeInt64);
			this->getValue(row, id, &v);
			return v.vInt64;
		}
		inline int set64(tRowId row , tColId id , __int64 val) {
			return this->setValue(row, id, ValueInt64(val));
		}

		inline std::string getStr(tRowId row , tColId id) const {
			Value v = ValueStr(0, -1); // this way we will get string duplicate
			this->getValue(row, id, &v);
			std::string s = v.vChar;
			free(v.vChar);
			return s;
		}
		inline int setStr(tRowId row , tColId id , const std::string& val) {
			return setCh(row, id, val.c_str());
		}

		const ColumnsDesc& getColumns() {
			return this->_cols;
		}

		void mergeColumns(const ColumnsDesc& columns) {
			return this->_cols.join(columns);
		}


		int checkColType(tColId id , int type) {
			return this->_cols.getColumn(id).type == type;
		}

		bool idExists(int id) {
			if (isRowId(id)) 
				return getRowPos(id) != -1;
			else 
				return getRowId(id) != -1;
		}

		void lock(tRowId row);
		void unlock(tRowId row);
		bool canAccess(tRowId row);

/*
  Gdy pobierana jest wartoœæ char to...
    vChar = 0 buffSize = -1  - zwracany jest duplikat
	vChar = 0 buffSize = 0   - b³¹d
	vChar = * buffSize = 0   - zwracana jest aktualna wartoœæ, a w przypadku liczb u¿ywany jest podany wskaŸnik
	vChar = * buffSize = #   - wartoœæ jest kopiowana do *
*/
		bool getValue(tRowId row , tColId col , Value& value);
		
		/** Sets the value using conversion.
		*/
		bool setValue(tRowId row , tColId col , const Value& value);

		inline void setError(enError error) {
			_error = error;
		}
		inline void resetError() {
			_error = 0;
		}
		inline int getError() {
		}

		inline setOldXorKey(const char* key) {
			_cxor_key = key;
		}


	private:
		tRows _rows;
		int _size;
		tRowId _lastId; // ostatni identyfikator wiersza
		ColumnsDesc _cols;
		enError _error;
		//int mode;
		//int notypecheck;
		char * _cxor_key;
		//unsigned int dbID; // Identyfikator bazy
		cCriticalSection _cs; // mechanizm blokuj¹cy
		bool _changed;


	};


	class LockerDT { 
	public:
		inline LockerDT(DataTable* dt, tRowId row):_dt(dt), _row(row) {
			_dt->lock(_row);
		}
		inline LockerDT(DataTable& dt, tRowId row):_dt(&dt), _row(row) {
			_dt->lock(_row);
		}
		inline ~LockerDT() {
			_dt->unlock(_row);
		}
	private:
		DataTable* _dt;
		tRowId _row;
	}



};}; // namespace'y

#endif


