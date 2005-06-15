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
#include "stdafx.h"

#include "ListView.h"

namespace Stamina {
namespace ListWnd {
	bool Entry::isCollection() {
		return this->getICollection() != 0;
	}
	iCollection* Entry::getICollection() {
		return 0;
	}
	void Entry::refreshEntry(ListView* lv, RefreshFlags refresh) {
		if (refresh == refreshAuto) 
			return;
		//if (refresh == refreshPaint
		ItemList list;
		lv->getRootItem()->getEntryItems(*this, list);
		lv->lockPaint();
		for (ItemList::iterator it = list.begin(); it != list.end(); ++it) {
			if (refresh & refreshPaint) {
				(*it)->repaint(lv, refresh == refreshPaintWhole);
			} else {
				(*it)->setRefreshFlag(refresh);
			}
		}
		lv->unlockPaint();
		if (refresh & refreshPaint) {
			lv->repaintView();
		} else {
			lv->refreshItems();
		}
	}
	oItem Entry::createItem(ListView* lv, const oItemCollection& collection) {
		return *(new Item(*this));
	}

	void Entry::remove(ListView* lv) {
		ParentItemList list;
		lv->lockRefresh();
		lv->getRootItem()->getEntryItems(*this, list);
		for (ParentItemList::iterator it = list.begin(); it != list.end(); ++it) {
			it->first->removeItem(lv, (it->second).get());
		}
		lv->unlockRefresh();
	}


} /* ListWnd */

} /* Stamina */

