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
#include "FileBin.h"

namespace Stamina { namespace DT {

	FileBase::FileBase() {
		_fcols.setLoader(true);
		_opened = fileClosed;
		_writeFailed = false;
		_headerLoaded = false;
		_recreating = true;
	}

	FileBase::FileBase(DataTable& table) {
		_fcols.setLoader(true); 
		_opened = fileClosed;
		assign(table);
	}

	FileBase::~FileBase() {
	}

	void FileBase::assign(DataTable& table) {
		this->_table = &table;
		_headerLoaded = false;
		//_fcols.table=t;
		this->reset();
	}

	void FileBase::reset() {
		_headerLoaded = false;
	}

    enResult FileBase::load (const std::string& fn, enFileOperation operation) {
		if (!_table) return errNotInitialized;

		_table->clearRows();

		try {
			this->open(fn , fileRead);
			this->readDescriptor();
			if (operation & loadColumns) {
				this->mergeLoadedColumns();
			}
			/*TODO: przywracanie backupów*/
			this->readRows(false);
		} catch (DTException e) {
			close();
			return e.errorCode;
		}
		close();
		return success;
    }


    enResult FileBase::save (const std::string& fn, enFileOperation operation)
    {
		if (!_table) return errNotInitialized;

		this->setColumns(_table->getColumns());

		try {
			this->open(fn , fileWrite);
			LockerDT lock(_table, allRows);
			if (operation & saveOldCryptVersion && this->getClass() >= FileBin::staticClassInfo()) {
				this->castObject<FileBin>()->setOldCryptVersion();
			}
			this->writeHeader();
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


    enResult FileBase::append (const std::string& fn, enFileOperation operation) {
		if (!_table) return errNotInitialized;
		
		_fcols.clear();
	    
		try {
			this->open(fn , fileAppend);
			if (this->isCreatingNewFile()) {
				_fcols = _table->getColumns();
				//_table->lastId = DT_ROWID_MIN;
				this->seekToBeginning();
				this->writeHeader();
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