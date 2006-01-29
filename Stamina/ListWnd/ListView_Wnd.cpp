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

#define _WIN32_WINNT 0x501
#include <windows.h>

#include "ListView.h"
#include "ItemPlacer.h"
#include "ItemWalk.h"
#include "..\ThreadInvoke.h"
#include "boost\function.hpp"
#include "boost\bind.hpp"
#include "..\WinHelper.h"
namespace Stamina
{
namespace ListWnd
{

	void ListView::onCreateWindow() {
		SetWindowLong(this->_hwnd, GWL_USERDATA, (LONG) this);

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = 0;
		si.nMax = 100;
		si.nPage = 50;
		si.nPos = 0;
		SetScrollInfo(this->_hwnd, SB_VERT, &si, true);
		SetScrollInfo(this->_hwnd, SB_HORZ, &si, true);

	}

	void ListView::alwaysShowScrollbars(bool horz, bool vert) {
		this->scrollbarHAlwaysVisible = horz;
		this->scrollbarVAlwaysVisible = vert;
		ShowScrollBar(this->_hwnd, horz, horz || this->_hscroll);
		ShowScrollBar(this->_hwnd, vert, vert || this->_vscroll);
	}


	int ListView::onPaint() {
		ObjLocker lock(this, lockWrite);
		PAINTSTRUCT ps;
		GetUpdateRgn(this->_hwnd, this->_paintRgn, false);
		this->_paintNeeded = false;
		HDC origDC = BeginPaint(this->_hwnd, &ps);
        this->_paintdc = CreateCompatibleDC(origDC);
		HBITMAP memBmp = CreateCompatibleBitmap(origDC , this->_viewSize.w, this->_viewSize.h);
		HGDIOBJ oldBmp = SelectObject(this->_paintdc , memBmp);
        if (oldBmp) DeleteObject(oldBmp);

		this->repaintView_safe();
		/*HBRUSH hbr = CreateSolidBrush(rand()*0xFFFFFF);
		this->_paintRgn.dcFrame(this->_paintdc, hbr, 1, 1);
		DeleteObject(hbr);*/
		this->_paintRgn.clear();

		BitBlt(origDC , 0 , 0 , this->_viewSize.w , this->_viewSize.h
			, this->_paintdc , 0 , 0 , SRCCOPY);
        //   DeleteObject(memRgn);
		DeleteDC(this->_paintdc);
        DeleteObject(memBmp);

		EndPaint(this->_hwnd, &ps);
		this->_paintdc = 0;
		return 0;
	}
	void ListView::onSize(const Size& newSize) {
		updateClientSize();
		this->refreshItems(refreshSize);
		//this->_rootItem->_wholeSize = newSize;
		updateScrollbars();
	}
	void ListView::onMouseWheel(short distance, short vkey, short x, short y) {
		onVScroll(0, (distance>0?SB_PAGEUP:SB_PAGEDOWN), 0);
	}


	void ListView::onVScroll(short trackPos, short request, HWND ctrl) {
		Point pos = this->getScrollPos();
		pos.y = parseScrollRequest(this->_hwnd, SB_VERT, trackPos, request, 10);
		this->scrollTo(pos);
	}
	void ListView::onHScroll(short trackPos, short request, HWND ctrl) {
		Point pos = this->getScrollPos();
		pos.x = parseScrollRequest(this->_hwnd, SB_HORZ, trackPos, request, 10);
		this->scrollTo(pos);
	}


	template <class IF>
	class WalkItemNotification {
	public:
		WalkItemNotification(IF& func):_func(func) {
		}
		void operator()(const oItem& item, ItemWalk& walk) {
			int count = walk.getCount();
			ListView* lv = walk.getListView();
			if (_func(*item->getEntry().get(), lv, item, count) == false) {
				walk.stop();
			}
		}
	private:
		IF& _func;
	};
	template <class IF>
	bool walkItemNotification(ListView*lv, Point pos, IF& f) {
		WalkItemNotification<IF> win(f);
		ItemList list;
		lv->getItemsAt(pos, list);
		if (list.empty()) return true;
		return ItemWalk::walk(lv, list, win, oItem(0), oItem(0), false, false);
	}

