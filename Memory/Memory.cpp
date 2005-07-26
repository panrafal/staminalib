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

} }
