/*

The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License from
/LICENSE.HTML in this package or at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is "Stamina.lib" library code, released Feb 1, 2006.

The Initial Developer of the Original Code is "STAMINA" - Rafa³ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa³ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

*/


#include "stdafx.h"
#include "ColumnTypes.h"
#include "DataTable.h"


namespace Stamina { namespace DT {


	// ---------------------------------------------------  Column

	// ---------------------------------------------------  ColumnsDesc

	StaticObj<Column_undefined> colUndefined;

    ColumnsDesc::operator = (const ColumnsDesc & x) {
        LockerCS lock(_cs);
		_cols.resize(x._cols.size());
		for (unsigned int i=0; i<x._cols.size();i++)
			_cols[i] = x._cols[i]->cloneColumn();
		return 0;
    }

    int ColumnsDesc::setColumnCount (int count, bool expand) {
        LockerCS lock(_cs);
        if (!expand) {
			// 
			_cols.clear();
		}
        int resize = _cols.size();
        _cols.resize(count + resize, 0);
		for (int i = resize; i < count + resize; ++i) {
			_cols[i] = colUndefined.get();
		}
        return _cols.size();
    } // ustawia ilosc kolumn

	oColumn ColumnsDesc::setColumn (tColId id, enColumnType type, const AStringRef&  name) {
        LockerCS lock(_cs);
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
        LockerCS lock(_cs);

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
        LockerCS lock(_cs);
        int i = 0;
		for (tColumns::const_iterator it = _cols.begin(); it != _cols.end(); it++) {
			if ((*it)->getId() == id) 
				return i; 
			i++;
		}
		return colNotFound;
    }


	iColumn* ColumnsDesc::getUndefinedColumn() const {
		return colUndefined.get();
	}


    tColId ColumnsDesc::getNewUniqueId(void) {
        LockerCS lock(_cs);
        int unique = time(0) & 0xFFFF;
        do {
            unique++;
            unique &=0xFFFF;
		} while (this->colIndex((tColId)(unique | colIdUniqueFlag)) != colNotFound);
        return (tColId)(unique | colIdUniqueFlag);
    }

    tColId ColumnsDesc::getNameId(const StringRef& name) const {
        LockerCS lock(_cs);
        if (_cols.size()==0) return colNotFound;
		for (tColumns::const_iterator it = _cols.begin(); it != _cols.end(); it++) {
			if ((*it)->getName().equal(name)) 
				return (*it)->getId(); 
		}
		return colNotFound;
    }

	int ColumnsDesc::join(const ColumnsDesc& other, bool overwrite) {
        LockerCS lock(_cs);
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