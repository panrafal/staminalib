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
#include <vector>
#include "Item.h"
#include "ListView.h"
#include "ItemWalk.h"
namespace Stamina {
namespace ListWnd {

	Item::Item(const oEntry& entry)
	{
        this->_entry = entry;
		this->_itemState = stateExcluded;
		this->_itemFlags = (ItemFlags) (flagRepaint | flagRepos | flagForExpand) ;
		this->_level = 0;
	}
/*
	Size Item::calculateSize(ListView* lv)
	{
		if (this->needResize() == false && this->_size)
			return this->_size;
		// nie ma rozmiaru - trzeba go policzyæ...
		this->_size = this->getEntry()->getEntrySize(lv;
		return this->_size;
	}   */

	Size Item::getWholeSize(bool accurate) {
		if (this->_size)
			return this->_size;
		if (accurate)
			return Size(0,0);
		else
			return this->getEntry()->getQuickSize();
	}
				   
	Size Item::getSize() {
		return this->_size;
	}

	void Item::setVisible(ListView* lv, bool visible) {
		this->setFlag(flagHidden, !visible, lv);
		lv->refreshItems();
	}

	bool Item::isVisible()
	{
		return !this->getFlag(flagHidden) && !this->getFlag(flagHiddenInCollection);
	}
	bool Item::isPositioned() {
		return this->isVisible() && this->getState() == stateNormal;
	}



	void Item::setState(ItemState state)
	{
		if (this->_itemState == state) return;
		ObjLocker lock(this);
/*		if (state == stateExcluded) {
			this->_pos = Point();
			this->_size = Size();
		}*/
		this->_itemState = state;
	}

	ItemState Item::getState()
	{
		return this->_itemState;
	}

	void Item::setFlag(ItemFlags flag, bool setting, ListView* lv)
	{
		if (this->getFlag(flag) == setting) return;	
		ObjLocker lock(this);
		bool repaint = false;
		bool visible = this->isVisible();

		if (setting)
			this->_itemFlags = (ItemFlags) (this->_itemFlags | flag);
		else
			this->_itemFlags = (ItemFlags) (this->_itemFlags & (~flag));

		switch (flag) {
			case flagActive:
				repaint = true;
				break;
			case flagChecked:
				repaint = true;
				break;
			case flagSelected:
				repaint = true;
				break;
			case flagExpanded:
				repaint = true;
				break;
			case flagHidden: case flagHiddenInCollection:
				if (visible != this->isVisible()) {
					if (setting) {
						if (lv) { // odznaczamy aktywne...
							if (lv->isActiveItem(this)) {
								// ustawiamy aktywnym kogoœ w pobli¿u...
								lv->setActiveItem(this->getNeighbour(lv, -1));

							}
						}
// hiddenInCollection repaint idzie "z góry"
						if (flag == flagHidden) {
							if (lv) // zanim go wykluczymy...
				 				lv->repaintItemRect(this->getWholeRect());

						}
						this->setState(stateExcluded);
					}
					this->setFlag(flagChanged, true);
				}
				break;
		};

		this->_entry->onFlagChanged(lv, this, flag);

		if (repaint) {
			if (lv)
				this->repaint(lv);
			else
				this->setRefreshFlag(refreshPaint);
		}

	}

	bool Item::getFlag(ItemFlags flag)
	{
		return (this->_itemFlags & flag) == flag;
	}

	oEntry Item::getEntry()
	{
		return this->_entry;
	}

	Rect Item::getRect()
	{
		ObjLocker lock(this);
		return Rect(this->getPos(), this->getSize());
	}
	Rect Item::getWholeRect(bool accurate) {
		ObjLocker lock(this);
		return Rect(this->getPos(), this->getWholeSize(accurate));	
	}

/*	bool Item::setRect(Rect rect)
	{
		
	}*/

	void Item::setRefreshFlag(RefreshFlags refresh, bool setting)
	{
		/* RefreshFlags i ItemFlags siê pokrywaj¹ */
		if (setting) {
			this->_itemFlags = (ItemFlags) (this->_itemFlags | refresh);
		} else {
			this->_itemFlags = (ItemFlags) (this->_itemFlags & ~refresh);
		}
	}

	void Item::repaint(ListView* lv, bool whole) {
		if (!this->isPositioned()) return;
		lv->repaintItemRect(whole ? this->getWholeRect() : this->getRect());
	}

	oItemCollection Item::getParent(ListView* lv)
	{
		return lv->findItem(oItem(this));
	}

	ItemCollection* Item::getItemCollection()
	{
		return 0;	
	}

	bool Item::setSize(Size size)
	{
		if (size == this->_size) return false;
		ObjLocker lock(this);
		this->_size = size;
		this->setRefreshFlag(refreshDimensionsChanged);
		return true;
	}

	bool Item::setPos(Point pos)
	{
		if (pos == this->_pos) return false;
		ObjLocker lock(this);
		this->_pos = pos;
		this->setRefreshFlag(refreshDimensionsChanged);
		return true;
	}

	Point Item::getPos()
	{
		return this->_pos;
	}


	class WalkFindNext {
	public:
		WalkFindNext(int delta):_delta(delta) {
		}
		void operator()(const oItem& item, ItemWalk& walk) {
			_found = item;
			if (!_delta) {
				walk.stop();
			} else {
				_delta--;
			}
		}
		oItem result() {
			return _found;
		}
	private:
		oItem _found;
		int _delta;
	};
	class WalkFindPrev {
	public:
		WalkFindPrev(int delta):_delta(delta) {
			_found.reserve(delta);
		};
		void operator()(const oItem& item, ItemWalk& walk) {
			if (item == walk.getEnd())
				return;
			if (_found.size() >= _delta)
				_found.erase(_found.begin());
			_found.push_back(item.get());
		}
		oItem result() {
			if (_found.empty()) return oItem();
			return oItem(_found.front());
		}
	private:
		std::vector<Item*> _found;
		int _delta;
	};


	oItem Item::getNeighbour(ListView* lv, int delta) {

		if (delta == 0) return *this;
		if (delta > 0) {
			WalkFindNext fn(delta);
			ItemWalk::walk(lv, fn, oItem(this));
			return fn.result();
		} else {
			WalkFindPrev fp(-delta);
			ItemWalk::walk(lv, fp, oItem(), oItem(this));
			return fp.result();
		}
	}

	bool Item::containsActiveItem(ListView* lv) {
		return lv->isActiveItem(this);
	}

} /* ListWnd */

} /* Stamina */

