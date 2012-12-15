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
#include "..\CriticalSection.h"
#include "..\MD5.h"
#include "..\Time64.h"
#include "..\Version.h"

#include "DT.h"
#include "DataRow.h"
#include "ColumnsDesc.h"
#include "Find.h"
#include "Interface.h"

#define DT_SETBLANKSBYTYPE
#define DT_CHECKTYPE


namespace Stamina { namespace DT {

	const char* const paramComment = "Comment";
	const char* const paramName = "Name";
	const char* const paramOriginalFile = "OriginalFile";
	const char* const paramOwner = "Owner";
	const char* const paramAgent = "Agent";

	const ModuleVersion dataTableVersion (versionModule, "DataTable", Version(3,0,0,0));
	STAMINA_REGISTER_VERSION(DataTable, dataTableVersion);

	class DataTable: public Object<iObject> {
	public:

		STAMINA_OBJECT_CLASS(DT::DataTable, iObject);

		typedef std::vector <oDataRow> tRows;
		typedef std::map <String, String> tParams;


		friend class FileBase;
		friend class FileBin;
		friend class ColumnsDesc;

	public:
		DataTable ();
		~DataTable ();

		// rows
		tRowId DataTable::getRowId(unsigned int row) const {
	        LockerCS lock(_cs);
			if (isRowId(row)) return row;
			if (row >= this->getRowCount()) return rowNotFound;
			return flagId(_rows[row]->getId());
		}
		tRowId DataTable::getRowPos(tRowId row) const {
	        LockerCS lock(_cs);
			if (!isRowId(row)) return row;
			for (unsigned int i=0; i < _rows.size(); i++)
				if (_rows[i]->getId() == row) return i;
			return rowNotFound;
		}

		inline static bool isRowId(unsigned int val) {
			return ((val)!=0xFFFFFFFF)&&((val) & DT::rowIdFlag) != 0;
		}

		/** Adds flag to the row Id */
		inline static tRowId flagId(tRowId row) {
			return DataRow::flagId(row);
		}
		/** Removes flag from the row Id */
		inline static tRowId unflagId(tRowId row) {
			return DataRow::unflagId(row);
		}

		oDataRow addRow(tRowId id = rowNotFound); // Dodaje wiersz , mozna podac ID

		oDataRow insertRow(unsigned int row , tRowId id = rowNotFound); // wstawia wiersz , mozna podac ID

		bool deleteRow(tRowId row);

		inline unsigned int getRowCount() const {
	        LockerCS lock(_cs);
			return _rows.size();
		}

		void swapRows(tRowId a, tRowId b);

		void clearRows();

		tRowId getNewRowId() {
	        LockerCS lock(_cs);
	        _lastId++;
			if (_lastId > rowIdMax) _lastId = rowIdMin;
			// Je¿eli wiersz z tym id ju¿ istnieje - szukamy nowego...
			if (this->rowIdExists(flagId(_lastId))) return getNewRowId();
			return flagId(_lastId);
		}

		inline bool rowIdExists(tRowId id) {
			return this->getRowPos(id) != rowNotFound;
		}

		/** Znajduje wiersz spe³niaj¹cy podane kryteria.
		@param startPos - numer wiersza od którego zaczynamy szukanie
		@param argCount - liczba przekazywanych struktur Find*, lub -1 je¿eli ostatnie kryterium jest równe 0
		@param ... - kolejne kryteria jako struktury Find*, najbezpieczniej dodaæ na koñcu kryterium 0

		np. Znajduje pierwszy kontakt sieci NET_GG aktywny w ci¹gu ostatniej minuty.
		dt->findRow(0, -1, &Find::EqInt(CNT_NET, NET_GG), &Find(Find::gt, CNT_ACTIVITY, ValueInt64(_time64(0) - 60000)), 0);
		*/
		oDataRow findRow(unsigned int startPos, int argCount, ...);

		oDataRow findRow(unsigned int startPos, int argCount, va_list list);

		inline oDataRow findRow(unsigned int startPos, Find& f1) {
			return this->findRow(startPos, 1, &f1);
		}
		inline oDataRow findRow(unsigned int startPos, Find& f1, Find& f2) {
			return this->findRow(startPos, 2, &f1, &f2);
		}
		inline oDataRow findRow(unsigned int startPos, Find& f1, Find& f2, Find& f3) {
			return this->findRow(startPos, 3, &f1, &f2, &f3);
		}


