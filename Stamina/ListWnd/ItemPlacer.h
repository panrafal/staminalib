/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
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
