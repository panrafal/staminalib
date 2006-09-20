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


#ifndef __ITEMPLACER__
#define __ITEMPLACER__


#include "iItemPlacer.h"
#include "..\ObjectImpl.h"
namespace Stamina {
namespace ListWnd {
	class ItemPlacer : public SharedObject<iItemPlacer>
	{
	protected:
		static inline bool setSize(Item* item, Size size) {
			return item->setSize(size);
		}
		static inline bool setPos(Item* item, Point pos) {
			return item->setPos(pos);
		}
		static inline bool setWholeSize(ItemCollection* coll, Size size) {
			return coll->setWholeSize(size);
		}
		static inline bool refreshItems(ItemCollection* coll, ListView* lv,
			RefreshFlags refresh, const oItemCollection& parent) 
		{
			return coll->refreshItems(lv, refresh, parent);
		}

	};

	// ------------------------------------------------------------


	// ------------------------------------------------------------

	class ItemPerRow : public ItemPlacer
	{
	public:
		ItemPerRow()
			:marginLeft(0), marginTop(0), marginRight(0), marginBottom(0)
			,spaceBetween(0), spaceTop(0), spaceBottom(0)
		{}
		int marginLeft, marginTop, marginRight, marginBottom;
		int spaceBetween, spaceTop, spaceBottom;
	protected:
		bool place(ListView* lv
			, const oItemCollection& collection
			, const oItemCollection& parent
			, RefreshFlags refresh);

	};


} /* ListWnd */

} /* Stamina */


#endif /* __ITEMPLACER__ */
