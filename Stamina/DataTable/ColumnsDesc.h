/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: Column.h 95 2005-08-25 01:14:51Z hao $
 */
#pragma once
#ifndef __DT_COLUMNSDESC__
#define __DT_COLUMNSDESC__

#include "iColumn.h"

namespace Stamina { namespace DT {

	#ifndef __DT_COLUMN__
	typedef iColumn Column;
	#define __DT_COLUMN__
	#define __DT_COLUMN__CHEAP
	#endif

	class ColumnsDesc {  // Opis typow kolumn
	public:
		typedef std::vector<oColumn> tColumns;
		
	public:
		ColumnsDesc () {
			_loader=false;
		}
		int operator = (const ColumnsDesc & x);

		int setColumnCount (int count, bool expand = false); // ustawia ilosc kolumn
		oColumn setColumn (tColId id, enColumnType type , const AStringRef& name="");  // ustawia rodzaj danych w kolumnie
		bool setColumn (const oColumn& col);  // ustawia rodzaj danych w kolumnie

		oColumn setUniqueCol (const AStringRef& name, enColumnType type);  // ustawia rodzaj danych w kolumnie o podanej nazwie

		inline unsigned int getColCount () const {
	        LockerCS lock(_cs);
			return _cols.size();
		}
		inline unsigned int size() const {
	        LockerCS lock(_cs);
			return _cols.size();
		}

		tColumns::const_iterator begin() const {
			return _cols.begin();
		}
		tColumns::const_iterator end() const {
			return _cols.end();
		}

		Column* getColumnByIndex(unsigned int index) const {
	        LockerCS lock(_cs);
			if (index > _cols.size()) {
				return (Column*)getUndefinedColumn();
			}
			return (Column*)_cols[index].get();
		}

		iColumn* getUndefinedColumn() const;

		Column* getColumn(tColId id) const {
			return getColumnByIndex(colIndex(id));
		}
		Column* getColumn(const StringRef& name) const {
			return getColumn(getNameId(name));
		}

		unsigned int colIndex (tColId id) const; // zwraca index kolumny

		void clear() {
	        LockerCS lock(_cs);
			_cols.clear();
		}

		int join(const ColumnsDesc& other , bool overwrite); // ³¹czy dwa deskryptory, zwraca liczbê do³¹czonych

		tColId getNewUniqueId(void);
		tColId getNameId(const StringRef& name) const;

		bool isLoader() const {
			return _loader;
		}
		void setLoader(bool loader) {
			_loader = loader;
		}

	protected:
		tColumns _cols;
		bool _loader;
		CriticalSection_w32 _cs;
	};
} }

#endif