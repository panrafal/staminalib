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

	class Column {
	public:

		Column();

		inline enColumnType getType() const {
			return (enColumnType) (_type & ctypeMask);
		}
		inline enColumnFlag getFlags() const {
			return (enColumnFlag) _type;
		}

		inline tColId getId() const {
			return _id;
		}

		inline void setId(tColId id) {
			_id = id;
		}

		inline bool isIdUnique() const {
			return (_id & colIdUniqueFlag) != 0;
		}

		/** Returns true if type occupies only 4 bytes and can be stored directly (without creating objects)
		*/
		inline bool isStaticType() const {
			return this->getType() == ctypeInt;
		}

		inline bool empty() const {
			return this->_type == ctypeUnknown;
		}

		inline const std::string& getName() const {
			return _name;
		}
		inline void setName(const std::string& name) {
			_name = name;
		}

		void setType(enColumnType type, bool resetFlags) {
			if (resetFlags)
				_type = type;
			else
				_type = (enColumnType)((_type & ~ctypeMask) | type);
		}

		void setFlag(enColumnFlag flag, bool setting) {
			if (setting)
				_type = _type | flag;
			else
				_type = (enColumnType) (_type & (~flag));
		}
		bool hasFlag(enColumnFlag flag) const {
			return (_type & flag) != 0;
		}

		inline DataEntry getDefValue() const {
			return _def;
		}
		inline void setDefValue(DataEntry val) {
			_def = val;
		}

	private:
		enColumnType _type;
		tColId _id;
		DataEntry _def; // default
		std::string _name;  // tekstowy identyfikator


	};

	extern const Column emptyColumn;

	class ColumnsDesc {  // Opis typow kolumn
	public:
		typedef std::vector<Column> tColumns;
	public:
		ColumnsDesc ();
		operator = (const ColumnsDesc & x);

		int setColumnCount (int count, bool expand = false); // ustawia ilosc kolumn
		tColId setColumn (tColId id , enColumnType type , DataEntry def=0 , const char * name="");  // ustawia rodzaj danych w kolumnie
		tColId setUniqueCol (const char * name , enColumnType type , DataEntry def=0);  // ustawia rodzaj danych w kolumnie o podanej nazwie

		inline unsigned int getColCount () const {
			return _cols.size();
		}
		inline unsigned int size() const {
			return _cols.size();
		}

		const Column& getColumnByIndex(unsigned int index) const;
		const Column& getColumn(tColId id) const {
			return getColumnByIndex(colIndex(id));
		}
		const Column& getColumn(const char* name) const {
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