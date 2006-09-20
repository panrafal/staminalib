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
#include "Column.h"
#include "DataTable.h"

namespace Stamina { namespace DT {

	/*
	DataRow::DataRow (const DataRow & v) {
		freeData(); 
	}
	*/

	void DataRow::prepareSlot(unsigned int colIndex) {
		unsigned int oldSize = _data.size();
		_data.resize(max(_data.size(), colIndex + 1), 0);
		// inicjalizujemy domyœlne proste poza aktualnie szykowanym elementem! (za chwile i tak zostanie nadpisany)
		if (oldSize < _data.size()) {
			ColumnsDesc::tColumns::const_iterator colIt = _table->getColumns().begin() + oldSize;
			for (unsigned int i = oldSize; i < _data.size() - 1; ++i, ++colIt) {
				S_ASSERT( colIt != _table->getColumns().end() );
				Column* col = static_cast<Column*>( colIt->get() );
				if (col->isStaticType()) {
					_data[i] = col->getDefaultStaticData();
				}
			}
		}
	}

    bool DataRow::freeData() {
		ObjLocker lock (this, lockDefault);
		ColumnsDesc::tColumns::const_iterator colIt = _table->getColumns().begin();
		for (tData::iterator i = _data.begin() ; i != _data.end() ; ++i, ++colIt) {
			S_ASSERT( colIt != _table->getColumns().end() );
			Column* col = static_cast<Column*>( colIt->get() );
			col->dataDispose(*i);
		}
		_data.clear();
	    return true;
    }


	const DataEntry DataRow::getData (unsigned int colIndex) const { // Pobiera wartosc kolumny
		if (this->hasColumnSlot(colIndex) == false) {
			return 0;
		}
		return _data[colIndex];
    }



	bool DataRow::setData (unsigned int colIndex, DataEntry val) { // ustawia wartosc kolumny
		if (! this->hasColumnSlot(colIndex)) {
			this->prepareSlot(colIndex);
		}

		_data[colIndex] = val;

		return true;
    }

	void DataRow::removeFromTable() {
		if (_table) {
			_table->deleteRow(this->getId());
		}
	}


	// ----------------------------------------  DataRow





} }