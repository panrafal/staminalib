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
#include "String.h"
#include "Memory.h"

namespace Stamina {



// --- StringBuffer ---------------------------------------------------------

	template StringBuffer<char>;
	template StringBuffer<wchar_t>;

	template <typename CHAR>
	CHAR* StringBuffer<CHAR>::_alloc(unsigned int &size) {
		return Memory::allocBuffer<CHAR>(size);
	}

	template <typename CHAR>
	void StringBuffer<CHAR>::_free(CHAR* buff, unsigned int size) {
		Memory::freeBuffer<CHAR>(buff, size);
	}
};