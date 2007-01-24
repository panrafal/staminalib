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

#pragma once
#ifndef __DT_DATAROW__
#define __DT_DATAROW__

#include "iRow.h"
#include "../ObjectImpl.h"


namespace Stamina { namespace DT {

	class DataTable;


	class DataRow: public SharedObject<iRow, LockableObject<iRow, CriticalSection_simple> > {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
		friend class ColumnsDesc;
		friend class FileBin;
	public:

		STAMINA_OBJECT_CLASS(DT::DataRow, iRow)

		DataRow(DataTable* table) {
			_id = 0;
			_flag = rflagNone;
			_table = table;
		}

		~DataRow() {
			freeData();
		}


		inline void setId(tRowId id) {
			_id = flagId(id);
		}

		const DataEntry getData (unsigned int colIndex) const throw(...); // Pobiera wartosc kolumny
		/**Sets the column value (without conversion and without locking and column lookup)*/
		bool setData (unsigned int colIndex, DataEntry val) throw(...);

		inline bool hasColumnSlot(unsigned int colIndex) const {
			return (colIndex >= 0 && colIndex < _data.size());
		}

		virtual void removeFromTable();


	protected:

		bool freeData();

		virtual void prepareSlot(unsigned int colIndex);

		typedef std::vector<DataEntry> tData;
		tData _data;
		unsigned int _filePos;
		//CriticalSection_blank _cs; // blokada

	};


	typedef oRowT<DataRow> oDataRow;


} }

#endif