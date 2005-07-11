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
		_flag = rflagNone;
		_size=0;
		_table = table;
		if (allocate) {
			allocData();
		}
    }

    DataRow::~DataRow () {
		freeData(); 
	}

    bool DataRow::allocData() {
		LockerCS lock (_cs);
        this->freeData();
		_size = _table->getColumns().getColCount();
		if (!_size) {
			return false;
		}

		this->_data = new DataEntry [_size];
	    for (unsigned int i = 0 ; i < _size ; i++) {
        #ifdef DT_SETBLANKSBYTYPE
			const Column& col = _table->getColumns().getColumnByIndex(i);
			if (col.isStaticType()) {
				_data[i] = col.getDefValue();
			} else {
				_data[i] = 0;
			}
/*
			switch (col.getType()) {
				// teraz ustawiamy tylko dane "statyczne"
				case ctypeInt: _data[i] = col.getDefValue(); break;
				default: _data[i] = 0; break;
					/*
				case ctypeInt64: 
					_data[i]=(void*)new __int64;
					if (col.getDefValue()) {
						*(__int64*)_data[i]=*(__int64*)col.getDefValue();
					} else {
						*(__int64*)_data[i]=0;
					}
					break;
				case ctypeString:
					if (col.getDefValue()) {
						_data[i] = (void*)new char [strlen((char *)col.getDefValue())+1];
		                _data[i] = (void*)strcpy((char *)_data[i] , (char *)col.getDefValue());
					} else _data[i]=0;
					break;
				case ctypeBin:
					if (col.getDefValue()) {
						TypeBin * bin = (TypeBin*) col.getDefValue();
						_data[i]=(void*)new char [4+bin->size];
						memcpy((void*)_data[i] , &bin->size , 4);
						if (bin->buff && bin->size) 
							memcpy((char *)_data[i]+4 , bin->buff , bin->size);
					} else _data[i]=0;
					break;
					
			}
			*/
        #else
			_data[i]=0;
        #endif
		}
		return true;
    }

    bool DataRow::freeData() {
		LockerCS lock (_cs);
	    if (_size) {/*cout << "f" ;*/
			for (unsigned int i = 0 ; i < _size ; i++) {
				switch (_table->getColumns().getColumnByIndex(i).getType()) {
					case ctypeInt64:
						if (_data[i]) delete (__int64*)_data[i];
						break;
					case ctypeString:
						if (_data[i]) delete [] (char*)_data[i];
						break;
					case ctypeWideString:
						if (_data[i]) delete [] (wchar_t*)_data[i];
						break;
					/*case DT_CT_STRING:
               if (data[i]) delete data[i];
               break;*/
					case ctypeBin:
						if (_data[i]) delete [] (char*)_data[i];
						break;
				}
			}
			delete [] _data;
			_size=0;
		}
	    return true;
    }

    const DataEntry DataRow::get(tColId id) {
		LockerCS lock(_cs);
		return getByIndex(_table->getColumns().colIndex(id));
	}
	const DataEntry DataRow::getByIndex (unsigned int colIndex) { // Pobiera wartosc kolumny
		if (! this->hasColumnData(colIndex)) {
			throw DTException(errNoColumn);
			return 0;
		}
		const Column& col = this->_table->getColumns().getColumnByIndex(colIndex);
		DataEntry r = _data[colIndex];
		if (r == 0 && col.isStaticType() == false) {
			r = col.getDefValue();
		}
/*		if (col.getType() == ctypeString && r == 0) {
			r = (DataEntry)"";
		}*/
		return r;
    }


    bool DataRow::set (tColId id , DataEntry val, bool dropDefault) {
		LockerCS lock(_cs);
		return this->setByIndex(_table->getColumns().colIndex(id), val, dropDefault);
	}

	bool DataRow::setByIndex (unsigned int colIndex, DataEntry val, bool dropDefault) { // ustawia wartosc kolumny
		if (! this->hasColumnData(colIndex)) {
			throw DTException(errNoColumn);
			return false;
		}
		const Column& col = this->_table->getColumns().getColumnByIndex(colIndex);

		switch (col.getType()) {
			case ctypeInt64:
				if (val == 0 || (dropDefault && col.getDefValue() && (*(__int64*)col.getDefValue()) == *(__int64*)val)) {
					if (_data[colIndex] != 0) delete _data[colIndex];
					_data[colIndex] = 0;
				} else {
					if (!_data[colIndex]) _data[colIndex] = (void*)new __int64;
					*((__int64*)_data[colIndex]) = *((__int64*)val);
				}
				break;
			case ctypeString: {
				if (val == 0 || (dropDefault && col.getDefValue() && (strcmp((char*)col.getDefValue(), (char*)val) == 0))) {
					if (_data[colIndex] != 0) delete [] (char*)_data[colIndex];
					_data[colIndex] = 0;
				} else {
					size_t valLen = strlen((char *)val);
					size_t curLen = _data[colIndex] ? strlen((char *)_data[colIndex]): 0;
					if (_data[colIndex] && (valLen > curLen || valLen < curLen/2)) {
						delete [] (char*)_data[colIndex];
						_data[colIndex]=0;
					}
					if (!_data[colIndex]) {
						_data[colIndex] = new char [valLen + 1];
					}
					_data[colIndex] = (void*)strcpy((char *)_data[colIndex] , (char *)val);
				}
				break;}

			case ctypeWideString: {
				if (val == 0 || (dropDefault && col.getDefValue() && (wcscmp((wchar_t*)col.getDefValue(), (wchar_t*)val) == 0))) {
					if (_data[colIndex] != 0) delete [] (wchar_t*)_data[colIndex];
					_data[colIndex] = 0;
				} else {
					size_t valLen = wcslen((wchar_t *)val);
					size_t curLen = _data[colIndex] ? wcslen((wchar_t *)_data[colIndex]): 0;
					if (_data[colIndex] && (valLen > curLen || valLen < curLen/2)) {
						delete [] (wchar_t*)_data[colIndex];
						_data[colIndex]=0;
					}
					if (!_data[colIndex]) {
						_data[colIndex] = new wchar_t [valLen + 1];
					}
					_data[colIndex] = (void*)wcscpy((wchar_t *)_data[colIndex] , (wchar_t *)val);
				}
				break;}
							  
							  /*case DT_CT_STRING:
				if (!data[i]) data[i]=(void*)new string(*((string *)val));
				else *(string *)data[i]=*((string *)val);
				break;*/
			case ctypeBin: {
				if (val == 0 || (dropDefault && col.getDefValue() && (*(TypeBin*)col.getDefValue()) == *(TypeBin*)val)) {
					if (_data[colIndex] != 0) delete [] (char*)_data[colIndex];
					_data[colIndex] = 0;
				} else {
					TypeBin * bin = (TypeBin *)val;
					TypeBin * cur = (TypeBin *)_data[colIndex];
					if (cur && (bin->size > cur->size || bin->size < cur->size/2)) {
						delete [] _data[colIndex];
						_data[colIndex] = 0;
					}
					if (_data[colIndex] == 0) {
						_data[colIndex] = new char [4 + 4 + bin->size];
						cur = (TypeBin*)_data[colIndex];
						cur->buff = cur + 1; // dane s¹ w pamiêci zaraz za struktur¹
					}
					cur->size = bin->size;
					if (bin->size && bin->buff) {
						memcpy(cur->buff, bin->buff, bin->size);
					}
				}
				break;}
			default:
				_data[colIndex] = val;

      }
      return true;
    }



} }