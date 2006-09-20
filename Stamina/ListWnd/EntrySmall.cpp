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
#include "EntrySmall.h"
namespace Stamina
{
namespace ListWnd
{
	EntrySmall::EntrySmall(const std::string& text):_text(text) {
	}

	Size EntrySmall::getMinSize() {
		return Size(30, 18);
	}
	Size EntrySmall::getMaxSize() {
		return Size(0x7FFF, 18);
	}
	Size EntrySmall::getQuickSize() {
		return Size(0, 18);
	}
	Size EntrySmall::getEntrySize(ListView* lv, const oItem& li,			const oItemCollection& parent, Size fitIn) {
		return Size(fitIn.w, 16);
		HDC dc = lv->getDC();
		Rect r = Rect(0, 0, fitIn.w - (li->getLevel() * 20 + 2), 0);
		fitIn.h = DrawText(dc, this->_text.c_str(), -1, r.ref(), DT_NOPREFIX | DT_CALCRECT | DT_WORDBREAK);
		//fitIn.h = r.getHeight();
		if (fitIn.h >= 14)
			fitIn.h += 2;
		else
			fitIn.h = 16;
		lv->releaseDC(dc);
		fitIn.w = max(200, fitIn.w);
		return fitIn;
	}
	void EntrySmall::paintEntry(ListView* lv, const oItem& li,				const oItemCollection& parent) {
		ObjLocker lock(this, lockRead);
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
		trc.left += li->getLevel() * 20;
		trc.top += 2;
		SetBkMode(dc, TRANSPARENT);
		DrawText(dc, this->_text.c_str(), -1, trc.ref(), DT_NOPREFIX | DT_WORDBREAK);
		lv->releaseDC(dc);
	}

	void EntrySmall::setText(ListView* lv, const std::string& text, bool repaint) {
		ObjLocker lock(this, lockWrite);
		this->_text = text;
		this->refreshEntry(lv, refreshSize);
		if (repaint)
			this->refreshEntry(lv, refreshPaint);
	}


} /* ListWnd */

} /* Stamina */

