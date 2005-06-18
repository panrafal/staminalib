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
#include <map>
#include <Stdstring.h>
#include <Stamina\CriticalSection.h>
#include <Stamina\MD5.h>
#include <Stamina\Time64.h>

#include "DT.h"
#include "DataRow.h"
#include "Column.h"

#define DT_SETBLANKSBYTYPE
#define DT_CHECKTYPE


namespace Stamina { namespace DT {

	const char* const paramComment = "Comment";
	const char* const paramName = "Name";
	const char* const paramOriginalFile = "OriginalFile";
	const char* const paramOwner = "Owner";
	const char* const paramAgent = "Agent";


	class DataTable {
	public:
		typedef std::vector <DataRow *> tRows;
		typedef std::map <std::string, std::string> tParams;


		friend class FileBase;
		friend class FileBin;

	public:
		DataTable ();
		~DataTable ();

		// rows
		tRowId DataTable::getRowId(unsigned int row) const {
			if (isRowId(row)) return row;
			if (row >= this->getRowCount()) return -1;
			return flagId(_rows[row]->getId());
		}
		tRowId DataTable::getRowPos(tRowId row) const {
			if (!isRowId(row)) return row;
			row = unflagId(row);
			for (unsigned int i=0; i < _rows.size(); i++)
				if (_rows[i]->getId() == row) return i;
			return -1;
		}

		inline static bool isRowId(unsigned int val) {
			return ((val)!=0xFFFFFFFF)&&((val) & DT::rowIdFlag) != 0;
		}

		/** Adds flag to the row Id */
		inline static tRowId flagId(tRowId row) {
			return (tRowId)((row) | DT::rowIdFlag);
		}
		/** Removes flag from the row Id */
		inline static tRowId unflagId(tRowId row) {
			return (tRowId)( row&(~DT::rowIdFlag) );
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
			// Je¿eli wiersz z tym id ju¿ istnieje - szukamy nowego...
			if (this->rowIdExists(_lastId)) return getNewRowId();
			return _lastId;
		}

		inline bool rowIdExists(tRowId id) {
			return this->getRowPos(id) != -1;
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


		DataRow& getRow(tRowId row) throw(...) {
			row = this->getRowPos(row);
			if (row == rowNotFound) throw DTException(errNoRow);
			return *this->_rows[row];
		}

		DataEntry get(tRowId row , tColId id) const; // zwraca wartosc w wierszu/kolumnie
		bool set(tRowId row , int tColId , DataEntry val);


		// inne
		inline int getInt(tRowId row , tColId id) const {
			Value v = Value(ctypeInt);
			this->getValue(row, id, v);
			return v.vInt;
		}
		inline int setInt(tRowId row , tColId id , int val) {
			return this->setValue(row, id, ValueInt(val));
		}
		inline const char * getCh(tRowId row , tColId id) const {
			Value v = Value(ctypeString);
			this->getValue(row, id, v);
			return v.vChar;
		}
		inline int setCh(tRowId row , tColId id , const char * val) {
			return this->setValue(row, id, ValueStr(val));
		}

		inline TypeBin getbin(tRowId row , tColId id) const {
			Value v = Value(ctypeBin);
			this->getValue(row, id, v);
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
			this->getValue(row, id, v);
			return v.vInt64;
		}
		inline int set64(tRowId row , tColId id , __int64 val) {
			return this->setValue(row, id, ValueInt64(val));
		}

		inline std::string getStr(tRowId row , tColId id) const {
			Value v = ValueStr(0, -1); // this way we will get string duplicate
			this->getValue(row, id, v);
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
			this->_cols.join(columns, false);
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
		bool getValue(tRowId row , tColId col , Value& value) const;
		
		/** Sets the value using conversion.
		*/
		bool setValue(tRowId row , tColId col , const Value& value);

		inline void setError(enError error) {
			_error = error;
		}
		inline void resetError() {
			_error = errSuccess;
		}
		inline int getError() {
		}

		inline void setOldXorKey(char* key) {
			_cxor_key = key;
		}
		inline const char* getOldXorKey() {
			return _cxor_key;
		}

		static MD5Digest createPasswordDigest(const std::string& pass) {
			return MD5Digest(pass);
		}

		/**Generates password digest*/
		void setPassword(const std::string& pass) {
			_passwordDigest = createPasswordDigest(pass);
		}

		/**Sets password digest*/
		void setPasswordDigest(const MD5Digest& digest) {
			this->_passwordDigest = digest;
		}

		const MD5Digest& getPasswordDigest() {
			return this->_passwordDigest;
		}

		inline bool paramExists(const std::string& name) {
			return _params.find(name) != _params.end();
		}

		inline CStdString getParam(const std::string& name) {
			if (!paramExists(name)) return "";
			return _params[name];
		}

		inline const void setParam(const std::string& name, const std::string& value) {
			_params[name] = value;
		}

		inline void resetParams() {
			_params.clear();
		}

		const tParams getParamsMap() {
			return _params;
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
		MD5Digest _passwordDigest;
		//unsigned int dbID; // Identyfikator bazy
		CriticalSection _cs; // mechanizm blokuj¹cy
		bool _changed;

		Time64 _timeCreated;
		Time64 _timeModified;
		Time64 _timeLastBackup;
	
		/**Additional parameters*/
		tParams _params;
		

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
	};



} }  // namespace'y

#endif


