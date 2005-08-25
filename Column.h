/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
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

		// enResult getHandler (oValue&, const iColumn*, iRow*, GetFlags)
		// enResult setHandler (oValue&, const iColumn*, iRow*, SetFlags)
		// void trigger(const iColumn*, iRow*, SetFlags)

		boost::function4< enResult, oValue&, const iColumn*, const iRow*, GetFlags > _getHandler;
		boost::function4< enResult, oValue&, const iColumn*, iRow*, SetFlags > _setHandler;
		boost::signal3<void, const iColumn*, iRow*, SetFlags> _preTrigger;
		boost::signal3<void, const iColumn*, iRow*, SetFlags> _postTrigger;


	};

} }

#endif