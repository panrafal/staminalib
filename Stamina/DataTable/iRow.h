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
#ifndef __DT_IROW__
#define __DT_IROW__

#include "DT.h"
#include "..\ObjectPtr.h"
#include "..\Lock.h"

namespace Stamina { namespace DT {

	class DataTable;

	class iRow: public iSharedObject {  // Wiersz w tabeli ... Zawiera tylko wartosci kolumn
	public:

		STAMINA_OBJECT_CLASS_VERSION(DT::iRow, iSharedObject, Version(1, 0, 0, 0));

		enRowFlag getFlags() const {
			return _flag;
		}
		void setFlag(enRowFlag flag, bool setting) {
			if (setting)
				_flag = (enRowFlag) (_flag | flag);
			else
				_flag = (enRowFlag) (_flag & (~flag));
		}
		bool hasFlag(enRowFlag flag) const {
			return (_flag & flag) != 0;
		}

		inline tRowId getId() const {
			return _id;
		}

		inline static tRowId flagId(tRowId row) {
			return (tRowId)((row) | DT::rowIdFlag);
		}
		/** Removes flag from the row Id */
		inline static tRowId unflagId(tRowId row) {
			return (tRowId)( row&(~DT::rowIdFlag) );
		}

		virtual void removeFromTable()=0;

		DataTable* getOwner() const {
			return _table;
		}


	protected:
		enRowFlag _flag;
		DataTable * _table; ///< Parent table
		tRowId _id;    // identyfikator wiersza

	};

	template <class TYPE>
	class oRowT: public SharedPtr<TYPE> {
	public:
		oRowT(TYPE * obj = 0):SharedPtr<TYPE>() {
			this->set(obj);
		}
		oRowT(TYPE & obj):SharedPtr<TYPE>() {
			this->set(obj);
		}
		oRowT(const oRowT & b):SharedPtr<TYPE>() {
			this->set(&*b);
		}
		oRowT& operator = (const oRowT & b) {
			this->set(&*b);
			return *this;
		}

		operator tRowId() const {
			return getId();
		}

		/*
		operator int() const {
			return getId();
		}
		*/

		tRowId getId() const {
			if (this->get()) {
				return this->get()->getId();
			} else {
				return rowNotFound;
			}
		}
		
		bool isFound() const {
			return getId() != rowNotFound;
		}

	};

	typedef oRowT<iRow> oRow;

	iRow* const rowDefault = (iRow*)-1;

	STAMINA_REGISTER_CLASS_VERSION(iRow);

} }

#endif