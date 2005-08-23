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
#include "Find.h"

namespace Stamina { namespace DT {

	DataTable::DataTable() {
		//mode=0;
		//notypecheck=0;
//      filecols.table=this;
		//cols.table=this;
		//index_eq_id=1;
		_lastId = rowIdMin;
		_changed = false;
		//_error = success;
		_xor1_key = (unsigned char*)"";
    }
    DataTable::~DataTable() {
		clearRows();
    }

    void DataTable::clearRows() {
        LockerCS lock(_cs);
        _changed = true;
        _rows.clear();
    }

	oDataRow DataTable::addRow(tRowId id) {
		return this->insertRow(rowNotFound, id);
    }

    oDataRow DataTable::insertRow(unsigned int rowPos , tRowId id) {
        LockerCS lock(_cs);

		if (id != rowNotFound) {
			if (unflagId(id) > rowIdMax || unflagId(id) < rowIdMin) {
				return oDataRow();
			}
			id = flagId(id);
			if (getRowPos(id) != rowNotFound) {
				//throw DTException(errNoRow);
				return oDataRow();
			}
			if (_lastId < unflagId(id)) {
				_lastId = unflagId(id);
			}
		}

		_changed = true;
		oDataRow row = new DataRow(this);
		if (rowPos == -1) {
			rowPos = _rows.size();
			_rows.push_back(row);
		} else {
			rowPos = getRowPos(rowPos);
			_rows.insert(_rows.begin() + rowPos, row);
		}
		row->setId(id != rowNotFound ? unflagId(id) : getNewRowId());
        return row;
    }

    bool DataTable::deleteRow(tRowId row) {
        Locker lock(_cs);
        _changed = true;
        row = (tRowId) getRowPos(row);
		if (row == rowNotFound || row >= _rows.size()) {
			return false;
		}
        _rows.erase(_rows.begin() + row);
        return true;
    }

	void DataTable::swapRows(tRowId a, tRowId b) {
        Locker lock(_cs);
        _changed = true;
		a = getRowPos(a);
		b = getRowPos(a);
		if (a == rowNotFound || b == rowNotFound) return;
		oDataRow temp = _rows[a];
		_rows[a] = _rows[b];
		_rows[b] = temp;
	}


    void DataTable::lock(tRowId row) {
        if (row == allRows) {
            this->_cs.lock();
            for (size_t i=0; i < _rows.size(); i++)
                _rows[i]->lock();
            return;
        } else {
            _cs.lock();
            row = getRowPos(row);
			if (row == rowNotFound || row >= _rows.size()) {
                _cs.unlock();
				throw DTException(errNoRow);
                return;
			}
			_rows[row]->lock();
            _cs.unlock();
            return;//rows[row]->CS.getLockCount();
        }
    }

    void DataTable::unlock(tRowId row){
		if (row == allRows) {
            for (size_t i=0; i < _rows.size(); i++)
                _rows[i]->unlock();
            _cs.unlock();
            return;
        } else {
            _cs.lock();
			row = getRowPos(row);
			if (row == rowNotFound || row >= _rows.size()) {
                _cs.unlock();
				throw DTException(errNoRow);
                return;
			}
			_rows[row]->unlock();
            _cs.unlock();
            return;
        }
    }

    bool DataTable::canAccess(tRowId row){
        if (row == allRows) {
			if (!this->_cs.canAccess()) return false;
            LockerCS lock(_cs);
            for (size_t i = 0; i < _rows.size(); i++)
				if (!_rows[i]->canAccess())
					return false;
        } else {
			if (!this->_cs.canAccess()) return false;
            LockerCS lock(_cs);
			row = getRowPos(row);
			if (row == rowNotFound || row >= _rows.size()) {
				throw DTException(errNoRow);
			}
			if (_rows[row]->canAccess() == false)
				return false;
        }
        return true;
    }



/*
    void DataTable::setindexes() {
        CS.lock();
        changed = true;
        for (unsigned int i=0; i<rows.size(); i++) {
            rows[i]->index=i;
        }
        CS.unlock();
    }
*/



/*
    int DataTable::findby(DataEntry val , int id , bool csens) {
        if (id==-1) {
            for (int i = 0; i<cols.getcolcount(); i++)
            if (cols.getcolflag(i) & DT_CT_KEY) {id = i;break;}
        } else id = cols.colindex(id);
        if (id==-1) return -1;
        int type = cols.getcoltype(id);
        char * ch;
        CS.lock();
        int ret = -1;
        for (unsigned int i = 0; i<rows.size(); i++) {
            switch (type) {
            case DT_CT_INT: if (rows[i]->getbyindex(id) == val) {ret=i;goto end;} break;
            case DT_CT_64: if (*(__int64*)rows[i]->getbyindex(id) == *(__int64*)val) {ret=i;goto end;} break;
            case DT_CT_PCHAR:
                ch = (char*)rows[i]->getbyindex(id);
                if (!ch) continue;
                if (csens?!strcmp(ch,(char*)val) : !stricmp(ch,(char*)val)) {ret=i;goto end;} break;
            default: goto end;
            }
        }
        end:
        CS.unlock();
        return ret;
    }
*/


