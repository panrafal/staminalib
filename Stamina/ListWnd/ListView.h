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


#ifndef __LISTVIEW__
#define __LISTVIEW__


/* Include files */
#include "Item.h"
#include "Entry.h"
#include "..\Region.h"
#include "..\Thread.h"
#include "..\Image.h"
#include "..\Menu.h"
#include <boost\signals.hpp>
namespace Stamina {
namespace ListWnd {

	const ModuleVersion listWndVersion (versionModule, "ListWnd", Version(1,0,0,0));

	STAMINA_REGISTER_VERSION(ListWnd, listWndVersion);

	class iListView: public iLockableObject {
	public:
		STAMINA_OBJECT_CLASS(ListWnd::iListView, iLockableObject);
	};
	/*TODO: Oddzieliæ Collection i? ItemCollection*/
	class ListView: public LockableObject<iListView>
	{
	public:
		static const char * const windowClassName;

		class LVCollection: public Collection {
		public:
			friend class ListView;
			LVCollection():Collection() {
			}

			Size getMinSize();
			Size getMaxSize();
			Size getQuickSize();
			Size getEntrySize(ListView* lv, const oItem& li,			const oItemCollection& parent, Size fitIn);
			void paintEntry(ListView* lv, const oItem& li,				const oItemCollection& parent);
			Rect getItemsRect(ListView* lv, const oItemCollection& item) {
				return Rect(Point(0,0), lv->_viewSize);
			}
			oItemPlacer getItemPlacer(ListView* lv,	const oItemCollection& coll) {
				return lv->defaultViewItemPlacer();
			}

		};
		class LVItemCollection: public ItemCollection {
		private:
			ListView* lv;
		public:
			friend class ListView;
			LVItemCollection(ListView* lv):ItemCollection(oCollection(*new LVCollection())), lv(lv) {
				this->setFlag(flagExpandable, false);
			}
			operator oItemCollection() {
				return oItemCollection(this);
			}
			operator oItem() {
				return oItem(this);
			}

		};

		typedef SharedPtr<LVItemCollection> oLVItemCollection;

	public:
		ListView(int x , int y , int w , int h , HWND parent , HMENU id);
		ListView(HWND hwnd);
		static void registerWindowClass();

		void setActiveItem(const oItem& item);

		oItem getActiveItem() {
			return this->_activeItem;
		}
		bool isActiveItem(const oItem& item) {
			return this->_activeItem == item;
		}

		void scrollToActive(bool leaveSpace=true);

		void selectionToActive();

		/** Returns true if item is visible to user */
		InView isInView(oItem item);

		HWND getHwnd() {
			return this->_hwnd;
		}

		void scrollBy(Point offset);

		void lockRefresh();

		void unlockRefresh(bool refresh = true);

		bool canRefresh();

		HDC getDC();

		void releaseDC(HDC dc);

		void refreshItems(RefreshFlags refresh = refreshAuto);

		/**Repaints areas marked for repaint*/
		void repaintView();

		/**Repaints whole client area*/
		void repaintClient();

		/**Repaints (or requests repaint) rectangle in client/view coordinates*/
		void repaintRect(const Rect& rect);

		void repaintRegion(const Region& region);

		/**Repaints (or requests repaint) rectangle in item coordinates*/
		void repaintItemRect(const Rect& rect);

		void repaintItemRegion(Region region);


		/**Checks if given rectangle (in client/view coordinates!) fits in area being repainted.*/
		bool inRepaintRegion(const Rect& rc);

		bool itemInRepaintRegion(const Rect& rc) {
			return inRepaintRegion(itemToClient(rc));
		}

		void lockPaint();

		void unlockPaint(bool repaint = true);

		bool canPaint();

		inline Point itemToClient(const Point& pt);

		Point screenToClient(const Point& pt);

		Point clientToScreen(const Point& pt);

		Point clientToItem(const Point& pt);

		inline Rect itemToClient(const Rect& r) {
			return Rect(itemToClient(r.getLT()), r.getSize());
		}


		void scrollTo(Point pos);

		Point getScrollPos() {
			return this->_scrollPos;
		}
		Rect getViewRect() {
			return Rect(this->_scrollPos , this->_viewSize);
		}
		Size getViewSize() {
			return this->_viewSize;
		}
		Rect getClientRect() {
			return Rect(Point() , this->_viewSize);
		}


