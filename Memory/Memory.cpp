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

#define GETBYTES(TYPE) (sizeof(TYPE) / 8)

#define DECLARE_POOLED_BUFFER(TYPE, SIZE, SMALL, MEDIUM)\
	class BufferSmallTag##SIZE {};\
	typedef boost::singleton_pool<BufferSmallTag##SIZE, (SMALL + 1) * GETBYTES(TYPE)> poolSmall##SIZE; \
	class BufferMediumTag##SIZE {};\
	typedef boost::singleton_pool<BufferMediumTag##SIZE, (MEDIUM + 1) * GETBYTES(TYPE)> poolMedium##SIZE; \
	\
	TYPE* __stdcall sm_allocBuffer##SIZE(unsigned int& size) {\
		if (size <= SMALL) {\
			size = SMALL;\
			return (TYPE*) poolSmall##SIZE::malloc();\
		} else if (size <= MEDIUM) {\
			size = MEDIUM;\
			return (TYPE*) poolMedium##SIZE::malloc();\
		} else {\
			return (TYPE*) malloc((size + 1) * GETBYTES(TYPE));\
		}\
	}\
	\
	void __stdcall sm_freeBuffer##SIZE(TYPE* buff, unsigned int size) {\
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
