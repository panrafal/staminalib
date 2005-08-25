/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#include "stdafx.h"
#include "ColumnTypes.h"
#include "DataTable.h"


namespace Stamina { namespace DT {


	// ---------------------------------------------------  Column

	// ---------------------------------------------------  ColumnsDesc

	oColumn colUndefined = new Column_undefined();

    ColumnsDesc::operator = (const ColumnsDesc & x) {
		_cols.resize(x._cols.size());
		for (unsigned int i=0; i<x._cols.size();i++)
			_cols[i] = x._cols[i]->cloneColumn();
		return 0;
    }

    int ColumnsDesc::setColumnCount (int count, bool expand) {
        if (!expand) {
			// 
			_cols.clear();
		}
        int resize = _cols.size();
        _cols.resize(count + resize, 0);
		for (int i = resize; i < count + resize; ++i) {
			_cols[i] = colUndefined;
		}
        return _cols.size();
    } // ustawia ilosc kolumn

	oColumn ColumnsDesc::setColumn (tColId id, enColumnType type, const AStringRef&  name) {
		oColumn col;

		switch (type & ctypeMask) {
			case ctypeInt64: col = new Column_int64(); break;
			case ctypeString: col = new Column_string(); break;
			case ctypeBin: col = new Column_bin(); break;
			case ctypeDouble: col = new Column_double(); break;
			default: col = new Column_int(); break;
		};

		col->castObject<Column>()->init(0, id, (enColumnFlag)(type & ~ctypeMask), name);

		if (setColumn(col)) {
			return col;
		} else {
			return oColumn();
		}
	}

	oColumn ColumnsDesc::setUniqueCol (const AStringRef& name , enColumnType type) {
		return this->setColumn(colByName, type, name);
    }

	bool ColumnsDesc::setColumn (const oColumn& col) {

		if (col->getId() != colNotFound && ((col->getId() & 0xFF000000) == 0xFF000000)) 
			col->castStaticObject<Column>()->setId(colNotFound);
		if (col->getId() == colByName && col->getName().empty() == false) {
			tColId id = getNameId(col->getName().c_str());
			if (id == colNotFound) {
				id = getNewUniqueId();
			}
			col->castStaticObject<Column>()->setId(id);
		}

		int index = this->colIndex(col->getId());

        try {
			if (index == colNotFound) { //jezeli nie moze znalezc istniejacej kolumny szuka wolnej
				index = this->colIndex(colNotFound);
				if (index == colNotFound) { // jezeli nie moze znalezc wolnej
					index = this->setColumnCount(1 , true) - 1; // dodaje nowa
                }
            }

			col->castStaticObject<Column>()->setIndex(index);

			if (this->isLoader())
				col->setFlag(cflagIsLoaded, true);
			else 
				col->setFlag(cflagIsDefined, true);

			_cols[index] = col;

        } catch (...) {
			return false;
		}
        return true;
    }

    unsigned int ColumnsDesc::colIndex (tColId id) const {
        int i = 0;
		for (tColumns::const_iterator it = _cols.begin(); it != _cols.end(); it++) {
			if ((*it)->getId() == id) 
				return i; 
			i++;
		}
		return colNotFound;
    }

    tColId ColumnsDesc::getNewUniqueId(void) {
        int unique = time(0) & 0xFFFF;
        do {
            unique++;
            unique &=0xFFFF;
		} while (this->colIndex((tColId)(unique | colIdUniqueFlag)) != colNotFound);
        return (tColId)(unique | colIdUniqueFlag);
    }

    tColId ColumnsDesc::getNameId(const char * name) const {
        if (_cols.size()==0) return colNotFound;
		for (tColumns::const_iterator it = _cols.begin(); it != _cols.end(); it++) {
			if ((*it)->getName().equal(name)) 
				return (*it)->getId(); 
		}
		return colNotFound;
    }

	int ColumnsDesc::join(const ColumnsDesc& other, bool overwrite) {
		int c = 0;
		for (tColumns::const_iterator it = other._cols.begin(); it != other._cols.end(); it ++) {
			oColumn col = *it;

			// Sprawdzamy czy taka ju¿ nie istnieje...
			if (!overwrite) {
				tColId id = col->getId();
				if (col->isIdUnique())
					id = this->getNameId(col->getName().c_str());
				if (id != colNotFound && this->colIndex(id) != colNotFound)
					continue;
			}
			// ustawiamy kolumnê

			oColumn newCol = static_cast<Column*>( col->cloneObject() );
			if (col->isIdUnique()) {
				newCol->castStaticObject<Column>()->setId(colByName);
			}
			this->setColumn(newCol);

		}
		return c;
	}


} }