	void ListView::onMouseMove(int vkey, const Point& pos) {
		Point itemPos = clientToItem(pos);
		if (!walkItemNotification(this, itemPos, boost::bind<bool>(&iEntry::onMouseMove, _1, _2, _3, _4, vkey, boost::ref(itemPos)))) {
			return;
		}
	}
	void ListView::onMouseDown(int vkey, const Point& pos) {
		Point itemPos = clientToItem(pos);
		if (!walkItemNotification(this, itemPos, boost::bind<bool>(&iEntry::onMouseDown, _1, _2, _3, _4, vkey, boost::ref(itemPos)))) {
			return;
		}
		bool itemHit = false;
		if (vkey & MK_LBUTTON) {
			this->lockPaint();
			oItem item = this->getItemAt(itemPos);
			bool shift = (vkey & MK_SHIFT) != 0;
			bool ctrl = (vkey & MK_CONTROL) != 0;
			oItem oldActive = this->getActiveItem();
			if ((!shift) || (shift && ctrl)) {
				this->setActiveItem( item );
			}
			if (!ctrl) {
				selectionToActive();
			}
			if (item) {
				itemHit = item->getRect().contains(itemPos);
				if (shift && oldActive.isValid()) {
					ItemWalk::walk(this
						, boost::bind(Item::setSelected
							, boost::bind(oItem::get, _1)
							, boost::bind(ItemWalk::getListView,_2), true)
						, oldActive, item, true, true);
				} else if (ctrl/* && item != oldActive*/) {
					item->setSelected(this, !item->isSelected());
				} else {
					//item->setSelected(this, true);
				}
			}
			if (itemHit)
				this->scrollToActive();
			this->unlockPaint();
		} else if (vkey & MK_RBUTTON) {
			// context
			this->onContextMenu(vkey, pos);
		}
	}
	void ListView::onMouseUp(int vkey, const Point& pos) {
		Point itemPos = clientToItem(pos);
		if (!walkItemNotification(this, itemPos, boost::bind<bool>(&iEntry::onMouseUp, _1, _2, _3, _4, vkey, boost::ref(itemPos)))) {
			return;
		}
	}
	void ListView::onMouseDblClk(int vkey, const Point& pos) {
		Point itemPos = clientToItem(pos);
		if (!walkItemNotification(this, itemPos, boost::bind<bool>(&iEntry::onMouseDblClk, _1, _2, _3, _4, vkey, boost::ref(itemPos)))) {
			return;
		}
	}

	void ListView::onContextMenu() {
		oItem item = this->getActiveItem();
		if (item) {
			this->onContextMenu(MK_RBUTTON, item->getRect().getCenter());
		}
	}
	void ListView::onContextMenu(int mkey, const Point& pos) {
		Point itemPos = clientToItem(pos);
		oItem item = this->getItemAt(itemPos);
		this->setActiveItem(item);
		if (item)
			this->scrollToActive();
		oMenu menu = this->getContextMenu();
		if (!menu) return;
		walkItemNotification(this, itemPos, boost::bind<bool>(&iEntry::onContextMenu, _1, _2, _3, _4, mkey, boost::ref(itemPos), menu));
		if (!menu->empty()) {
			menu->popupMenu(clientToScreen(pos), this->getHwnd());
		}
	}

	void ListView::onKeyDown(int vkey, int info) {
		if (this->getActiveItem()) {
			Point pos = this->getActiveItem()->getRect().getPos();
			if (!walkItemNotification(this, pos, boost::bind(&iEntry::onKeyDown, _1, _2, _3, _4, vkey, info))) {
				return;
			}
		}
		bool shift = getKeyState(VK_SHIFT);
		bool ctrl = getKeyState(VK_CONTROL);
		switch(vkey) {
			case VK_UP: case VK_DOWN: case VK_NEXT: case VK_PRIOR:
			case VK_HOME: case VK_END: {
				this->lockPaint();
				oItem item;
				if (!this->getActiveItem()) {
					item = walkFind(this, WalkFind_nullOp());
				} else if (vkey == VK_HOME) {
					item = walkFind(this, WalkFind_nullOp());
				} else if (vkey == VK_END) {
					item = walkFind(this, WalkFind_nullOp(), false);
				} else {
					int delta=0;
					switch (vkey) {
						case VK_UP: delta = -1;break;
						case VK_DOWN: delta = 1;break;
						case VK_PRIOR: delta = -10;break;
						case VK_NEXT: delta = 10;break;
					}
					item = this->getActiveItem()->getNeighbour(this, delta);
				}
				if (item) {
					this->setActiveItem(item);
				//	if (!shift && !ctrl) {
						this->selectionToActive();
						this->scrollToActive(false);
						
				//	}
				}
				this->unlockPaint();
				
				break;}
			case VK_F10:
				if (!shift || ctrl)
					break;
			case VK_APPS:
				this->onContextMenu();
				break;
		}
	}
	void ListView::onKeyUp(int vkey, int info) {
		if (this->getActiveItem()) {
			Point pos = this->getActiveItem()->getRect().getPos();
			if (!walkItemNotification(this, pos, boost::bind(&iEntry::onKeyUp, _1, _2, _3, _4, vkey, info))) {
				return;
			}
		}
	}



