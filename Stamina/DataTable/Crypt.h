/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once
#ifndef __DT_CRYPT__
#define __DT_CRYPT__

namespace Stamina { namespace DT {


char * sc_key(unsigned char * key , int type);

void sc_encrypt(unsigned char * key , unsigned char * data , unsigned int size = 0);
void sc_decrypt(unsigned char * key , unsigned char * data , unsigned int size);

} }