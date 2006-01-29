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

$Id: $

*/

#pragma once


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