		oDataRow getRow(tRowId row) throw(...) {
			row = this->getRowPos(row);
			if (row == rowNotFound) return 0; // throw DTException(errNoRow);
			return this->_rows[row];
		}

		/*
		DataEntry get(tRowId row , tColId id) throw(...); // zwraca wartosc w wierszu/kolumnie
		bool set(tRowId row , tColId col, DataEntry val, bool dropDefault = false) throw(...);
		*/

		// inne
		inline int getInt(tRowId row , tColId id, GetSet flags = gsNone)  {
			return this->getColumn(id)->getInt( this->getRow(row), flags);
		}
		inline bool setInt(tRowId rowId , tColId id , int val, GetSet flags = gsNone) {
			oRow row = this->getRow(rowId);
			if (row.isValid()) {
				return this->getColumn(id)->setInt(row , val, flags);
			}
			return false;
		}
		inline String getString(tRowId row , tColId id, GetSet flags = getCopy) {
			return PassStringRef( this->getColumn(id)->getString( this->getRow(row), flags ) );
		}
		inline bool setString(tRowId rowId , tColId id , const StringRef& val, GetSet flags = gsNone) {
			oRow row = this->getRow(rowId);
			if (row.isValid()) {
				return this->getColumn(id)->setString(row, val, flags);
			}
			return false;
		}

		inline ByteBuffer getBin(tRowId row , tColId id, GetSet flags = getCopy) {
			ByteBuffer b;
			b.swap( this->getColumn(id)->getBin( this->getRow(row), flags ) );
			return b;
		}
		inline bool setBin(tRowId rowId , tColId id , const ByteBuffer& val, GetSet flags = gsNone) {
			oRow row = this->getRow(rowId);
			if (row.isValid()) {
				return this->getColumn(id)->setBin(row, val, flags);
			}
			return false;
		}

		inline __int64 get64(tRowId row , tColId id, GetSet flags = gsNone) {
			return this->getColumn(id)->getInt64( this->getRow(row), flags );
		}
		inline bool set64(tRowId rowId , tColId id , __int64 val, GetSet flags = gsNone) {
			oRow row = this->getRow(rowId);
			if (row.isValid()) {
				return this->getColumn(id)->setInt64(row , val, flags );
			}
			return false;
		}

		inline double getDouble(tRowId row , tColId id, GetSet flags = gsNone) {
			return this->getColumn(id)->getDouble( this->getRow(row), flags );
		}
		inline bool setDouble(tRowId rowId , tColId id , double val, GetSet flags = gsNone) {
			oRow row = this->getRow(rowId);
			if (row.isValid()) {
				return this->getColumn(id)->setDouble(row , val, flags );
			}
			return false;
		}



		inline const ColumnsDesc& getColumns() {
			return this->_cols;
		}

		inline Column* getColumn(tColId colId) const {
			return this->_cols.getColumn(colId);
		}

		inline Column* getColumn(const StringRef& name) const {
			return this->_cols.getColumn(name);
		}

		void clearColumns() {
			if (_rows.size() > 0) return;
			this->_cols.clear();
		}

		void mergeColumns(const ColumnsDesc& columns) {
			if (_rows.size() > 0) return;
			this->_cols.join(columns, false);
		}

		inline oColumn setColumn (tColId id , enColumnType type , const AStringRef& name = AStringRef()) {
			if (_rows.size() > 0) return oColumn();
			return _cols.setColumn(id, type, name);
		}
		inline oColumn setUniqueCol (const AStringRef& name , enColumnType type) {
			if (_rows.size() > 0) return oColumn();
			return _cols.setUniqueCol(name, type);
		}

		inline tColId getColumnId(const AStringRef& name) {
			return this->_cols.getNameId(name);
		}


		int checkColType(tColId id , enColumnType type) {
			return this->_cols.getColumn(id)->getType() == type;
		}

		bool idExists(int id) {
			if (isRowId(id)) 
				return getRowPos(id) != rowNotFound;
			else 
				return getRowId(id) != rowNotFound;
		}

		void setChanged() {
	        LockerCS lock(_cs);
			_changed = true;
		}

