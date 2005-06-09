/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

/* Model statyczny */
#include "stdafx.h"
#include "ListView.h"
namespace Stamina
{
namespace ListWnd
{
	iCollection* Collection::getICollection() {
		return (iCollection*)this;
	}
	oItem Collection::createItem(ListView* lv, const oItemCollection& collection) {
		return new ItemCollection(*this);
	}
	void Collection::insertEntry(ListView* lv, const oEntry& entry, int pos, ItemList* created) {
		ItemList list;
		lv->lockRefresh();
		lv->getRootItem()->getEntryItems(*this, list);
		for (ItemList::iterator it = list.begin(); it != list.end(); ++it) {
			oItem item = (*it)->getItemCollection()->insertEntry(lv, entry, pos);
			if (created)
				created->push_back(item);
		}
		lv->unlockRefresh();
	}
	void Collection::removeEntry(ListView* lv, const oEntry& entry, bool recurse) {
		ItemList list;
		lv->lockRefresh();
		lv->getRootItem()->getEntryItems(*this, list);
		for (ItemList::iterator it = list.begin(); it != list.end(); ++it) {
			(*it)->getItemCollection()->removeEntry(lv, entry, recurse);
		}
		lv->unlockRefresh();
	}
	void Collection::removeAll(ListView* lv) {
		ItemList list;
		lv->lockRefresh();
		lv->getRootItem()->getEntryItems(*this, list);
		for (ItemList::iterator it = list.begin(); it != list.end(); ++it) {
			(*it)->getItemCollection()->removeAll(lv);
		}
		lv->unlockRefresh();
	}


	oItemPlacer Collection::getItemPlacer(ListView* lv,	const oItemCollection& coll) {
		return lv->defaultItemPlacer();
	}

	Rect Collection::getItemsRect(ListView* lv, const oItemCollection& item) {
		Rect r = item->getRect();
		r.top += item->getSize().h;
		return r;
	}

} /* ListWnd */

} /* Stamina */

