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
#include "../StringSTL.h"


namespace Stamina { namespace DT {

	class Column: public SharedObject<iColumn> {
	public:

		STAMINA_OBJECT_CLASS(DT::Column, iColumn);

		Column() {
		}

		//virtual iObject* cloneObject();

		void init(unsigned int index, tColId id, enColumnFlag flags, const AStringRef& name) {
			_index = index;
			_id = id;
			this->setFlag(flags, true);
			_name = name;
		}

		virtual DataEntry getDefaultStaticData() const {
			return 0;
		}
		virtual void dataDispose(DataEntry data) const {
		}

	private:

		void setIndex(unsigned int index) {
			_index = index;
		}
		void setId(tColId id) {
			_id = id;
		}

		virtual cloneMembers(const iObject* a) {
			BaseClass::cloneMembers(a);
		}


	};

	class ColumnsDesc {  // Opis typow kolumn
	public:
		typedef std::vector<oColumn> tColumns;
		
	public:
		ColumnsDesc () {
			_loader=false;
		}
		operator = (const ColumnsDesc & x);

		int setColumnCount (int count, bool expand = false); // ustawia ilosc kolumn
		oColumn setColumn (tColId id, enColumnType type , const AStringRef& name="");  // ustawia rodzaj danych w kolumnie
		bool setColumn (const oColumn& col);  // ustawia rodzaj danych w kolumnie

		oColumn setUniqueCol (const AStringRef& name, enColumnType type);  // ustawia rodzaj danych w kolumnie o podanej nazwie

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
			return (Column*)_cols[index].get();
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