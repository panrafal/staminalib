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
#include "CollectionEntry.h"
namespace Stamina
{
namespace ListWnd
{

	CollectionEntry::CollectionEntry(const std::string& text, bool expanded):_text(text), _expanded(expanded) {
		
	}

	oItem CollectionEntry::createItem(ListView* lv, const oItemCollection& collection) {
		oItem item = Collection::createItem(lv, collection);
		if (_expanded)
			item->setFlag(flagExpanded, true);
		return item;
	}


	Size CollectionEntry::getMinSize() {
		return Size(30, 18);
	}
	Size CollectionEntry::getMaxSize() {
		return Size(0x7FFF, 18);
	}
	Size CollectionEntry::getQuickSize() {
		return Size(0, 18);
	}
	Size CollectionEntry::getEntrySize(ListView* lv, const oItem& li,			const oItemCollection& parent, Size fitIn) {
		fitIn.h = 16;
		return fitIn;
	}
	void CollectionEntry::paintEntry(ListView* lv, const oItem& li,				const oItemCollection& parent) {
		ObjLocker lock(this);
		HDC dc = lv->getDC();
		Rect rc = lv->itemToClient(li->getRect());
		if (li->isActive()) {
			FillRect(dc, rc.ref(), GetSysColorBrush(COLOR_ACTIVECAPTION));
		} 
		if (li->isSelected()) {
			SetTextColor(dc, 0xFF);
		} else {
			SetTextColor(dc, 0);
		}
		Rect trc = rc;
		int left = rc.left + 2;
		left += (li->getLevel()-1) * 20;
		MoveToEx(dc, left+2, rc.top + 8, 0);
		LineTo(dc, left + 7, rc.top + 8);
		if (!li->getItemCollection()->isExpanded()) {
			MoveToEx(dc, left + 4, rc.top + 6, 0);
			LineTo(dc, left + 4, rc.top + 11);
		}
		trc.left += (li->getLevel()-1) * 20 + 18;
		trc.top += 2;
		SetBkMode(dc, TRANSPARENT);
		DrawText(dc, this->_text.c_str(), -1, trc.ref(), DT_NOPREFIX);
		lv->releaseDC(dc);
	}

	void CollectionEntry::setText(ListView* lv, const std::string& text, bool repaint) {
		ObjLocker lock(this);
		this->_text = text;
		if (repaint)
			this->refreshEntry(lv, refreshPaint);
	}


	bool CollectionEntry::onKeyUp(ListView* lv, const oItem& li, int level, int vkey, int info) {
		if (level > 0) return true;
		if (vkey == VK_RIGHT) {
			li->getItemCollection()->setExpanded(lv, true);
			return false;
		} else if (vkey == VK_LEFT) {
			li->getItemCollection()->setExpanded(lv, false);
			return false;
		}
		return true;
	}
	bool CollectionEntry::onMouseDblClk(ListView* lv, const oItem& li, int level, int vkey, const Point& pos) {
		if (level > 0) return true;
		if (vkey == MK_LBUTTON) {
			li->getItemCollection()->setExpanded(lv, !li->getItemCollection()->isExpanded());
			return false;
		}
        return true;
	}

} /* ListWnd */

} /* Stamina */

