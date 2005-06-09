/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once

//#define CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define S_ASSERT(a) _ASSERTE(a);
#define S_ASSERT_MSG(a, msg) _ASSERTE(a);

#define S_ERROR(msg) 

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
