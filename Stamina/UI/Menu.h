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

#pragma once

#include "../ObjectImpl.h"
#include "../Rect.h"
#include <list>
#include <boost\function.hpp>

namespace Stamina { namespace UI {

	typedef SharedPtr<class iMenu> oMenu;
	typedef SharedPtr<class iMenuItem> oMenuItem;

	class iMenu: public iSharedObject {
	public:

		STAMINA_OBJECT_CLASS_VERSION(iMenu, iSharedObject, Version(1,0,0,0));


		virtual oMenuItem popupMenu(Point pt, HWND hwnd)=0;

		virtual void insertItem(const oMenuItem& item, int pos = -1)=0;

		virtual bool empty()=0;

	};

	class iMenuItem: public iSharedObject {
	public:

		STAMINA_OBJECT_CLASS_VERSION(iMenuItem, iSharedObject, Version(1,0,0,0));

		virtual void onClick(iMenu* menu)=0;

	};

	class MenuW32: public SharedObject<iMenu> {
	public:

		MenuW32();

		oMenuItem popupMenu(Point pt, HWND hwnd);

		void insertItem(const oMenuItem& item, int pos = -1);

		HMENU getHandle() {
			return _menu;
		}

		int size() {
			return _items.size();
		}
		bool empty() {
			return _items.empty();
		}

	protected:

		HMENU build();

		typedef std::list<SharedPtr<class MenuItemW32> > ItemsList;
		ItemsList _items;
		int _useId;
		HMENU _menu;
	};

	// separator
	class MenuItemW32: public SharedObject<iMenuItem> {
	public:

		friend class MenuW32;

		MenuItemW32();
		void onClick(iMenu* menu) {}

	protected:

		virtual void createItem(MenuW32* menu, int& useId);

		virtual void fillItemInfo(MenuW32* menu, MENUITEMINFO& mi)=0;

		virtual oMenuItem selectById(MenuW32* menu, int id) {
			if (_id == id) {
				return this;
			}
			return oMenuItem();
		}

		int _id;

	};

	class MenuItemW32_separator: public MenuItemW32 {
	public:

		void fillItemInfo(MenuW32* menu, MENUITEMINFO& mi) {
			mi.fMask |= MIIM_FTYPE;
			mi.fType = MFT_SEPARATOR;
		}

	};

	class MenuItemW32_label: public MenuItemW32 {
	public:
		typedef boost::function<void(iMenu*, MenuItemW32*)> fOnClick;

		enum {
			itemChecked = MFS_CHECKED,
			itemDefault = MFS_DEFAULT,
			itemDisabled = MFS_DISABLED,
		};

		MenuItemW32_label(const std::string& title, const fOnClick& onClick = fOnClick(), int state = 0):_title(title), _onClick(onClick), _state(state) {
		}

		void onClick(iMenu* menu) {
			if (_onClick)
				this->_onClick(menu, this);
		}


	protected:

		virtual void fillItemInfo(MenuW32* menu, MENUITEMINFO& mi);


		std::string _title;
		fOnClick _onClick;
		int _state;
	};

	
	STAMINA_REGISTER_CLASS_VERSION(iMenu);
	STAMINA_REGISTER_CLASS_VERSION(iMenuItem);


} };
