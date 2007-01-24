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

The Initial Developer of the Original Code is "STAMINA" - Rafa- Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa- Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id: BrowserCtrl.cpp 179 2006-03-14 22:56:40Z hao $

*/

#pragma once

#include "Stamina.h"
#include <comutil.h>


#define S_COM_SMARTPTR_TYPEDEF(T) __if_not_exists(T##Ptr) {typedef _com_ptr_t<_com_IIID<T, &__uuidof(T)> > T ## Ptr ;}


