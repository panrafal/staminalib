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

//#define CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define S_ASSERT(a) _ASSERTE(a)



#ifdef _DEBUG
#define S_ASSERT_RUN(a) _ASSERTE(a)
#else
#define S_ASSERT_RUN(a) a
#endif

#ifdef _DEBUG
#define S_ASSERT_MSG(a, msg) _ASSERT_BASE(a, msg)
#else
#define S_ASSERT_MSG(a, msg) ((void)0)
#endif

#define S_DEBUG_ERROR(msg) S_ASSERT_MSG(false, msg)

#ifdef _DEBUG
#define S_ERROR(msg) _ASSERT_BASE(false, msg)
#else
#define S_ERROR(msg) MessageBox(0, msg, "S_ERROR!", MB_OK | MB_ICONEXCLAMATION)
#endif

#if 0
//#ifdef _DEBUG
//#define S_ASSERT_LOGMSG(a, log) if (!(a)) {IMDEBUG(DBG_ASSERT, log #a " (" __FUNCTION__ " : " __FILE__ ":%d)",__LINE__ ); }
//#define S_ASSERT_LOG(a)  S_ASSERT_LOGMSG(a, "Assert failed: ")
#else
#define S_ASSERT_LOG(a) 
#define S_ASSERT_LOGMSG(a, log) 
#endif

#define S_CHECK_PTR(ptr) ((int)ptr & 0xFFFF0000)




/*
#ifdef _DEBUG
#define K_ASSERT_PTR(ptr) K_ASSERT(K_CHECK_PTR(ptr))
#else
#define K_ASSERT_PTR(ptr) {if (!K_CHECK_PTR(ptr)) {IMDEBUG(DBG_ERROR, "Bad pointer: " #ptr " = %x in " __FUNCTION__, ptr);}}
#endif
*/