	void ListView::registerWindowClass() {
		static bool once = false;
		if (once) return;
		once = true;
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style		= CS_DBLCLKS/* | CS_PARENTDC*/;
		wcex.lpfnWndProc	= (WNDPROC)windowProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 4;
		wcex.hInstance		= Stamina::getInstance();
		wcex.hIcon			= 0;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= GetSysColorBrush(COLOR_WINDOW);
		wcex.lpszMenuName	= "";
		wcex.lpszClassName	= windowClassName;
		wcex.hIconSm = 0 ;
		RegisterClassEx(&wcex);
	}

	ListView * ListView::fromHWND(HWND wnd) {
		ListView * lv = (ListView*)GetWindowLong(wnd, GWL_USERDATA);
		S_ASSERT_MSG(lv, "Stamina::ListWnd::ListView control is not initialized properly!");
		return lv;
	}

	int CALLBACK ListView::windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		switch (message) {
			case WM_CREATE:{
				CREATESTRUCT * cs = (CREATESTRUCT*)lParam;
				ListView * lv = (ListView*) cs->lpCreateParams;
				if (!lv) 
					lv = new ListView(hwnd);
				lv->_hwnd = hwnd;
				lv->onCreateWindow();
				break;}
			case WM_SIZE:
				fromHWND(hwnd)->onSize(Size(LOWORD(lParam), HIWORD(lParam)));
			    break;
			case WM_ERASEBKGND:
				return 0;
			case WM_PAINT:
				return fromHWND(hwnd)->onPaint();
			case WM_DESTROY:
				delete fromHWND(hwnd);
				break;
			case WM_MOUSEWHEEL:
				fromHWND(hwnd)->onMouseWheel(HIWORD(wParam), LOWORD(wParam), HIWORD(lParam), LOWORD(lParam));
				return 0;
			case WM_VSCROLL:
				fromHWND(hwnd)->onVScroll(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
				return 0;
			case WM_HSCROLL:
				fromHWND(hwnd)->onHScroll(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
				return 0;
			case WM_GETDLGCODE:
				return DLGC_WANTARROWS | DLGC_WANTCHARS;

			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				fromHWND(hwnd)->onMouseDown(wParam, Point(LOWORD(lParam), HIWORD(lParam)));
				return 0;
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				fromHWND(hwnd)->onMouseUp(wParam, Point(LOWORD(lParam), HIWORD(lParam)));
				return 0;
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
				fromHWND(hwnd)->onMouseDblClk(wParam, Point(LOWORD(lParam), HIWORD(lParam)));
				return 0;
			case WM_MOUSEMOVE:
				fromHWND(hwnd)->onMouseMove(wParam, Point(LOWORD(lParam), HIWORD(lParam)));
				return 0;

			case WM_KEYDOWN:
				fromHWND(hwnd)->onKeyDown(wParam, lParam);
				return 0;
			case WM_KEYUP:
				fromHWND(hwnd)->onKeyUp(wParam, lParam);
				return 0;

			case WM_CONTEXTMENU:
				if (lParam == -1) {
//					fromHWND(hwnd)->onContextMenu();
				} else {
//					fromHWND(hwnd)->onContextMenu(MK_RBUTTON, Point::fromLParam(wParam));
				}
				return 0;


			case WM_MOUSEACTIVATE:
				SetFocus(hwnd);
				return MA_ACTIVATE;



		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}


};
};