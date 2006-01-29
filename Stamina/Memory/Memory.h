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


/** @file Basic memory management functions for applications that share objects between modules (Exe and Dlls).
*/

#pragma once

#ifndef __STAMINA_MEMORY__
#define __STAMINA_MEMORY__

#ifdef S_MEMORY_EXPORTS
#define S_MEMORY_DLL __declspec(dllexport)
#else
#define S_MEMORY_DLL __declspec(dllimport)
#endif

#ifndef S_MEMORY_EXPORTS
#ifdef _DEBUG
#pragma comment(lib, "SMemory_d.lib")
#else
#pragma comment(lib, "SMemory.lib")
#endif
#endif

namespace Stamina { namespace Memory {

	extern "C" {

		S_MEMORY_DLL void* __stdcall sm_malloc(unsigned int size);
		S_MEMORY_DLL void __stdcall sm_free(void* buff);

		S_MEMORY_DLL char* __stdcall sm_allocBuffer8(unsigned int& size);
		S_MEMORY_DLL void __stdcall sm_freeBuffer8(char* buff, unsigned int size);
		
		S_MEMORY_DLL wchar_t* __stdcall sm_allocBuffer16(unsigned int& size);
		S_MEMORY_DLL void __stdcall sm_freeBuffer16(wchar_t* buff, unsigned int size);

		S_MEMORY_DLL void* __stdcall sm_allocBuffer(unsigned int& size, unsigned int itemSize);
		S_MEMORY_DLL void __stdcall sm_freeBuffer(void* buff, unsigned int size, unsigned int itemSize);

	};


	inline void* malloc(unsigned int size) {
		return sm_malloc(size);
	}
	inline void free(void* buff) {
		sm_free(buff);
	}

	/** Allocates character buffer for storing strings and other continous data.
	For sizes below 129 buffers are taken from two fast memory pools (small - 32 items, and medium - 128 items). Everything above is being malloc'ed.
	@size - Size in CHAR of requested buffer. Value of size is likely to change, you'll have to provide this value in freeBuffer().
	*/
	template <typename TYPE>
	inline TYPE* allocBuffer(unsigned int& size) {
		return (TYPE*)sm_allocBuffer(size, sizeof(TYPE));
	}
	/** Frees memory allocateted with allocBuffer.
	@param buff - buffer to free
	@param size - Size set in allocBuffer
	*/
	template <typename TYPE>
	inline void freeBuffer(TYPE* buff, unsigned int size) {
		sm_freeBuffer(buff, size, sizeof(TYPE));
	}

	template <>
	inline char* allocBuffer<char>(unsigned int& size) {
		return sm_allocBuffer8(size);
	}
	template <>
	inline wchar_t* allocBuffer<wchar_t>(unsigned int& size) {
		return sm_allocBuffer16(size);
	}
	template <>
	inline unsigned char* allocBuffer<unsigned char>(unsigned int& size) {
		return (unsigned char*)sm_allocBuffer8(size);
	}

	template <>
	inline void freeBuffer<char>(char* buff, unsigned int size) {
		sm_freeBuffer8(buff, size);
	}
	template <>
	inline void freeBuffer<wchar_t>(wchar_t* buff, unsigned int size) {
		sm_freeBuffer16(buff, size);
	}
	template <>
	inline void freeBuffer<unsigned char>(unsigned char* buff, unsigned int size) {
		sm_freeBuffer8((char*)buff, size);
	}


}; };

#endif