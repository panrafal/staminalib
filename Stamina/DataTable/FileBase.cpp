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

	FileBase::FileBase() {
		_fcols.setLoader(true);
		_opened = false;
		_writeFailed = false;
	}

	FileBase::FileBase(DataTable * table) {
		_fcols.setLoader(true); 
		_opened = false;
		assign(table);
	}

	FileBase::~FileBase() {
	}

	void FileBase::assign(DataTable * table) {
		this->_table = table;
		//_fcols.table=t;
	}

    enResult FileBase::load (const std::string& fn, bool loadColumns) {
		if (!fn.empty())
			fileName = fn;
		if (!_table) return errNotInitialized;
		if (fileName.empty() || _access(fileName, 0))
			return errFileNotFound;
		_table->clearRows();
		_fcols.clear();

		try {
			this->open(fileName , fileRead);
			this->readDescriptor();
			if (loadColumns) {
				this->_table->mergeColumns(&fcols);
			}
			this->readRows();
		} catch (DTException e) {
			close();
			return e.errorCode;
		}
		close();
		return success;
    }


    int FileBase::save (const std::string& fn)
    {
		if (!fn.empty())
			fileName = fn;
		if (!_table) return errNotInitialized;
		if (fileName.empty())
			return errFileNotFound;

		_fcols = _table->getColumns();
		try {
			this->open(fileName , fileWrite);
			LockerDT lock(this, allRows);
		    this->writeDescriptor();
			for (unsigned int i=0; i < _table->getRowCount() ; i ++) {
				if (_table->getRow(i).hasFlag(rflagDontSave) == false) {
					this->writeRow(i);
				}
			}
		} catch (DTException e) {
			this->close();
			return e.errorCode;
		}
		this->close();
		return success;
    }


    int FileBase::append (const std::string& fn) {
		if (!fn.empty())
			fileName = fn;
		if (!_table) return errNotInitialized;
		if (fileName.empty() || _access(fileName, 0))
			return errFileNotFound;
		
		_fcols.clear();
	    
		bool first = (_access(fn , 0) !=0); // tworzymy plik
		try {
			this->open(fn , fileAppend);
			this->seekToBeginning();
		    if (first) {
				_fcols = _table->getColumns();
				//_table->lastId = DT_ROWID_MIN;
				this->writeDescriptor();
			} else {
				this->readDescriptor();
			}
			this->seekToEnd();
			for (unsigned int i=0; i < _table->getRowCount() ; i ++) {
				_table->getRow(i).id = _table->getNewRowId();
				this->writeRow(i);
			}
		} catch (DTException e) {
			this->close();
			return e.errorCode;
		}
	    this->close();
		return success;
	}


} }