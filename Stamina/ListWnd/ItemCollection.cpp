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

#include <algorithm>
#include <boost/bind.hpp>

#include "Item.h"
#include "ListView.h"
namespace Stamina
{
namespace ListWnd
{

	ItemCollection::ItemCollection(const oCollection& entry)
		:Item(entry) {
		this->_itemFlags = (ItemFlags) (this->_itemFlags | flagExpandable) ;
	}


	ItemCollection* ItemCollection::getItemCollection()
	{
		return this;	
	}

	ItemList& ItemCollection::getItemList()
	{
		return this->_items;	
	}

	void ItemCollection::setState(ItemState state) {
//		if (this->_itemState == state) return;
//		ObjLocker lock(this);
		this->Item::setState(state);
	}

	void ItemCollection::setFlag(ItemFlags flag, bool setting, ListView* lv) {
		if (this->getFlag(flag) == setting) return;
		ObjLocker lock(this, lockWrite);
		bool visible = this->isVisible();
		bool repaint = false;

		this->Item::setFlag(flag, setting, lv);

		switch (flag) {
			case flagHidden: case flagHiddenInCollection:
				if (visible != this->isVisible()) {
					if (!setting && flag == flagHidden) // "dzieciaki" powinny byæ ju¿ zawarte w whole, wiêc nie trzeba odœwie¿aæ przy InColl... Gdy pokazujemy i tak zostanie wszystko ³adnie odœwie¿one
						repaint = true;
					this->setFlag(flagSubitemsChanged, true);
					for (ItemList::iterator it = _items.begin(); it != _items.end(); ++it) {
						if (setting) {
							(*it)->setFlag(flagHiddenInCollection, true);
						} else {
							if (this->isExpanded()) {
								(*it)->setFlag(flagHiddenInCollection, false);
							} else {
								if ((*it)->notForExpand())
									(*it)->setFlag(flagHiddenInCollection, false);
							}
						}
					}
				}
				break;
			case flagExpanded:
				if (this->isVisible()) {
					if (!setting) 
						repaint = true;
					this->setFlag(flagSubitemsChanged, true);
					for (ItemList::iterator it = _items.begin(); it != _items.end(); ++it) {
						if ((*it)->isForExpand()) {
							(*it)->setFlag(flagHiddenInCollection, !setting, lv);
						}
					}
				}
				break;

		}

		if (repaint) {
			this->setRefreshFlag(refreshPaintWhole);
		}

	}


	oItem ItemCollection::getEntryItem(const oEntry& entry, bool recurse)
	{
		ObjLocker lock(this, lockRead);
		oItem found;
		ItemList::iterator it;
		for (it = _items.begin(); it != _items.end(); ++it) {
			if ((*it)->getEntry() == entry) return *it;
			if ((*it)->getItemCollection()) {
				found = (*it)->getItemCollection()->getEntryItem(entry);
				if (found) return found;
			}
		}
		return found;
	}

	bool ItemCollection::getEntryItems(const oEntry& entry, ItemList &list)
	{
		ObjLocker lock(this, lockRead);
		ItemList::iterator it;
		for (it = _items.begin(); it != _items.end(); ++it) {
			if ((*it)->getEntry() == entry) {
				list.push_back(*it);
			}
			if ((*it)->getItemCollection()) {
				(*it)->getItemCollection()->getEntryItems(entry, list);
			}
		}
		return list.begin() != list.end();
	}

	bool ItemCollection::getEntryItems(const oEntry& entry, ParentItemList &list)
	{
		ObjLocker lock(this, lockRead);
		ItemList::iterator it;
		for (it = _items.begin(); it != _items.end(); ++it) {
			if ((*it)->getEntry() == entry) {
				list.push_back(ParentItemPair(this, *it));
			}
			if ((*it)->getItemCollection()) {
				(*it)->getItemCollection()->getEntryItems(entry, list);
			}
		}
		return list.begin() != list.end();
	}


