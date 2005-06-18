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

	DataRow::DataRow (const DataRow & v) {
		freeData(); 
	}

    DataRow::DataRow (DataTable * table, char allocate) {
		_id = 0;
		_flag = 0;
		_size=0;
		_table=t;
		if (allocate) {
			allocData();
		}
    }

    DataRow::~DataRow () {
		freeData(); 
	}

    int DataRow::allocData() {
		LockerCS lock (_cs);
        this->freeData();
		_size = _table->getColumns().size();
		if (!size) {
			return 0;
		}

		this->_data = new DataEntry [size];
	    for (int i = 0 ; i < size ; i++) {
        #ifdef DT_SETBLANKSBYTYPE
			const Column& col = _table->getColumns().getColumnByIndex(i);
			switch (col.getType()) {
				case ctypeInt: _data[i] = col.def; break;
				case ctypeInt64: 
					_data[i]=(void*)new __int64;
					if (col.def) {
						*(__int64*)_data[i]=*(__int64*)col.def;
					} else {
						*(__int64*)_data[i]=0;
					}
					break;
				case ctypeString:
					if (col.def) {
						_data[i] = (void*)new char [strlen((char *)col.def)+1];
		                _data[i] = (void*)strcpy((char *)_data[i] , (char *)col.def);
					} else _data[i]=0;
					break;
/*				case DT_CT_STRING:
               if (tb->cols.cols[i].def)
                 {data[i]=(void*)new string((char *)tb->cols.cols[i].def);
                 }
                 else data[i]=0;
               break;*/
				case ctypeBin:
					if (col.def) {
						TypeBin * bin = (TypeBin*) col.def;
						_data[i]=(void*)new char [4+bin->size];
						memcpy((void*)_data[i] , &bin->size , 4);
						if (bin->buff && bin->size) 
							memcpy((char *)_data[i]+4 , bin->buff , bin->size);
					} else _data[i]=0;
					break;
			}
        #else
			_data[i]=0;
        #endif
		}
		return 1;
    }

    int DataRow::freeData() {
		LockerCS lock (_cs);
	    if (_size) {/*cout << "f" ;*/
			for (int i = 0 ; i < _size ; i++) {
				switch (_table->getColumns().getColumnByIndex(i)) {
					case ctypeInt64:
						if (_data[i]) delete _data[i];
						break;
					case ctypeString:
						if (_data[i]) delete [] _data[i];
						break;
					/*case DT_CT_STRING:
               if (data[i]) delete data[i];
               break;*/
					case ctypeBin:
						if (_data[i]) delete [] _data[i];
						break;
				}
			}
			delete [] _data;
			_size=0;
		}
	    return 1;
    }

    const DataEntry DataRow::get(tColId id) {
		LockerCS lock(_cs);
	    _table->resetError();
		return getByIndex(_table->getColumns().colIndex(id));
	}
	const DataEntry DataRow::getByIndex (unsigned int colIndex); // Pobiera wartosc kolumny
		if (! this->hasColumnData()) {
			_table->setError(errNoColumn);
			return 0;
		}
		const Column& col = this->_table->getColumns().getColumnByIndex(colIndex);
		DataEntry r = _data[colIndex];
		if (col.getType() == ctypeString && r == 0) {
			r = (DataEntry)"";
		}
		return r;
    }


    bool DataRow::set (tColId id , DataEntry val) {
		LockerCS lock(_cs);
	    _table->resetError();
		return this->setByIndex(_table->getColumns().colIndex(id), val);
	}

	bool DataRow::setByIndex (unsigned int colIndex, DataEntry val); // ustawia wartosc kolumny
		if (! this->hasColumnData()) {
			_table->setError(errNoColumn);
			return false;
		}
		const Column& col = this->_table->getColumns().getColumnByIndex(colIndex);

		switch (col.getType()) {
			case ctypeInt64:
				if (!_data[i]) _data[i]=(void*)new __int64;
				if (val) *((__int64*)_data[i]) = *((__int64*)val);
				break;
			case ctypeString: {
				size_t valLen = strlen((char *)val);
				size_t curLen = strlen((char *)_data[i]);
				if (_data[i] && (valLen > curLen || valLen < curLen/2)) {
					delete [] _data[i];
					_data[i]=0;
				}
				if (!_data[i]) {
					_data[i] = new char [valLen + 1];
				}
				_data[i] = (void*)strcpy((char *)_data[i] , (char *)val);
				break;}
			/*case DT_CT_STRING:
				if (!data[i]) data[i]=(void*)new string(*((string *)val));
				else *(string *)data[i]=*((string *)val);
				break;*/
			case ctypeBin: {
				TypeBin * bin = (TypeBin *)val;
				TypeBin * cur = (TypeBin *)_data[i];
				if (_data[i] && (bin->size > cur->size || bin->size < cur->size/2)  < ) {
					delete [] _data[i];
					_data[i]=0;
				}
				if (_data[i] == 0) {
					_data[i] = new char [4 + bin->size];
				}
				memcpy(data[i] , &bin->size , 4);
				if (bin->size && bin->buff) {
					memcpy((char *)_data[i] + 4 , bin->buff , bin->size);
				}
				break;}
			default:
				_data[i] = val;

      }
      return true;
    }



} }