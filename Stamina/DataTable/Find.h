#pragma once
/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: DT.h 93 2005-08-22 00:23:52Z hao $
 */

/**@file All basic definitions needed to work with DataTable

*/

#include "DT.h"
#include "iColumn.h"
#include "Value.h"

namespace Stamina { namespace DT {

	class Find {
	public:

		enum Operation {
			eq, neq, gt, gteq, lt, lteq
		};

		inline Find(Operation operation, const oColumn& col, const oValue& value):operation(operation), col(col), value(value) {
			S_ASSERT(col.isValid());
		}

		static inline Find EqStr(const oColumn& col, const StringRef& str) {
			S_ASSERT(col.isValid());
			return Find(eq, col, new Value_stringRef(str));
		}
		static inline Find EqInt(const oColumn& col, int value) {
			S_ASSERT(col.isValid());
			return Find(eq, col, new Value_int(value));
		}

		Operation operation;
		oColumn col;
		oValue value;
	};


} }