		bool duringPaint() {
			return this->_paintdc != 0;
		}

		oItemPlacer& defaultViewItemPlacer() {
			return this->_defaultViewItemPlacer;
		}

		oItemPlacer& defaultItemPlacer() {
			return this->_defaultItemPlacer;
		}

		oLVItemCollection getRootItem() {
			return this->_rootItem;
		}

		void alwaysShowScrollbars(bool horz, bool vert);

		// ----------------------------------------------------------

		ItemList& getItemList() {
			return this->_rootItem->getItemList();
		}

		oItem getEntryItem(oEntry entry, bool recurse = true) {
			return this->_rootItem->getEntryItem(entry, recurse);
		}

		bool getEntryItems(oEntry entry, ItemList &list) {
			return this->_rootItem->getEntryItems(entry, list);
		}

		bool getEntryItems(oEntry entry, ParentItemList &list) {
			return this->_rootItem->getEntryItems(entry, list);
		}

		oItem getItemAt(Point pos, bool recurse = true, bool hitWhole = true) {
			return this->_rootItem->getItemAt(pos, recurse, hitWhole);
		}

		int getItemIndex(oItem item) {
			return this->_rootItem->getItemIndex(item);
		}

		bool containsItem(oItem item) {
			return this->_rootItem->containsItem(item);
		}

		oItemCollection findItem(oItem item) {
			return this->_rootItem->findItem(item);
		}

		oItem insertEntry(oEntry entry, int pos = -1) {
			return this->_rootItem->insertEntry(this, entry, pos);
		}
 
		void removeEntry(oEntry entry, bool recurse = true) {
			return this->_rootItem->removeEntry(this, entry, recurse);
		}

		void removeItem(oItem item) {
			return this->_rootItem->removeItem(this, item);
		}

		void removeAll() {
			return this->_rootItem->removeAll(this);
		}

		void getItemsAt(Point pt, ItemList &items) {
			return this->_rootItem->getItemsAt(pt, items);
		}

		Size getWholeSize(bool accurate = true) {
			return this->_rootItem->getWholeSize(accurate);
		}

		virtual oMenu getContextMenu() {
			return new MenuW32();
		}


	public:

		bool scrollbarHAlwaysVisible;
		bool scrollbarVAlwaysVisible;

	public: // signals
        
		boost::signal<void(ListView* lv, const oItem& old)> evtActiveItemChanged;

	protected:
		void init();
		static ListView * fromHWND(HWND wnd);
		static int CALLBACK windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);
		void onCreateWindow();
		int onPaint();
		void onSize(const Size& newSize);
		void onMouseWheel(short distance, short vkey, short x, short y);
		void onVScroll(short pos, short request, HWND ctrl);
		void onHScroll(short pos, short request, HWND ctrl);
		virtual void onMouseDown(int vkey, const Point& pos);
		virtual void onMouseUp(int vkey, const Point& pos);
		virtual void onMouseDblClk(int vkey, const Point& pos);
		virtual void onKeyDown(int vkey, int info);
		virtual void onKeyUp(int vkey, int info);
		virtual void onMouseMove(int vkey, const Point& pos);

		virtual void onContextMenu();
		virtual void onContextMenu(int mkey, const Point& pos);

		/***/
		void updateScrollbars();
		void updateClientSize();

		void refreshItems_safe(RefreshFlags refresh);
		void repaintView_safe();
		void scrollTo_safe(Point pos);


	protected:

		HWND _hwnd;
		Point _scrollPos;
		HDC _dc;
    	HDC _paintdc;
		Region _paintRgn;
		unsigned int _dcCount;
		int _paintLockCount;
		bool _paintNeeded;
		int _refreshLockCount;
		bool _refreshNeeded;
		oItem _activeItem;
		Size _viewSize;
		oLVItemCollection _rootItem;
		Thread _creationThread;
		oItemPlacer _defaultItemPlacer;
		oItemPlacer _defaultViewItemPlacer;
		bool _vscroll, _hscroll;
		unsigned char _vscrollMult, _hscrollMult;
		bool _scrollWholeItems;

		oImage _background;

	};/* END CLASS DEFINITION ListView */



} /* ListWnd */

} /* Stamina */


#endif /* __LISTVIEW__ */
