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
#include "DataTable.h"


namespace Stamina { namespace DT {

	const Column emptyColumn;

	Column::Column() {
		_type = ctypeUnknown;
		_id = colNotFound;
		_def = 0;
		_name = "";
	}

	ColumnsDesc::ColumnsDesc () {
		_loader=false;
	}

    ColumnsDesc::operator = (const ColumnsDesc & x) {
		_cols.resize(x._cols.size());
		for (unsigned int i=0; i<x._cols.size();i++)
			_cols[i]=x._cols[i];
		return 0;
    }


    int ColumnsDesc::setColumnCount (int count, bool expand) {
        if (!expand) {_cols.clear();}
        int resize = _cols.size();
        _cols.resize(count + resize);
        return _cols.size();
    } // ustawia ilosc kolumn

    void ColumnsDesc::optimize(void) {
		tColumns::iterator it = _cols.begin();
        while (it!=_cols.end()) {
			if (it->getId() == colNotFound) 
				it = _cols.erase(it);
			else 
				it ++;
        }
        return;
    }

	tColId ColumnsDesc::setColumn (tColId id , enColumnType type , DataEntry def , const char * name) {
        if (!name) name = "";
        if (id != colNotFound && ((id & 0xFF000000) == 0xFF000000)) 
			id = colNotFound;
        if (id == colNotFound && *name) return setUniqueCol(name , type , def);
        //if (table) ((DataTable *)table)->error=0;
        int index = colIndex(id);
        try {
			if (index == colNotFound) { //jezeli nie moze znalezc istniejacej kolumny szuka wolnej
				index = colIndex(colNotFound);
				if (index == colNotFound) { // jezeli nie moze znalezc wolnej
					index = setColumnCount(1 , true) - 1; // dodaje nowa
                }
            }
            Column& v = _cols[index];
			v.setType(type, true);
			if (this->isLoader())
				v.setFlag(cflagIsLoaded, true);
			else 
				v.setFlag(cflagIsDefined, true);
			v.setId(id);
			v.setDefValue(def);
			v.setName(name);
        } catch (...) {
			//if (table) ((DataTable *)table)->error=DT_ERR_NOCOL;
		}
        return id;
    }

	tColId ColumnsDesc::setUniqueCol (const char * name , enColumnType type , DataEntry def) {
        if (!name) name = "";
        //if (table) ((DataTable *)table)->error=0;
        tColId id = getNameId(name);
        if (id == colNotFound) {
			id = getNewUniqueId();
		}
		setColumn(id , type , def , name);
        return id;
    }

	const Column& ColumnsDesc::getColumnByIndex(unsigned int index) const {
		if (index > _cols.size()) {
			return emptyColumn;
		}
		return _cols.at(index);
	}

    unsigned int ColumnsDesc::colIndex (tColId id) const {
        int i = 0;
		for (tColumns::const_iterator it = _cols.begin(); it != _cols.end(); it++) {
			if (it->getId() == id) 
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
			if (it->getName() == name) return it->getId(); 
		}
		return colNotFound;
    }

	/** Appends another descriptor
	*/
	int ColumnsDesc::join(const ColumnsDesc& other, bool overwrite) {
		int c = 0;
		for (tColumns::const_iterator it = other._cols.begin(); it != other._cols.end(); it ++) {
			// Sprawdzamy czy taka ju¿ nie istnieje...
			if (!overwrite) {
				tColId id = it->getId();
				if (it->isIdUnique())
					id = this->getNameId(it->getName().c_str());
				if (id != colNotFound && this->colIndex(id) != colNotFound)
					continue;
			}
			// ustawiamy kolumnê
			this->setColumn((it->isIdUnique())? colByName : it->getId() , (enColumnType) it->getFlags() , it->getDefValue() , it->getName().c_str());
		}
		return c;
	}


} }