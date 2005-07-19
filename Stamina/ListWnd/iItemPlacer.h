/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
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
