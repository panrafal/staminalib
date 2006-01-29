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


/* Model statyczny */


#ifndef __STAMINA_LISTWND_IITEMPLACER__
#define __STAMINA_LISTWND_IITEMPLACER__


/* Include files */
#include "..\Object.h"
#include "iItem.h"
#include "ListWnd.h"
namespace Stamina
{
namespace ListWnd
{
	class iItemPlacer: public iSharedObject
	{

	public:

		STAMINA_OBJECT_CLASS_VERSION(ListWnd::iItemPlacer, iSharedObject, Version(1,0,0,0));

		virtual bool place(ListView* lv,
					 const oItemCollection& collection,
					 const oItemCollection& parent,
					 RefreshFlags refresh)=0;

	};/* END INTERFACE DEFINITION IItemPlacer */

	STAMINA_REGISTER_CLASS_VERSION(iItemPlacer);

	class oItemPlacer: public SharedPtr<iItemPlacer> {
	};

} /* ListWnd */

} /* Stamina */


#endif /* __IITEMPLACER__ */