	oItem ItemCollection::getItemAt(Point pos, bool recurse, bool hitWhole)
	{
		ObjLocker lock(this, lockRead);
		oItem found;

		if (!hitWhole && this->getRect().contains(pos)) {
			return this;
		}

		ItemList::iterator it;
		for (it = _items.begin(); it != _items.end(); ++it) {
			if (!(*it)->isPositioned()) continue;
			if ((*it)->getItemCollection()) {
				if (recurse) {
					found = (*it)->getItemCollection()->getItemAt(pos, recurse, hitWhole);
					if (found) return found;
				}
			} else {
				if ((*it)->getRect().contains(pos))
					return *it;
			}
		}
		/*Je¿eli nie by³o nigdzie "poni¿ej", wszystko co znajdziemy w aktualnym elemencie uznajemy za jego "ramkê"*/
		if (hitWhole && this->getWholeRect().contains(pos))
			return oItem(this);
        return found;		
	}

	void ItemCollection::getItemsAt(Point pos, ItemList &items)
	{
		ObjLocker lock(this, lockRead);
		ItemList::iterator it;
		for (it = _items.begin(); it != _items.end(); ++it) {
			if (!(*it)->isPositioned()) continue;
			if ((*it)->getItemCollection()) {
				(*it)->getItemCollection()->getItemsAt(pos, items);
			} else {
				if ((*it)->getRect().contains(pos)) {
					items.push_front(*it);
					break;
				}
			}
		}
		/*Je¿eli nie by³o nigdzie "poni¿ej", wszystko co znajdziemy w aktualnym elemencie uznajemy za jego "ramkê"*/
		if (this->getWholeRect().contains(pos)) {
			items.push_back(oItem(this));
		}
	}


	bool ItemCollection::refreshItems(ListView* lv,
								  RefreshFlags refresh, const oItemCollection& parent)
	{
		oItemPlacer ip = this->getEntry()->getICollection()->getItemPlacer(lv, this);
		return ip->place(lv, this, parent, refresh);
	}

	void ItemCollection::repaintItems(ListView* lv) {
		ObjLocker lock(this, lockWrite);
		ItemList::iterator it;
		for (it = _items.begin(); it != _items.end(); ++it) {
			if ((*it)->isPositioned() && lv->itemInRepaintRegion((*it)->getWholeRect())) {
				(*it)->getEntry()->paintEntry(lv, *it, oItemCollection(*this));
				(*it)->setRefreshFlag(refreshPaintWhole, false);
			}
			if ((*it)->getItemCollection()) {
				(*it)->getItemCollection()->repaintItems(lv);
			}
		}
	}


	int ItemCollection::getItemIndex(const oItem& item)
	{
		ObjLocker lock(this, lockRead);
		ItemList::iterator it;
		int found = 0;
		for (it = _items.begin(); it != _items.end(); ++it) {
			if ((*it) == item) return found;
			found++;
		}
		return -1;		
	}

	bool ItemCollection::containsItem(const oItem& item)
	{
		return getItemIndex(item) != -1;	
	}

	oItemCollection ItemCollection::findItem(const oItem& item)
	{
		ObjLocker lock(this, lockRead);
		oItemCollection found;
		ItemList::iterator it;
		for (it = _items.begin(); it != _items.end(); ++it) {
			if ((*it) == item) return oItemCollection(this);
			if ((*it)->getItemCollection()) {
				found = (*it)->getItemCollection()->findItem(item);
				if (found)
					return found;
			}
		}
        return found;		
	}

	oItem ItemCollection::insertEntry(ListView* lv,	const oEntry& entry, int pos)
	{
		oItem item = entry->createItem(lv, this);
		this->insertItem(lv, item, pos);
		return item;
	}
	oItem ItemCollection::insertItem(ListView* lv, const oItem& item, int pos) {
		ObjLocker lock(this, lockWrite);
		if (item) {
			ItemList::iterator it;
			fCompareItems compare = this->getCompareFunction();
			if (compare) {
				it = std::find_if(_items.begin(), _items.end(), boost::bind(compare, item, _1));
			} else {
				it = (pos >= 0) ? _items.begin() : _items.end();

				int ipos = (pos >= 0) ? pos : -pos - 1;
				while (ipos) {
					if (pos >= 0) {
						if (it == _items.end())
							break;
						it ++;
					} else {
						if (it == _items.begin())
							break;
						it --;
					}
					ipos --;
				}
			}
			item->setRefreshFlag(refreshInserted);
			this->_items.insert(it, item);
			item->setLevel(this->getLevel() + 1);
			this->getEntry()->getICollection()->onItemInsert(lv, this, item);
			item->setFlag(flagHiddenInCollection
				, (!this->isVisible() || (item->isForExpand() && !this->isExpanded()) ));
			this->setFlag(flagSubitemsChanged, true);
		}
		lv->refreshItems();
		return item;
	}


