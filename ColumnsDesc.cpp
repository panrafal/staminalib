/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#include "stdafx.h"
#include "DataTable.h"


namespace Stamina { namespace DT {

	const Column emptyColumn;

	Column::Column() {
		type = ctypeUnknown;
		id = colNotFound;
		def = 0;
		name = "";
	}

	ColumnsDesc::ColumnsDesc () {
		_loader=false;
	}

    ColumnsDesc::operator = (ColumnsDesc & x) {
		_cols.resize(x._cols.size());
		for (unsigned int i=0; i<x._cols.size();i++)
			_cols[i]=x._cols[i];
		return 0;
    }


    int ColumnsDesc::setColCount (int count, bool expand) {
        if (!expand) {_cols.clear();}
        int resize = _cols.size();
        _cols.resize(count + resize , bl);
		for (tColumns::iterator = _cols.begin() + resize; cols_it != _cols.end() ; cols_it++ ) {
            cols_it->id=-1;
            cols_it->type=deftype;
            cols_it->def=0;
        }
/*        if (fill) {
			for (int i = 0;i<count;i++)
				setcol(i , deftype);
        }*/
        return _cols.size();
    } // ustawia ilosc kolumn

    void ColumnsDesc::optimize(void) {
		tColumns::iterator it = _cols.begin();
        while (it!=_cols.end()) {
          if (it->id == -1) 
			  it = cols.erase(it);
          else 
			  cols_it++;
        }
        return;
    }

	tColId ColumnsDesc::setcol (tColId id , enColumnFlag type , DataEntry def , const char * name) {
        if (!name) name = "";
        if (id != colNotFound && ((id & 0xFF000000) == 0xFF000000)) 
			id = colNotFound;
        if (id == colNotFound && *name) return setUniqueCol(name , type , def);
        //if (table) ((DataTable *)table)->error=0;
        int index = colIndex(id);
        try {
			if (index<0) { //jezeli nie moze znalezc istniejacej kolumny szuka wolnej
				index = colIndex(colNotFound);
				if (index < 0) { // jezeli nie moze znalezc wolnej
					index = setColCount(1 , true) - 1; // dodaje nowa
                }
            }
            Column& v = _cols[index];
			if (this->isLoader())
				v.type = cflagIsLoaded;
			else
				v.type = 0;
			v.type |= type;
			v.id=id;
			v.def=def;
			v.name=name;
        } catch (...) {
			//if (table) ((DataTable *)table)->error=DT_ERR_NOCOL;
		}
        return id;
    }

	tColId ColumnsDesc::setUniqueCol (const char * name , enColumnFlag type , DataEntry def) {
        if (!name) name = "";
        //if (table) ((DataTable *)table)->error=0;
        tColId id = getNameId(name);
        if (id == colNotFound) {
			id = getNewUniqueId();
		}
        setCol(id , type , def , name);
        return id;
    }

	const Column& ColumnsDesc::getColumnByIndex(int index) const {
		if (index > _cols.size()) {
			return emptyColumn;
		}
		return _cols.at(index);
	}

    int ColumnsDesc::colIndex (tColId id) const {
        int i = 0;
		for (tColumns::iterator it = _cols.begin(); it != _cols.end(); it++) {
			if (it->id == id) 
				return i; 
			i++;
		}
        return -1;
    }

    tColId ColumnsDesc::getNewUniqueId(void) {
        int unique = time(0) & 0xFFFF;
        do {
            unique++;
            unique &=0xFFFF;
		} while (this->colIndex((tColId)(unique | colIdUniqueFlag))!=-1);
        return (tColId)(unique | colIdUniqueFlag);
    }

    tColId ColumnsDesc::getNameId(const char * name) const {
        if (_cols.size()==0) return colNotFound;
		for (tColumn::iterator it = _cols.begin(); it != _cols.end(); it++) {
			if (it->name == name) return it->id; 
		}
		return colNotFound;
    }

	/** Appends another descriptor
	*/
	int ColumnsDesc::join(const ColumnsDesc& other, bool overwrite) {
		int c = 0;
		for (tColumns::const_iterator i = other->_cols.begin(); i != other->_cols.end(); i++) {
			// Sprawdzamy czy taka ju¿ nie istnieje...
			if (!overwrite) {
				int id = i->id;
				if (id & colIdUniqueFlag)
					id = this->getNameId(i->name.c_str());
				if (id != -1 && this->colIndex(id) != -1)
					continue;
			}
			// ustawiamy kolumnê
			this->setCol((i->id & colIdUniqueFlag)? -1 : i->id , i->type , i->def , i->name.c_str());
		}
		return c;
	}


} }