		bool isChanged() {
	        LockerCS lock(_cs);
			return _changed;
		}

		void lock(tRowId row) throw(...);
		void unlock(tRowId row) throw(...);
		bool canAccess(tRowId row) throw(...);

		/* Gets the value of a field using type conversion.
		- When retrieving ctypeString/ctypeWideString there are several possible ways of setting input buffer in Value:
			vChar = 0 buffSize = -1  - a duplicate is returned in vChar
			vChar = 0 buffSize = 0   - size of the string is returned in buffSize
			vChar = * buffSize = 0   - if the column is of the type string, the @a vChar is replaced with the internal value pointer (it's READ ONLY and it's not thread-safe! you MUST lock the row first!). Otherwise, the small buffer * (at least 32 bytes) that is provided in @a vChar is used for conversion
			vChar = * buffSize = #   - the value is copied into the @a vChar
		- When retrieving ctypeBin there won't be any conversion! There are several ways of setting input buffer in Value::vBin :
			buffer = 0 size = -1     - a duplicate is returned in @a buffer
			buffer = 0 size = 0		 - size of the data is returned in @a size
			buffer = -1 size = 0		 - the @a buffer is replaced with the internal data pointer (it's READ ONLY and it's not thread-safe! you MUST lock the row first!). size is also being returned.
			buffer = * size = #		 - the data is copied into the @a buffer
*/
		//bool getValue(tRowId row, tColId col, Value& value);
		
		/** Sets the value using type conversion.
		*/
		//bool setValue(tRowId row, tColId col, const Value& value, bool dropDefault=false);

		inline void setXor1Key(char* key) {
	        LockerCS lock(_cs);
			_xor1_key = (unsigned char*) key;
		}
		inline const unsigned char* getXor1Key() {
			return _xor1_key;
		}

		static MD5Digest createPasswordDigest(const StringRef& pass) {
			return MD5Digest(pass.a_str());
		}

		/**Generates password digest*/
		void setPassword(const StringRef& pass) {
			setPasswordDigest(createPasswordDigest(pass.a_str()));
		}

		/**Sets password digest*/
		void setPasswordDigest(const MD5Digest& digest) {
	        LockerCS lock(_cs);
			this->_passwordDigest = digest;
			this->_changed = true;
		}

		const MD5Digest& getPasswordDigest() {
			return this->_passwordDigest;
		}

		inline bool paramExists(const StringRef& name) {
	        LockerCS lock(_cs);
			return _params.find(name) != _params.end();
		}

		inline String getParam(const StringRef& name) {
	        LockerCS lock(_cs);
			if (!paramExists(name)) return "";
			return _params[name];
		}

		inline const void setParam(const StringRef& name, const StringRef& value) {
	        LockerCS lock(_cs);
			_params[name] = value;
			_changed = true;
		}

		inline void resetParam(const StringRef& name) {
	        LockerCS lock(_cs);
			_changed = true;
			_params.erase(name);
		}

		inline void resetParams() {
	        LockerCS lock(_cs);
			_changed = true;
			_params.clear();
		}

		const tParams& getParamsMap() {
	        LockerCS lock(_cs);
			return _params;
		}

		const Time64& getTimeCreated() const { 
			return _timeCreated;
		}
		const Time64& getTimeModified() const { 
			return _timeModified;
		}
		const Time64& getTimeLastBackup() const { 
			return _timeLastBackup;
		}
		void setTimeLastBackup(const Time64& time) {
			_changed = true;
			_timeLastBackup = time;
		}

		const oInterface& getInterface() const {
			return _interface;
		}

		void setInterface(const oInterface& iface) {
			_interface = iface;
		}
		

	private:
		tRows _rows;
		int _size;
		tRowId _lastId; // ostatni identyfikator wiersza
		ColumnsDesc _cols;
		//enError _error;
		//int mode;
		//int notypecheck;
		unsigned char * _xor1_key;
		MD5Digest _passwordDigest;
		//unsigned int dbID; // Identyfikator bazy
		CriticalSection _cs; // mechanizm blokuj¹cy
		bool _changed;

		Time64 _timeCreated;
		Time64 _timeModified;
		Time64 _timeLastBackup;
	
		oInterface _interface;

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


