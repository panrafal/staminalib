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


namespace Stamina { namespace DT {

	struct Column {
		enColumnFlag type;
		tColId id;
		DataEntry def; // default
		std::string name;  // tekstowy identyfikator

		Column();

		inline enColumnType getType() {
			return (enColumnType) (type & ctypeMask);
		}
		inline enColumnFlag getFlags() {
			return type;
		}

		inline tColId getId() {
			return id;
		}

		inline bool isIdUnique() {
			return (id & colIdUniqueFlag) != 0;
		}

		inline bool empty() {
			return this->type == ctypeUnknown;
		}

		inline const std::string& getName() {
			return name;
		}

		void setFlag(enColumnFlag flag, bool setting) {
			if (setting)
				type = (enColumnFlag) (type | flag);
			else
				type = (enColumnFlag) (type & (~flag));
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
		operator = (const ColumnsDesc & x);

		int setColCount (int count, bool expand = false); // ustawia ilosc kolumn
		tColId setColumn (tColId id , enColumnFlag type , DataEntry def=0 , const char * name="");  // ustawia rodzaj danych w kolumnie
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
} }

#endif