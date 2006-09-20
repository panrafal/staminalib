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

// Memory.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Memory.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

namespace Stamina { namespace Memory {


void* __stdcall sm_malloc(unsigned int size) {
	return ::malloc(size);
}
void __stdcall sm_free(void* buff) {
	::free(buff);
}

// --- Pooled buffers

#define GETBYTES(TYPE) (sizeof(TYPE))

#define DECLARE_POOLED_BUFFER(TYPE, SIZE, SMALL, MEDIUM)\
	class BufferSmallTag##SIZE {};\
	typedef boost::singleton_pool<BufferSmallTag##SIZE, (SMALL + 2) * GETBYTES(TYPE)> poolSmall##SIZE; \
	class BufferMediumTag##SIZE {};\
	typedef boost::singleton_pool<BufferMediumTag##SIZE, (MEDIUM + 2) * GETBYTES(TYPE)> poolMedium##SIZE; \
	\
	TYPE* __stdcall sm_allocBuffer##SIZE(unsigned int& size) {\
		TYPE* buff;\
		if (size <= SMALL) {\
			size = SMALL;\
			buff = (TYPE*) poolSmall##SIZE::malloc();\
		} else if (size <= MEDIUM) {\
			size = MEDIUM;\
			buff = (TYPE*) poolMedium##SIZE::malloc();\
		} else {\
			buff = (TYPE*) malloc((size + 2) * GETBYTES(TYPE));\
		}\
		*buff = 0;\
		buff ++;\
		return buff;\
	}\
	\
	void __stdcall sm_freeBuffer##SIZE(TYPE* buff, unsigned int size) {\
		buff--;\
		if (size <= SMALL) {\
			poolSmall##SIZE::free(buff);\
		} else if (size <= MEDIUM) {\
			poolMedium##SIZE::free(buff);\
		} else {\
			free(buff);\
		}\
	}

DECLARE_POOLED_BUFFER(char, 8, 32, 128);
DECLARE_POOLED_BUFFER(wchar_t, 16, 32, 128);
DECLARE_POOLED_BUFFER(int, 32, 16, 64);


void* __stdcall sm_allocBuffer(unsigned int& size, unsigned int itemSize) {
	switch (itemSize) {
		case 1:
			return sm_allocBuffer8(size);
		case 2:
			return sm_allocBuffer16(size);
		case 4:
			return sm_allocBuffer32(size);
		default:
			return sm_malloc(size * itemSize);
	};
}

void __stdcall sm_freeBuffer(void* buff, unsigned int size, unsigned int itemSize) {
	switch (itemSize) {
		case 1:
			return sm_freeBuffer8((char*)buff, size);
		case 2:
			return sm_freeBuffer16((wchar_t*)buff, size);
		case 4:
			return sm_freeBuffer32((int*)buff, size);
		default:
			return sm_free(buff);
	};
}


} }
