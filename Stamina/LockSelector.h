/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: Lock.h 108 2005-09-10 11:53:46Z hao $
 */

#pragma once


namespace Stamina {

	enum enLockType {
		lockDefault = 0,
		lockSharedMutex = 1,
		lockRead = 2,
		lockWrite = 3,
	};



}