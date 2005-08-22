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
#ifndef __DT_COLUMN__
#define __DT_COLUMN__

#include "iColumn.h"


namespace Stamina { namespace DT {

	class Column: public SharedObject<iColumn> {
	public:

		STAMINA_OBJECT_CLASS(DT::Column, iColumn);

		Column();

		virtual iObject* cloneObject();

		void init(unsigned int index, tColId id, enColumnFlag flags, const StringRef& name) {
			_index = index;
			_id = id;
			this->setFlag(flags, true);
			_name = name;
		}

		/** Initializes the row's data.
		@param alloc - true if any data should be allocated.
		*/
		virtual DataEntry dataInit(bool alloc) const = 0;
		virtual void dataDispose(DataEntry data) const = 0;

	private:

	};

	class Column_int: public Column {
	public:

		STAMINA_OBJECT_CLASS(DT::Column_int, Column);

		virtual iObject* cloneObject();



	};



	class ColumnsDesc {  // Opis typow kolumn
	public:
		typedef std::vector<oColumn> tColumns;
	public:
		ColumnsDesc ();
		operator = (const ColumnsDesc & x);

		int setColumnCount (int count, bool expand = false); // ustawia ilosc kolumn
		tColId setColumn (tColId id, enColumnType type , const StringRef& name="");  // ustawia rodzaj danych w kolumnie
		tColId setUniqueCol (const StringRef& name, enColumnType type);  // ustawia rodzaj danych w kolumnie o podanej nazwie

		inline unsigned int getColCount () const {
			return _cols.size();
		}
		inline unsigned int size() const {
			return _cols.size();
		}

		tColumns::const_iterator begin() const {
			return _cols.begin();
		}
		tColumns::const_iterator end() const {
			return _cols.end();
		}

		const Column* getColumnByIndex(unsigned int index) const {
			if (index > _cols.size()) {
				return 0;
			}
			return _cols[index];
		}
		const Column* getColumn(tColId id) const {
			return getColumnByIndex(colIndex(id));
		}
		const Column* getColumn(const char* name) const {
			return getColumn(getNameId(name));
		}

		unsigned int colIndex (tColId id) const; // zwraca index kolumny

		void clear() {
			_cols.clear();
		}

		void optimize();
		int join(const ColumnsDesc& other , bool overwrite); // ³¹czy dwa deskryptory, zwraca liczbê do³¹czonych

		tColId getNewUniqueId(void);
		tColId getNameId(const char * name) const;

		bool isLoader() const {
			return _loader;
		}
		void setLoader(bool loader) {
			_loader = loader;
		}

	protected:
		tColumns _cols;
		bool _loader;
	};
} }

#endif