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
		LockerCS lock (_cs);
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



	// ----------------------------------------  DataRow





} }