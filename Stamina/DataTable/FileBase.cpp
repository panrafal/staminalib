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
#include "FileBase.h"

namespace Stamina { namespace DT {

	FileBase::FileBase() {
		_fcols.setLoader(true);
		_opened = fileClosed;
		_writeFailed = false;
		_headerLoaded = false;
	}

	FileBase::FileBase(DataTable * table) {
		_fcols.setLoader(true); 
		_opened = fileClosed;
		assign(table);
	}

	FileBase::~FileBase() {
	}

	void FileBase::assign(DataTable * table) {
		this->_table = table;
		_headerLoaded = false;
		//_fcols.table=t;
		this->reset();
	}

	void FileBase::reset() {
		_headerLoaded = false;
	}

    enResult FileBase::load (const std::string& fn, bool loadColumns) {
		if (!fn.empty())
			_fileName = fn;
		if (!_table) return errNotInitialized;
		if (_fileName.empty() || _access(_fileName.c_str(), 0))
			return errFileNotFound;
		_table->clearRows();
		_fcols.clear();

		try {
			this->open(_fileName , fileRead);
			this->readDescriptor();
			if (loadColumns) {
				this->_table->mergeColumns(_fcols);
			}
			this->readRows();
		} catch (DTException e) {
			close();
			return e.errorCode;
		}
		close();
		return success;
    }


    enResult FileBase::save (const std::string& fn)
    {
		if (!fn.empty())
			_fileName = fn;
		if (!_table) return errNotInitialized;
		if (_fileName.empty())
			return errFileNotFound;

		_fcols = _table->getColumns();
		try {
			this->open(_fileName.c_str() , fileWrite);
			LockerDT lock(_table, allRows);
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


    enResult FileBase::append (const std::string& fn) {
		if (!fn.empty())
			_fileName = fn;
		if (!_table) return errNotInitialized;
		if (_fileName.empty() || _access(_fileName.c_str(), 0))
			return errFileNotFound;
		
		_fcols.clear();
	    
		bool first = (_access(_fileName.c_str() , 0) !=0); // tworzymy plik
		try {
			this->open(_fileName , fileAppend);
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
				_table->getRow(i).setId(_table->getNewRowId());
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