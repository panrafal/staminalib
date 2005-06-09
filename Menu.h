/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once

#include "ObjectImpl.h"
#include "Rect.h"
#include <list>
#include <boost\function.hpp>

namespace Stamina {

	typedef SharedPtr<class iMenu> oMenu;
	typedef SharedPtr<class iMenuItem> oMenuItem;

	class iMenu: public iSharedObject {
	public:

		virtual oMenuItem popupMenu(Point pt, HWND hwnd)=0;

		virtual void insertItem(const oMenuItem& item, int pos = -1)=0;

		virtual bool empty()=0;

	};

	class iMenuItem: public iSharedObject {
	public:

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

	


};