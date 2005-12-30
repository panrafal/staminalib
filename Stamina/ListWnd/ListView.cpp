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
	const char * const ListView::windowClassName = "Stamina::ListWnd::ListView";

	ListView::ListView(int x , int y , int w , int h , HWND parent , HMENU id)
	{
		registerWindowClass();
		this->init();
		CreateWindowEx(0, windowClassName, "", 
			WS_CHILD | WS_TABSTOP | WS_VISIBLE, 
			x, y, w, h, parent, id, Stamina::getInstance(), this);
	}
	ListView::ListView(HWND hwnd)
	{
		this->init();
		this->_hwnd = hwnd;
	}
	void ListView::init() {
		this->_hwnd = 0;
		this->_dc = 0;
    	this->_paintdc = 0;
		this->_paintRgn = 0;
		this->_dcCount = 0;
		this->_paintLockCount = 0;
		this->_refreshLockCount = 0;
		this->_rootItem.set(new LVItemCollection(this));
		this->_defaultItemPlacer.set(new ItemPerRow());
		this->_defaultViewItemPlacer.set(new ItemPerRow());
		this->_paintNeeded = this->_refreshNeeded = false;
		this->_vscroll = _hscroll = false;
		this->_vscrollMult = 10;
		this->_hscrollMult = 2;
		this->_scrollWholeItems = false;

		this->scrollbarVAlwaysVisible = false;
		this->scrollbarHAlwaysVisible = false;
	}



    
	void ListView::setActiveItem(const oItem& item)
	{
		{
			ObjLocker lock(this, lockWrite);
			if (this->_activeItem == item) return;
			this->lockPaint();
			this->lockRefresh();
			/* ka¿emy im siê tylko odœwie¿yæ... */
			oItem oldActive = this->_activeItem;
			this->_activeItem = item;
			if (oldActive) {
				oldActive->setFlag(flagActive, false, this);
				//oldActive->repaint(this);
			}
			if (item) {
				this->_activeItem->setFlag(flagActive, true, this);
				//this->_activeItem->repaint(this);
			}
			this->evtActiveItemChanged(this, oldActive);

			this->refreshItems();
			
			this->unlockRefresh();
			this->unlockPaint();
		}
	}
	void ListView::selectionToActive() {
		lockPaint();
		ItemWalk::walk(this
			, boost::bind(Item::setSelected
				, boost::bind(oItem::get, _1)
				, boost::bind(ItemWalk::getListView,_2), false)
			, oItem(), oItem(), true, false);
		if (this->_activeItem)
			this->_activeItem->setSelected(this, true);
		unlockPaint();
	}

	void ListView::scrollToActive(bool leaveSpace)
	{
		Point pos;
		{
			ObjLocker lock(this, lockWrite);
			if (!this->_activeItem) return;
			InView in = this->isInView(this->_activeItem);
			pos = this->getScrollPos();
			Point itemPos = this->_activeItem->getPos();
			if (inViewWidthPartial(in)) { // skoro jest czêœciowy, trzeba go ustawiæ...
                 pos.x = itemPos.x;               
			}
			if (in & inviewHeightOversized) {
				pos.y = itemPos.y;
			} else if (!(in & inviewHeight)) {
				int itemH = this->_activeItem->getSize().h;
				int viewH = this->getViewSize().h;
				if (itemH > viewH) {
					pos.y = itemPos.y;
				} else {
					if (itemPos.y < pos.y) { // na górze..
						pos.y = itemPos.y - (leaveSpace ? min(viewH - itemH, 50) : 0);
					} else {
						pos.y = itemPos.y - viewH + itemH + (leaveSpace ? min(viewH - itemH, 50) : 0);
					}
				}
			}
		}
		this->scrollTo(pos);		
	}

	InView ListView::isInView(oItem itemObj) {
		ObjLocker lock(this, lockRead);
		S_ASSERT(itemObj.isValid());
		/*Lepiej przewin¹æ siê nie da, wiêc musimy pozwalaæ na czêœciowe*/
		/*if (this->getScrollPos() == item->getPos()) {
			canBePartial = true;
		}*/
		Rect view = this->getViewRect();
		Rect item = itemObj->getRect();
		if (!view.intersects(item))
			return inviewNone;
		InView in = inviewPartial;
		if (view.containsW(item))
			in = (InView)(in | inviewWidth);
		else if (item.containsW(view))
			in = (InView)(in | inviewWidthOversized);
		if (view.containsH(item))
			in = (InView)(in | inviewHeight);
		else if (item.containsH(view))
			in = (InView)(in | inviewHeightOversized);
		return in;
	}


	void ListView::scrollBy(Point offset)
	{
		this->scrollTo(this->getScrollPos() + offset);
	}

	void ListView::scrollTo(Point pos)
	{
		if (_creationThread.isCurrent())
			this->scrollTo_safe(pos);
		else {
			threadInvoke(_creationThread
				, boost::function<void()>(boost::bind(&ListView::scrollTo_safe, this, pos)), true);
		}
	}
	void ListView::scrollTo_safe(Point newPos) {
		Size size = this->getWholeSize(); 
		if (newPos.y < 0) newPos.y = 0;
		if (newPos.y > (size.h - _viewSize.h)) {
			if (size.h > _viewSize.h)
				newPos.y = (size.h - _viewSize.h);
			else
				newPos.y = 0;
		}

		/**@todo to jest ostro nie tak jak byæ powinno...  */
		if (this->_scrollWholeItems) {
			oItem item = this->getItemAt(Point( size.w / 2, newPos.y ), true, false);
			if (item) {
				Rect irc = item->getRect();
				if (newPos.y <= irc.getCenter().y) {
					newPos.y = irc.top;
				} else {
					newPos.y = irc.bottom;
				}
			}
		}

		Point pos = this->_scrollPos;
		this->_scrollPos = newPos; // ¿eby repaint nie zg³upial
		int dx = newPos.x - pos.x;
		int dy = newPos.y - pos.y;
		ScrollWindow(this->_hwnd , -dx , -dy , 0 , 0);
		Rect r(0, 0, _viewSize.w, 0);
		if (newPos.y < pos.y) {
			r.bottom = _viewSize.h;
			r.top = _viewSize.h - dy;
		} else {
			r.bottom = 0;
			r.top = -dy;
		}
		if (dy)
			this->repaintRect(r); 
		r = Rect(0, 0, 0, _viewSize.h);
		if (newPos.x < pos.x) {
			r.right = _viewSize.h;
			r.left = _viewSize.h - dx;
		} else {
			r.left = 0;
			r.right = -dx;
		}
		if (dx)
			this->repaintRect(r); 

		this->updateScrollbars();
		/*TODO:scroll*/
	}


	void ListView::lockRefresh()
	{
		this->_refreshLockCount++;		
	}

	void ListView::unlockRefresh(bool refresh)
	{
		S_ASSERT(this->_refreshLockCount > 0);		
		this->_refreshLockCount--;		
		if (refresh && this->_refreshLockCount == 0 && this->_refreshNeeded)
			this->refreshItems();
	}

	bool ListView::canRefresh()
	{
		return this->_refreshLockCount == 0;		
	}

	HDC ListView::getDC()
	{
		ObjLocker lock(this, lockWrite);
		this->_dcCount++;
		return this->_paintdc ? this->_paintdc : this->_dc ? this->_dc : this->_dc = GetDC(this->_hwnd);	
	}

	void ListView::releaseDC(HDC dc)
	{
		ObjLocker lock(this, lockWrite);
		S_ASSERT(this->_dcCount > 0);
		this->_dcCount--;
		if (dc == this->_paintdc) return;
		S_ASSERT(this->_dc == dc);
		if (this->_dcCount == 0) {
			ReleaseDC(this->_hwnd, dc);
			this->_dc = 0;
		}
	}

	void ListView::refreshItems(RefreshFlags refresh) {
		ObjLocker lock(this, lockWrite);
		if (!this->canRefresh()) {
			this->_refreshNeeded = true;
			return;
		}
		this->_refreshNeeded = false;
		if (_creationThread.isCurrent())
			this->refreshItems_safe(refresh);
		else {
			threadInvoke(_creationThread
				, boost::function<void()>(boost::bind(&ListView::refreshItems_safe, this, refresh)), true);
		}
	}
	void ListView::repaintView() {
		ObjLocker lock(this, lockWrite);
		if (!this->canPaint()) {
			this->_paintNeeded = true;
			return;
		}
		this->_paintNeeded = false;
		if (_creationThread.isCurrent())
			UpdateWindow(this->_hwnd);
		else {
			threadInvoke(_creationThread
				, boost::bind(&ListView::repaintView, this), true);
			return;
		}

		
/*		if (_creationThread.isCurrent())
			this->repaintView_safe(force);
		else {
			ThreadInvoke<boost::function<void()> >
				::invoke(_creationThread
				, boost::function<void()>(boost::bind(&ListView::repaintView_safe, this, force)), true);
		}*/
	}
	void ListView::repaintClient() {
		InvalidateRect(_hwnd, 0, false);
		repaintView();
	}

	void ListView::repaintRect(const Rect& rect) {
		InvalidateRect(_hwnd, &((RECT)rect), false);
		repaintView();
/*		if (this->canPaint()) {
			// wysy³a WM_PAINT
			RedrawWindow(this->_hwnd, &((RECT)rect), 0, RDW_INVALIDATE);
		} else {
			this->_paintRgn += rect;
		}*/
	}

	void ListView::repaintRegion(const Region& region) {
		InvalidateRgn(_hwnd, region, false);
		repaintView();
		/*if (this->canPaint()) {
			// wysy³a WM_PAINT
			RedrawWindow(this->_hwnd, 0, region, RDW_INVALIDATE);
		} else {
			this->_paintRgn += region;
		}*/
	}


	void ListView::repaintItemRect(const Rect& rect) {
		/*TODO:Trzeba przyci¹æ wynikowy prostok¹t do okna...*/
		this->repaintRect(itemToClient(rect));
	}
	void ListView::repaintItemRegion(Region region) {
		region.offset( - this->getScrollPos());
		this->repaintRegion(region);
	}
	bool ListView::inRepaintRegion(const Rect& rc) {
		return this->_paintRgn.contains(rc);
	}

	void ListView::updateClientSize() {
		Rect r;
		GetClientRect(this->_hwnd, r.ref());
		this->_viewSize = r.getSize();
/*		if (this->_vscroll)
			this->_viewSize.w -= GetSystemMetrics(SM_CXVSCROLL);
		if (this->_hscroll)
			this->_viewSize.h -= GetSystemMetrics(SM_CYHSCROLL);*/
	}
	void ListView::updateScrollbars() {
		// czy potrzebujemy pionowy...
		Size size = this->_rootItem->getWholeSize();
		Point pos = this->getScrollPos();
		bool vscroll = (this->_viewSize.h < size.h) || pos.y;
		bool hscroll = this->_viewSize.w < size.w || pos.x;

		if (vscroll) {
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			si.nMin = 0;
			si.nMax = max(pos.y + this->_viewSize.h, size.h);
			si.nPage = this->_viewSize.h;
			si.nPos = pos.y;
			SetScrollInfo(this->_hwnd, SB_VERT, &si, true);
			EnableScrollBar(this->_hwnd, SB_VERT, ESB_ENABLE_BOTH);
		} else {
			EnableScrollBar(this->_hwnd, SB_VERT, ESB_DISABLE_BOTH);
		}
		if (hscroll) {
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			si.nMin = 0;
			si.nMax = size.w;
			si.nPage = this->_viewSize.w;
			si.nPos = pos.x;
			SetScrollInfo(this->_hwnd, SB_HORZ, &si, true);
			EnableScrollBar(this->_hwnd, SB_HORZ, ESB_ENABLE_BOTH);
		} {
			EnableScrollBar(this->_hwnd, SB_HORZ, ESB_DISABLE_BOTH);
		}
		bool repaint = false;
		if (this->_vscroll != vscroll) {
			ShowScrollBar(this->_hwnd, SB_VERT, vscroll || this->scrollbarVAlwaysVisible);
			repaint = true;
		}
		if (this->_hscroll != hscroll) {
			ShowScrollBar(this->_hwnd, SB_HORZ, hscroll || this->scrollbarHAlwaysVisible);
			repaint = true;
		}
		this->_vscroll = vscroll;
		this->_hscroll = hscroll;
		if (pos.y > size.h || pos.x > size.w) {
			scrollTo(pos);
		}
		if (repaint) {
			this->repaintClient();
		}
	}


	void ListView::refreshItems_safe(RefreshFlags refresh)
	{
		this->lockRefresh();
		this->lockPaint();
		oItemCollection parent;
		Size oldSize = this->_rootItem->getWholeSize();
		if (this->_rootItem->refreshItems(this, refresh, parent)) {
			this->repaintView();
		}
		Size newSize = this->_rootItem->getWholeSize();
		if (newSize.h < oldSize.h) {
			this->repaintItemRect(Rect(0, newSize.h, oldSize.w, oldSize.h));
		}
		this->unlockPaint();
		this->unlockRefresh(false); // ¿eby nie wpaœæ przypadkiem w rekurencjê...
		if (newSize.h != oldSize.h) { // dopiero teraz, ¿eby móg³ refreshowaæ w razie czego
			updateScrollbars();
		}
	}

	void ListView::repaintView_safe()
	{
		/*TODO: flicker free powinien iœæ tutaj...*/
		this->lockPaint();
		this->_rootItem->getEntry()->paintEntry(this, *this->_rootItem, oItemCollection(0));

		this->_rootItem->repaintItems(this);	
		this->unlockPaint();
	}

	void ListView::lockPaint()
	{
		this->_paintLockCount++;		
	}

	void ListView::unlockPaint(bool repaint)
	{
		S_ASSERT(this->_paintLockCount > 0);		
		this->_paintLockCount--;
		if (repaint && this->_paintLockCount == 0 && this->_paintNeeded)
			this->repaintView();
	}

	bool ListView::canPaint()
	{
		return this->_paintLockCount == 0;
	}

	Point ListView::itemToClient(const Point& pt)
	{
		return pt - this->getScrollPos();		
	}
	Point ListView::clientToItem(const Point& pt) {
		return pt + this->getScrollPos();		
	}

	Point ListView::screenToClient(const Point& pt)
	{
		RECT rc;
		GetWindowRect(this->_hwnd, &rc);
		return Point(rc.left, rc.top) - pt;
	}

	Point ListView::clientToScreen(const Point& pt) {
		RECT rc;
		GetWindowRect(this->_hwnd, &rc);
		return Point(rc.left, rc.top) + pt;
	}

//------------------------------------------------------------

	Size ListView::LVCollection::getMinSize() {
		return Size();
	}
	Size ListView::LVCollection::getMaxSize() {
		return Size();
	}
	Size ListView::LVCollection::getQuickSize() {
		return Size();
	}
	Size ListView::LVCollection::getEntrySize(ListView* lv, const oItem& li, const oItemCollection& parent, Size fitIn) {
		return Size();
	}
	void ListView::LVCollection::paintEntry(ListView* lv, const oItem& li,				const oItemCollection& parent) {
		HDC dc = lv->getDC();

//		HBRUSH hbr = CreateSolidBrush(0xFF);

		if (lv->_background) {
			lv->_background->drawStretched(dc, lv->getClientRect());
		} else {
			FillRect(dc, lv->getClientRect().ref(), GetSysColorBrush(COLOR_WINDOW));
		}
//		DeleteObject(hbr);

		lv->releaseDC(dc);
	}



} /* ListWnd */

} /* Stamina */