	void ItemCollection::removeEntry(ListView* lv,
								 const oEntry& entry, bool recurse)
	{
		ObjLocker lock(this, lockWrite);
		lv->lockPaint();
		lv->lockRefresh();
		ItemList::iterator it = _items.begin();
		if (lv->getActiveItem().isValid() && lv->getActiveItem()->getEntry() == entry)
			lv->setActiveItem(oItem(0));
		while (it != _items.end()) {
			if ((*it)->getEntry() == entry) {
				(*it)->repaint(lv, true);
				it = _items.erase(it);
				if (it != _items.end()) {
					// Trzeba ustawiæ s¹siadowi odœwie¿enie pozycji...
					(*it)->setRefreshFlag(refreshPos);
					this->setFlag(flagSubitemsChanged, true);
					lv->refreshItems();
				}
				continue;
			}
			if ((*it)->getItemCollection() && recurse) {
				(*it)->getItemCollection()->removeEntry(lv, entry, recurse);
			}
			++it;
		}
		lv->unlockRefresh();
		lv->unlockPaint();

	}

	void ItemCollection::removeItem(ListView* lv, const oItem& item)
	{
		ObjLocker lock(this, lockWrite);
		ItemList::iterator it = std::find(_items.begin(), _items.end(), item);
		lv->lockPaint();
		item->repaint(lv, true);
		bool active = item->containsActiveItem(lv);
		it = _items.erase(it);
		if (it != _items.end()) {
			// Trzeba ustawiæ s¹siadowi odœwie¿enie pozycji...
			(*it)->setRefreshFlag(refreshPos);
			if (active)
				lv->setActiveItem(*it);
		} else {
			if (active)
				lv->setActiveItem(oItem(0));
		}
		this->setFlag(flagSubitemsChanged, true);
		lv->refreshItems();
		lv->unlockPaint();
	}

	void ItemCollection::removeAll(ListView* lv)
	{
		bool active = (this->containsActiveItem(lv) && !lv->isActiveItem(this));
		lv->lockPaint();
		this->repaint(lv, true);
        this->_items.clear();
		if (active)
			lv->setActiveItem(oItem(0));
		this->setFlag(flagSubitemsChanged, true);
		lv->refreshItems();
		lv->unlockPaint();
	}


	Size ItemCollection::getWholeSize(bool accurate) {
		return _wholeSize ? _wholeSize : _size;
	}

	bool ItemCollection::setWholeSize(Size size)
	{
		if (size == this->_wholeSize) return false;
		ObjLocker lock(this, lockWrite);
		this->_wholeSize = size;
		this->setRefreshFlag(refreshDimensionsChanged);
		return true;
	}

	void ItemCollection::setLevel(char level) {
		Item::setLevel(level);
		for_each(_items.begin(), _items.end(), boost::bind(Item::setLevel, boost::bind(oItem::get, _1), level+1));
	}

	void ItemCollection::setExpanded(ListView* lv, bool expanded) {
		//if (this->getFlag(flagExpanded) == expanded) return;
		//bool old = ;
		this->setFlag(flagExpanded, expanded, lv);
		this->_entry->getICollection()->updateItemsList(lv, this);
		lv->refreshItems();
	}

	bool ItemCollection::containsActiveItem(ListView* lv) {
		if (this->Item::containsActiveItem(lv))
			return true;
		for (ItemList::iterator it = _items.begin(); it != _items.end(); ++it) {
			if ((*it)->containsActiveItem(lv)) 
				return true;
		}
		return false;
	}

	void ItemCollection::sortItems(ListView* lv) {
		ObjLocker lock(this, lockWrite);
		fCompareItems comp = this->getCompareFunction();
		if (comp) {
			this->_items.sort(comp);
			this->setFlag(flagSubitemsChanged, true);
			lv->refreshItems();
		}
	}

	int ItemCollection::countVisible() { 
		ObjLocker lock(this, lockRead);
		int c = 0;
		for (ItemList::iterator it = _items.begin(); it != _items.end(); ++it) {
			if ((*it)->isVisible()) 
				c++;
		}
		return c;

	}

} /* ListWnd */

} /* Stamina */

