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
#ifndef __DT_COLUMN__
#define __DT_COLUMN__

#include <boost/function.hpp>
#include <boost/signal.hpp>

#include "../ObjectImpl.h"
#include "../StringSTL.h"
#include "iColumn.h"
#include "Value.h"


namespace Stamina { namespace DT {

	class Column: public SharedObject<iColumn> {
	public:

		friend class ColumnsDesc;

		STAMINA_OBJECT_CLASS(DT::Column, iColumn);

		Column() {
		}

		//virtual iObject* cloneObject();

		virtual DataEntry getDefaultStaticData() const {
			return 0;
		}
		virtual void dataDispose(DataEntry data) const {
		}

	protected:

		void init(unsigned int index, tColId id, enColumnFlag flags, const AStringRef& name) {
			_index = index;
			_id = id;
			this->setFlag(flags, true);
			_name = name;
		}

		void setIndex(unsigned int index) {
			_index = index;
		}
		void setId(tColId id) {
			_id = id;
		}

		virtual void cloneMembers(const iObject* a) {
			BaseClass::cloneMembers(a);
			const Column* b = a->castStaticObject<const Column>();
/*			this->_getHandler = b->_getHandler;
			this->_setHandler = b->_setHandler;
			this->_preTrigger = b->_preTrigger;
			this->_postTrigger = b->_postTrigger;*/
		}

	public:

		// enResult getHandler (oValue&, const iColumn*, iRow*, enColumnType, GetFlags)
		// enResult setHandler (oValue&, const iColumn*, iRow*, SetFlags)
		// void trigger(const iColumn*, iRow*, SetFlags)

		/** Handler for @b get operations on column.
		@param oValue - in/out value object. Contains the current value in column's type. You can change only the value, or even assign a new value object.
		@param iColumn - the column being read
		@param iRow - the row being read
		@param enColumnType - target value type
		@param GetFlags - additional flags
		*/
		boost::function5< enResult, oValue&, const iColumn*, const iRow*, enColumnType, GetFlags > getHandler;
		/** Handler for @b set operations on column.
		@param oValue - in/out value object. Contains the value to be set (can be any of any type!). You can change only the value, or even assign a new value object.
		@param iColumn - the column being set
		@param iRow - the row being set
		@param SetFlags - additional flags
		*/
		boost::function4< enResult, oValue&, const iColumn*, iRow*, SetFlags > setHandler;

		/** List of triggers which are called @before setting a new value on column.
		@param Value - value currently being set
		@param iColumn - the column being set
		@param iRow - the row being set
		@param SetFlags - additional flags
		*/
		boost::signal4<void, const Value*, const iColumn*, iRow*, SetFlags> preTrigger;

		/** List of triggers which are called @after setting a new value on column.
		@param iColumn - the column being set
		@param iRow - the row being set
		@param SetFlags - additional flags
		*/
		boost::signal3<void, const iColumn*, iRow*, SetFlags> postTrigger;


	};

} }

#endif