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

	void DefaultRow::prepareSlot(unsigned int colIndex) {
		int oldSize = _data.size();
		_data.resize(max(_data.size(), colIndex + 1), 0);
	}

    bool DefaultRow::freeData() {
		LockerCS lock (_cs);
		ColumnsDesc::tColumns::const_iterator colIt = _table->getColumns().begin() + oldSize;
		for (tData::iterator i = _data.begin() ; i != _data.end() ; ++i, ++colIt) {
			S_ASSERT( colIt != _table->getColumns().end() );
			Column* col = static_cast<Column*>( colIt->get() );
			col->dataDispose(*i);
		}
		_data.clear();
	    return true;
    }


	const DataEntry DefaultRow::getData (unsigned int colIndex) { // Pobiera wartosc kolumny
		if (this->hasColumnSlot(colIndex) == false) {
			return 0;
		}
		return _data[colIndex];
    }



	bool DataRow::setData (unsigned int colIndex, DataEntry val) { // ustawia wartosc kolumny
		if (! this->hasColumnData(colIndex)) {
			this->prepareSlot(colIndex);
		}

		_data[colIndex] = val;

		return true;
    }



	// ----------------------------------------  DataRow

	void DataRow::prepareSlot(unsigned int colIndex) {
		int oldSize = _data.size();
		_data.resize(max(_data.size(), colIndex + 1), 0);
		// inicjalizujemy domyœlne proste poza aktualnie szykowanym elementem! (za chwile i tak zostanie nadpisany)
		if (oldSize < _data.size()) {
			ColumnsDesc::tColumns::const_iterator colIt = _table->getColumns().begin() + oldSize;
			for (int i = oldSize; i < _data.size() - 1; ++i, ++colIt) {
				S_ASSERT( colIt != _table->getColumns().end() );
				iColumn* col = colIt->get();
				if (col->isStaticType()) {
					_data[i] = _table->getDefaults().getByIndex(i);
				}
			}
		}
	}




} }