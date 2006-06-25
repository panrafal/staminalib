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

#include "stdafx.h"
#include "Menu.h"

namespace Stamina {

	MenuW32::MenuW32():_useId(0) {

	}

	oMenuItem MenuW32::popupMenu(Point pt, HWND hwnd) {
		if (!this->size()) return oMenuItem();
		_menu = this->build();
		int ret = TrackPopupMenu(_menu,TPM_RETURNCMD,pt.x,pt.y,0,hwnd , 0);
		oMenuItem item;
		for (ItemsList::iterator it = _items.begin(); it != _items.end(); it++) {
			if ((item = it->get()->selectById(this, ret))) {
				break;
			}
		}
		DestroyMenu(_menu);
		_menu = 0;
		if (item) {
			item->onClick(this);
		}
		return item;
	}

	void MenuW32::insertItem(const oMenuItem& item, int pos) {
		if (pos < 0 || pos >= _items.size()) {
			this->_items.push_back(item->castObject<MenuItemW32>());
		} else {
			ItemsList::iterator it = _items.begin();
			while (pos--) {
				it ++;
			}
			this->_items.insert(it, item->castObject<MenuItemW32>());
		}
	}

	HMENU MenuW32::build() {
		_menu = CreatePopupMenu();
		
		for (ItemsList::iterator it = _items.begin(); it != _items.end(); it++) {
			it->get()->createItem(this, ++this->_useId);
		}
		
		return _menu;
	}

	// ------------------

	MenuItemW32::MenuItemW32():_id(0) {
	}

	void MenuItemW32::createItem(MenuW32* menu, int& useId) {
		_id = useId;
		MENUITEMINFO mi;
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask = MIIM_STATE | MIIM_ID;
		mi.fState = 0;
		mi.wID = _id;
		this->fillItemInfo(menu, mi);
		InsertMenuItem(menu->getHandle(), 0x7FFF, true, &mi);
	}



	// ------------------

	void MenuItemW32_label::fillItemInfo(MenuW32* menu, MENUITEMINFO& mi) {
		mi.fMask |= MIIM_STRING;
		mi.dwTypeData = (char*)_title.c_str();
		mi.fState = _state;
	}

};