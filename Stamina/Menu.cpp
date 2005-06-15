/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
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