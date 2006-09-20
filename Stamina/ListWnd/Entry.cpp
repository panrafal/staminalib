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

