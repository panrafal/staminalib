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

	DataTable::DataTable() {
		//mode=0;
		//notypecheck=0;
//      filecols.table=this;
		//cols.table=this;
		//index_eq_id=1;
		_lastId = rowIdMin;
		_changed = false;
		_error = 0;
		_cxor_key = "";
    }
    DataTable::~DataTable() {
		clearRows();
    }

    void DataTable::clearRows() {
        LockerCS lock(_cs);
        _changed = true;
		for (tRows::iterator it = _rows.begin(); it != _rows.end() ; it++) {
            delete *it;
        }
        _rows.clear();
    }

    tRowId DataTable::addRow(tRowId id) {
		return this->insertRow(-1, id);
    }

    unsigned int DataTable::insertRow(unsigned int rowPos , tRowId id) {
        LockerCS lock(_cs);
        _changed = true;
		DataRow* row = new DataRow(this,1);
		if (rowPos < 0) {
			rowPos = _rows.size();
			_rows.push_back(row);
		} else {
			rowPos = getRowPos(rowPos);
			_rows.insert(_rows.begin() + rowPos , row);
		}
        row->id = id > 0 ? unflagId(id) : getNewRowId();
        return row->id;
    }

    int DataTable::deleteRow(tRowID row) {
        LockerCS lock(_cs);
        _changed = true;
        row = (tRowId) getRowPos(row);
		if (row == rowNotFound || row >= _rows.size()) {
			return false;
		}
        delete _rows[row];
        _rows.erase(rows.begin() + _row);
        return true;
    }


    DataEntry DataTable::get(tRowId row , tColId id){
        LockerCS lock(_cs);
        row = (tRowId) getRowPos(row);
        if (row == rowNotFound || row >= _rows.size()) {
			this->setError(errNoRow);
            return 0;
		}
        try {
/*            if (id == DT_C_ID) ret=(DataEntry)rows[row]->id;
            else ret=(rows[row])->get(id);
            return ret;*/
			return _rows[row]->get(id);
        } catch (...) {
			this->setError(errNoRow);
		}
        return 0;
    }

    int DataTable::set(tRowId row , tColId id , DataEntry val){
        LockerCS lock(_cs);
        _changed = true;
        row = (tRowId) getRowPos(row);
		if (row == rowNotFound || row >= _rows.size()) {
			setError(errNoRow);
            return false;
		}
        try {
            _rows[row]->set(id, val);
        } catch (...) {
			setError(errNoRow);
		}
        return 0;
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
				setError(errNoRow);
                _cs.unlock();
                return;
			}
			_rows[row]->lock();
            CS.unlock();
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
				setError(errNoRow);
                _cs.unlock();
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
				setError(errNoRow);
                return false;
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


	tRowId DataTable::findRow(unsigned int startPos, int argCount, ...) {
	    va_list marker;
	    if (argCount == 0) return rowNotFound;
        
		this->lock(allRows);

		int found = -1;

		for (int i=this->getRowPos((tRowId) startPos); i < this->_dt.getrowcount(); i++) {

			found = i;
			int args = argCount;
			va_start( marker, argCount );
			do {
				Find* find = va_arg( marker, Find*);
				if (!find) break;
				// sprawdzamy...

				Value value(find->value.type);
				if (value.type == ctypeString) {
					// Pozniej trzeba to zwolnic
					value.buffSize = -1;
				}
				this->getValue(i, find->col, value);
				int cmp = 0;
				switch (value.type) {
					case ctypeString:
						cmp = stricmp(value.vCChar, find->value.vCChar);
						break;
					case ctypeInt64:
						if (value.vInt64 > find->value.vInt64)
							cmp = 1;
						else if (value.vInt64 < find->value.vInt64)
							cmp = -1;
						break;
					default:
						if (value.vInt > find->value.vInt)
							cmp = 1;
						else if (value.vInt < find->value.vInt)
							cmp = -1;
						break;
				}

				if (value.type == ctypeString) {
					free(value.vChar);
				}

				switch (find->operation) {
					case Find::eq:
						if (cmp != 0) found = -1;
						break;
					case Find::neq:
						if (cmp == 0) found = -1;
						break;
					case Find::gt:
						if (cmp <= 0) found = -1;
						break;
					case Find::gteq:
						if (cmp < 0) found = -1;
						break;
					case Find::lt:
						if (cmp >= 0) found = -1;
						break;
					case Find::lteq:
						if (cmp > 0) found = -1;
						break;
				}
			} while (--args != 0 && found == i);
			va_end( marker );

			if (found == i)
				break;

		}

		this->unlock(allRows);
        
		return this->getRowId(found);
	}



	bool DataTable::getValue(tRowId row , tColId col , Value& value) const {
		LockerCS lock(_cs);
		row = getRowPos(row);
		if (value->type == ctypeString && value->vChar)
			value->vChar[0] = 0;
		if (row >= this->_rows.size()) row = rowNotFound;
		if (row == rowNotFound) return false;
		LockerCS rowLock(_rows[row]->getCS());

		const Column& column = this->_cols.getColumn(col);
		if (column.type == ctypeUnknown) return false;

		// W type znajduje siê typ czytanej kolumny
		if (value->type == ctypeUnknown) value->type = column.getType();
		// nie mamy gdzie zapisaæ konwersji...
		if (value->type == ctypeString && column.getType() != ctypeString && !value->vChar && value->buffSize==0) return false;

		DataEntry val = this->get(row, col);
		switch (value->type) {
		case ctypeInt: 
			switch (column.getType()) {
			case ctypeInt: value->vInt = (int)val; return true;
			case ctypeInt64: value->vInt = val ? *(__int64*)val : 0; return true;
			case ctypeString: value->vInt = val ? atoi((char*)val) : 0; return true;
			};
			return false;
		case ctypeInt64: 
			switch (column.getType()) {
			case ctypeInt: value->vInt64 = (int)val; return true;
			case ctype64: value->vInt64 = val? *(__int64*)val : 0; return true;
			case ctypeString: value->vInt64 = val ? _atoi64((char*)val) : 0; return true;
			};
			return false;
		case ctypeString: 
			if (column.getType() == ctypeString) {
				const char* ch = val ? (char*)val : "";
				if (value->vChar && value->buffSize != -1 && value->buffSize != 0) {
					strncpy(value->vChar , ch , value->buffSize);
				} else if (value->buffSize == -1) {
					value->vChar = strdup(ch);
				} else {
					value->vCChar = ch;
				}
				return true;
			} else {
				if (!value->vChar && value->buffSize == -1) {
					value->vChar = (char*)malloc(32);
					value->buffSize = 31;
				}
				// pozosta³e mo¿liwoœci to dostarczony dzia³aj¹cy bufor i nic wiêcej... 
				switch (column.getType()) {
				case ctypeInt: itoa(val , value->vChar , 10); return true;
				case ctype64: _i64toa(val ? *(__int64*)val : 0 , value->vChar , 10); return true;
				default:
					value->vChar[0] = 0;
				};
			}
			return false;
		}

		return false; 
	}



	bool DataTable::setValue(tRowId row , tColId col , const Value& value) {
		LockerCS lock(_cs);
		row = getRowPos(row);
		if (row == rowNotFound) return false;

		const Column& column = this->_cols.getColumn(col);
		if (column.type == ctypeUnknown) return false;

		if (value->type == ctypeUnknown) value->type = column.getType();

		DataEntry val = 0;
		__int64 val64;
		char buff [32];
		switch (column.getType()) {
		case ctypeInt: 
			switch (value->type) {
			case ctypeInt: val = value->vInt; break;
			case ctypeString: val = atoi(value->vCChar); break;
			case ctype64: val = value->vInt64; break;
			default: return false;
			};
			break;
		case ctypeInt64: {
			switch (value->type) {
			case ctypeInt: val64 = value->vInt; break;
			case ctypeString: val64=_atoi64(value->vCChar); break;
			case ctype64: val64=value->vInt64; break;
			default: return false;
			}; 
			val = &val64;
			break;}
		case ctypeString: {
			switch (value->type) {
			case ctypeInt: itoa(value->vInt , buff , 10); val = buff; break;
			case ctypeString: val = value->vCChar; break;
			case ctype64: _i64toa(value->vInt64 , buff , 10); val = buff; break;
			default: return false;
			};
			break;}
		default: return false;
		}

		this->set(row,col,val); 
		return true;
	}




};};
