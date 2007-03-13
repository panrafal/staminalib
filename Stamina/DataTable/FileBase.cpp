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

    enResult FileBase::load (const StringRef& fn, enFileOperation operation) {
		if (!_table) return errNotInitialized;
		int i = 0;
		do {
			
			_table->clearRows();
			try {
				this->open(fn , fileRead);
				this->readDescriptor();
				if (operation & loadColumns) {
					this->mergeLoadedColumns();
				}
				this->readRows(false);
				break;
			} catch (DTException& e) {
				close();
				_table->clearRows();
				if (_table->getInterface().empty() || (_table->getInterface()->handleFailedLoad(this, e, i) & iInterface::fail)) {
					return e.errorCode;
				}
	            i++;
			}
		} while (1);

		close();
		return success;
    }

    enResult FileBase::loadPartial (const StringRef& fn, unsigned int start, unsigned int count, unsigned int* seekPtr, enFileOperation operation) {
		if (!_table) return errNotInitialized;
		int i = 0;
		try {
			this->open(fn , fileRead);
			this->readDescriptor();
			if (operation & loadColumns) {
				this->mergeLoadedColumns();
			}

			if (seekPtr && *seekPtr && this->getClass().inheritsFrom(FileBin::staticClassInfo())) {
				dynamic_cast<FileBin*>(this)->setFilePosition(*seekPtr, FileBin::fromBeginning);
				this->findNextRow(true);
			}

			while (start -- > 0 && this->isFileFinished() == false) {
				// pomijamy kolejne
				if (this->skipRow() != errSuccess) break;
			}
			while (count -- > 0 && this->isFileFinished() == false) {
				tRowId row = _table->addRow();
				if (this->readRow(row) != success) {
					_table->deleteRow(row);
					break;
				}
			}

			if (seekPtr && this->getClass().inheritsFrom(FileBin::staticClassInfo())) {
				*seekPtr = dynamic_cast<FileBin*>(this)->getFilePosition();
			}

			
		} catch (DTException& e) {
			close();
			if (_table->getInterface().empty() || (_table->getInterface()->handleFailedLoad(this, e, i) & iInterface::fail)) {
				return e.errorCode;
			}
            i++;
		}

		close();
		return success;
    }

	void FileBase::readRows(bool skipFailed) {
        _table->clearRows();
        tRowId row;
        while (!isFileFinished())
        { 
			row = _table->addRow();
			if (skipFailed == false) {
				if (this->readRow(row) != success)
					_table->deleteRow(row);
			} else {
				while (1) {
					try {
						if (this->readRow(row) != success)
							_table->deleteRow(row);
					} catch (DTException e) {
						if (findNextRow() == true) {
							continue; // jeszcze raz readRow
						} else {
							_table->deleteRow(row);
						}
					}
					// wyskakujemy z pêtli ¿eby wczytaæ nastêpny
					break;
				}
			}
		
		}
	}


    enResult FileBase::save (const StringRef& fn, enFileOperation operation)
    {
		if (!_table) return errNotInitialized;

		int i = 0;

		do {
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
					if (_table->getRow(i)->hasFlag(rflagDontSave) == false) {
						this->writeRow(i);
					}
				}
				break;
			} catch (DTException e) {
				this->close();
				if (_table->getInterface().empty() || _table->getInterface()->handleFailedSave(this, e, i) &iInterface::fail) {
					return e.errorCode;
				}
				i++;
			}
		} while (1);

		this->close();
		return success;
    }


    enResult FileBase::append (const StringRef& fn, enFileOperation operation) {
		if (!_table) return errNotInitialized;
		
	    int i = 0;
		do {
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
					_table->getRow(i)->setId(_table->getNewRowId());
					this->writeRow(i);
				}
				break;
			} catch (DTException e) {
				this->close();
				if (_table->getInterface().empty() || _table->getInterface()->handleFailedSave(this, e, i) &   iInterface::fail) {
					return e.errorCode;
				}
				i++;
			}
		} while (1);

	    this->close();
		return success;
	}


} }