	oDataRow DataTable::findRow(unsigned int startPos, int argCount, ...) {
		va_list list;
		va_start(list, argCount);
		oDataRow result = findRow(startPos, argCount, list);
		va_end(list);
		return result;
	}

	oDataRow DataTable::findRow(unsigned int startPos, int argCount, va_list list) {
	    if (argCount == 0) return oDataRow();
        
		LockerDT(this, allRows);

		unsigned int found = rowNotFound;
		startPos = this->getRowPos((tRowId) startPos);
		if (startPos == rowNotFound) return oDataRow();

		for (tRows::iterator it = _rows.begin() + startPos; it != _rows.end(); ++it) {
			int args = argCount;

			oDataRow& row = *it;
			bool found = true;
	
			va_list marker = list;

			do {
				Find* find = va_arg( marker, Find*);
				if (!find) break;
				// sprawdzamy...

				const Column* col = this->getColumn( find->col );
				if (col->isUndefined()) {
					found = false;
					break;
				}

				int cmp = 0;
				switch (find->value->getType()) {
					case ctypeString:
						cmp = find->value->castStaticObject<Value_string>()->cmp( col->getString(row, false) );
						break;
					case ctypeInt:
						cmp = find->value->castStaticObject<Value_int>()->cmp( col->getInt(row) );
						break;
					case ctypeInt64:
						cmp = find->value->castStaticObject<Value_int64>()->cmp( col->getInt64(row) );
						break;
					case ctypeDouble:
						cmp = find->value->castStaticObject<Value_double>()->cmp( col->getDouble(row) );
						break;
					case ctypeBin:
						cmp = find->value->castStaticObject<Value_bin>()->cmp( col->getBin(row, false) );
						break;
					default:
						found = false;
						break;
				}

				switch (find->operation) {
					case Find::eq:
						if (cmp != 0) found = false;
						break;
					case Find::neq:
						if (cmp == 0) found = false;
						break;
					case Find::gt:
						if (cmp <= 0) found = false;
						break;
					case Find::gteq:
						if (cmp < 0) found = false;
						break;
					case Find::lt:
						if (cmp >= 0) found = false;
						break;
					case Find::lteq:
						if (cmp > 0) found = false;
						break;
				}
			} while (--args != 0 && found);
			//va_end( marker );

			if (found == true)
				return row;

		}

		return oDataRow();
	}


/*
	bool DataTable::getValue(tRowId row , tColId col , Value& value) {
		LockerCS lock(_cs);
		row = getRowPos(row);
		if (row >= this->_rows.size()) row = rowNotFound;
		if (row == rowNotFound) return false;
		LockerCS rowLock(_rows[row]->getCS());

		const Column& column = this->_cols.getColumn(col);
		if (column.getType() == ctypeUnknown) return false;

		if (value.getType() == ctypeUnknown) value.type = (short)column.getType();

		DataEntry val = this->get(row, col);
		switch (value.getType()) {
		case ctypeInt: 
			switch (column.getType()) {
			case ctypeInt: value.vInt = (int)val; return true;
			case ctypeInt64: value.vInt = (int) (val ? *(__int64*)val : 0); return true;
			case ctypeString: value.vInt = val ? atoi((char*)val) : 0; return true;
			case ctypeWideString: value.vInt = val ? _wtoi((wchar_t*)val) : 0; return true;
			};
			return false;
		case ctypeInt64: 
			switch (column.getType()) {
			case ctypeInt: value.vInt64 = (int)val; return true;
			case ctype64: value.vInt64 = val? *(__int64*)val : 0; return true;
			case ctypeString: value.vInt64 = val ? _atoi64((char*)val) : 0; return true;
			case ctypeWideString: value.vInt64 = val ? _wtoi64((wchar_t*)val) : 0; return true;
			};
			return false;
		case ctypeString: 
			if (column.getType() == ctypeString) {
				const char* ch = val ? (char*)val : "";
				if (value.vChar && value.buffSize != -1 && value.buffSize != 0) {
					strncpy(value.vChar , ch , value.buffSize);
					value.vChar[value.buffSize - 1] = 0;
				} else if (value.buffSize == -1) {
					value.vChar = strdup(ch);
				} else {
					if (value.vChar && value.buffSize == 0) {
						value.vCChar = ch;
					} else {
						value.vChar = 0;
						value.buffSize = strlen(ch) + 1;
					}
				}
				return true;
			} else if (column.getType() == ctypeWideString) {
				const wchar_t* wch = val ? (wchar_t*)val : L"";
				if (value.vChar && value.buffSize != -1 && value.buffSize != 0) {
					WideCharToMultiByte(CP_ACP, 0, wch, -1, value.vChar, value.buffSize, 0, 0);
					value.vChar[value.buffSize - 1] = 0;
				} else if (value.buffSize == -1) {
					size_t len = wcslen(wch);
					value.vChar = (char*) malloc(len + 1);
					WideCharToMultiByte(CP_ACP, 0, wch, len + 1, value.vChar, len + 1, 0, 0);
					value.vChar[len] = 0;
				} else {
					if (value.vChar && value.buffSize == 0) {
						return false;
					} else {
						value.vChar = 0;
						value.buffSize = wcslen(wch) + 1;
					}
				}
				return true;
			} else {
				if (!value.vChar && value.buffSize == -1) {
					value.vChar = (char*)malloc(31);
					//value.buffSize = 32;
				}
				if (value.vChar == 0) {
					value.buffSize = 32;
					return false;
				}
				// pozosta³e mo¿liwoœci to dostarczony dzia³aj¹cy bufor i nic wiêcej... 
				switch (column.getType()) {
				case ctypeInt: itoa((int)val , value.vChar , 10); return true;
				case ctype64: _i64toa(val ? *(__int64*)val : 0 , value.vChar , 10); return true;
				default:
					value.vChar[0] = 0;
					return false;
				};
				//return true;
			}
//			return false; // string

		case ctypeWideString: 
			if (column.getType() == ctypeWideString) {
				const wchar_t* wch = val ? (wchar_t*)val : L"";
				if (value.vWChar && value.buffSize != -1 && value.buffSize != 0) {
					wcsncpy(value.vWChar, wch, value.buffSize / 2);
					value.vWChar[value.buffSize / 2 - 1] = 0;
				} else if (value.buffSize == -1) {
					value.vWChar = wcsdup(wch);
				} else {
					if (value.vWChar && value.buffSize == 0) {
						value.vCWChar = wch;
					} else {
						value.vWChar = 0;
						value.buffSize = wcslen(wch) * 2 + 2;
					}
				}
				return true;
			} else if (column.getType() == ctypeString) {
				const char* ch = val ? (char*)val : "";
				if (value.vWChar && value.buffSize != -1 && value.buffSize != 0) {
					MultiByteToWideChar(CP_ACP, 0, ch, -1, value.vWChar, value.buffSize / 2);
					value.vWChar[value.buffSize/2 - 1] = 0;
				} else if (value.buffSize == -1) {
					size_t len = strlen(ch);
					value.vWChar = (wchar_t*)malloc((len+1) * 2);
					MultiByteToWideChar(CP_ACP, 0, ch, len + 1 , value.vWChar, len + 1);
					value.vWChar[len] = 0;
				} else {
					if (value.vWChar && value.buffSize == 0) {
						return false;
					} else {
						value.vWChar = 0;
						value.buffSize = strlen(ch)*2 + 2;
					}
				}
				return true;
			} else {
				if (!value.vWChar && value.buffSize == -1) {
					value.vWChar = (wchar_t*)malloc(64);
					//value.buffSize = 32;
				}
				if (value.vWChar == 0) {
					value.buffSize = 64;
					return false;
				}
				// pozosta³e mo¿liwoœci to dostarczony dzia³aj¹cy bufor i nic wiêcej... 
				switch (column.getType()) {
				case ctypeInt: _itow((int)val , value.vWChar , 10); return true;
				case ctype64: _i64tow(val ? *(__int64*)val : 0 , value.vWChar , 10); return true;
				default:
					value.vWChar[0] = 0;
					return false;
				};
				//return true;
			}
//			return false; // string

		case ctypeBin:
			if (column.getType() == ctypeBin) {
				static TypeBin emptyBin;
				TypeBin* bin = val ? (TypeBin*) val : &emptyBin;
				if (value.vBin.buff == 0 && value.vBin.size == -1) {
					value.vBin.size = bin->size;
					value.vBin.buff = malloc(bin->size);
					memcpy(value.vBin.buff, bin->buff, bin->size);
				} else if (value.vBin.buff != 0 && value.vBin.size == 0) {
					value.vBin = *bin;
				} else if (value.vBin.buff == 0 && value.vBin.size == 0) {
					value.vBin.size = bin->size;
				} else if (value.vBin.buff != 0 && value.vBin.size != 0) {
					memcpy(value.vBin.buff, bin->buff, min(value.vBin.size, bin->size));
					value.vBin.size = bin->size;
				} else {
					return false;
				}
				return true;
			}
			return false;
		}

		return false; 
	}



	bool DataTable::setValue(tRowId row , tColId col , const Value& _value, bool dropDefault) {
		Locker lock(_cs);
		row = getRowPos(row);
		if (row == rowNotFound) return false;

		Value value = _value;

		const Column& column = this->_cols.getColumn(col);
		if (column.getType() == ctypeUnknown) return false;

		if (value.getType() == ctypeUnknown) value.type = (short)column.getType();

		DataEntry val = 0;
		__int64 val64;
		char buff [32];
		void* alloc = 0;
		switch (column.getType()) {
		case ctypeInt: 
			switch (value.getType()) {
			case ctypeInt: val = (DataEntry) value.vInt; break;
			case ctypeString: val = (DataEntry) chtoint(value.vCChar); break;
			case ctypeWideString: val = (DataEntry) chtoint(value.vCWChar); break;
			case ctype64: val = (DataEntry) value.vInt64; break;
			default: return false;
			};
			break;
		case ctypeInt64: {
			switch (value.getType()) {
			case ctypeInt: val64 = value.vInt; break;
			case ctypeString: val64 = chtoint64(value.vCChar); break;
			case ctypeWideString: val64 = chtoint64(value.vCWChar); break;
			case ctype64: val64 = value.vInt64; break;
			default: return false;
			}; 
			val = &val64;
			break;}
		case ctypeString: {
			switch (value.getType()) {
			case ctypeInt: itoa(value.vInt , buff , 10); val = buff; break;
			case ctypeString: val = value.vChar; break;
			case ctypeWideString: {
				size_t len = wcslen(value.vCWChar);
				alloc = malloc(len + 1);
				WideCharToMultiByte(CP_ACP, 0, value.vCWChar, len + 1 , (LPSTR)alloc, len + 1, 0, 0);
				val = alloc; 
				break;}
			case ctype64: _i64toa(value.vInt64 , buff , 10); val = buff; break;
			default: return false;
			};
			break;}
		case ctypeWideString: {
			switch (value.getType()) {
			case ctypeInt: _itow(value.vInt, (wchar_t*)buff , 10); val = buff; break;
			case ctypeWideString: val = value.vWChar; break;
			case ctypeString: {
				size_t len = strlen(value.vCChar);
				alloc = malloc((len+1)*2);
				MultiByteToWideChar(CP_ACP, 0, value.vCChar, len + 1 , (LPWSTR)alloc, len + 1);
				val = alloc; 
				break;}
			case ctype64: _i64tow(value.vInt64 , (wchar_t*)buff , 10); val = buff; break;
			default: return false;
			};
			break;}

		case ctypeBin: {
			if (value.getType() == ctypeBin) {
				val = &value.vBin;
			} else return false;
			break;}
		default: return false;
		}

		this->set(row,col,val,dropDefault); 
		if (alloc) free(alloc);
		return true;
	}

*/


};};
