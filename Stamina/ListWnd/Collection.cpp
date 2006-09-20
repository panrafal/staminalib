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

