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


#ifndef __STAMINA_LISTWND_ITEM__
#define __STAMINA_LISTWND_ITEM__


/* Include files */
#include "..\Rect.h"
#include "..\ObjectImpl.h"

#ifndef __STAMINA_LISTWND_IITEM__OBJECT_BY_IFACE__
namespace Stamina {
namespace ListWnd {
/*	class oItem : public SharedPtr<class Item>
	{
	};
	class oItemCollection : public SharedPtr<class ItemCollection>
	{
	};*/
	typedef SharedPtr<class Item> oItem;
	typedef SharedPtr<class ItemCollection> oItemCollection;


//	oItemCollection& operator = (oItemCollection& coll, const oItem& item);

};
};
#endif


#include "iItem.h"
#include "iEntry.h"
#include "ListWnd.h"
#include <list>
#include <boost\function.hpp>

namespace Stamina {
namespace ListWnd {



	class ItemCollection;

	class Item: public SharedObject<iItem>
	{
	protected:

		oEntry _entry;
		ItemState _itemState;
		ItemFlags _itemFlags;
		Point _pos;
		Size _size;
		unsigned char _level;

	public:

		Item(const oEntry& entry);


		/**Returns the size of a whole item (with subitems).
		@param accurate If size wasn't calculated before, allows to return iEntry::getQuickSize() instead of costly calculating
		*/
		virtual Size getWholeSize(bool accurate = true);

		/**Returns previously calculated size of an item (without subitems!)*/
		Size getSize();

		virtual void setState(ItemState state);

		ItemState getState();

		virtual void setFlag(ItemFlags flag, bool setting, ListView* lv=0);

		bool getFlag(ItemFlags flag);

		oEntry getEntry();

		Rect getRect();

		Rect getWholeRect(bool accurate = false);

		//virtual bool setRect(Rect rect);

		/**Sets flags checked in ListView::refreshItems().
		That's the only proper way to change the size of an item...
		One have to request for size refresh (with setRefreshFlag() refreshSize ) and then issue ListView::refreshItems(). ListView will iterate through the list refreshing all requested data, keeping the items properly positioned and painted...
		@warning If You use setRefreshFlag(refreshPaint) instead of setRepaint(), the item will be repainted only after issuing ListView::refreshItems() (which leads to the risk, that item could be repainted only partially in the meantime)...
		*/
		virtual void setRefreshFlag(RefreshFlags refresh, bool setting=true);

		/**Requests repaint for area occupied by the item (if visible in view).
		*/
		virtual void repaint(ListView* lv, bool whole=false);

		oItemCollection getParent(ListView* lv);

		virtual ItemCollection* getItemCollection();


		Point getPos();

		unsigned char getLevel() {
			return this->_level;
		}

		void setVisible(ListView* lv, bool visible);

		bool isVisible();

		void setSelected(ListView* lv, bool selected) {
			this->setFlag(flagSelected, selected, lv);
		}
		void setSelected(bool selected) {
			this->setSelected(0, selected);
		}


		bool isPositioned();

		inline bool isSelected() {
			return this->getFlag(flagSelected);
		}

		inline bool isActive() {
			return this->getFlag(flagActive);
		}

		inline bool needRepaint() {
			return this->getFlag(flagRepaint);
		}

		inline bool needResize() {
			return this->getFlag(flagResize);
		}

		inline bool needRepos() {
			return this->getFlag(flagRepos);
		}

		inline bool isForExpand() {
			return this->getFlag(flagForExpand);
		}

		inline bool notForExpand() {
			return this->getFlag(flagForExpand) == false;
		}

		operator oItem() {
			return oItem(this);
		}

		oItem getNeighbour(ListView* lv, int delta);

		virtual bool containsActiveItem(ListView* lv);

		virtual bool operator < (Item& b) {
			return *this->_entry < *b._entry;
		}
		virtual bool operator > (Item& b) {
			return *this->_entry > *b._entry;
		}


	protected:


		/* *Calculates,or returns stored size of a whole item. Subitems of collections are included.
		
		@param lv ListView this item belongs to. If omitted, the size won't be calculated if not present
		@param accurate If size wasn't calculated before, allows to return iEntry::getQuickSize() instead of costly calculating
		*/
//		virtual Size calculateSize(ListView* lv);

		bool setSize(Size size);

		bool setPos(Point pos);

		friend class ItemPlacer;

		virtual void setLevel(char level) {
			this->_level = level;
		}

		friend class ItemCollection;

	};/* END CLASS DEFINITION Item */


	// ---------------------------------------------------


	// ---------------------------------------------------
	class ItemList: public std::list<oItem> 
	{
	public:
		/*
		iterator insert(iterator where, oItem& item);
		void push_front(oItem& item);
		void push_back(oItem& item);
		*/
		unsigned int count();

	private:
		//void prepareItem(oItem& item);
	};

	typedef std::pair<oItemCollection, oItem> ParentItemPair;
	typedef std::list<ParentItemPair> ParentItemList;  
	typedef boost::function<bool (const oItem& a, const oItem& b)> fCompareItems;

	// --------------------------------------------------------------
/*TODO: Sprawdziæ, jak dzia³a takie dziedziczenie interfejsów*/

	/** Collection of Items */
	class ItemCollection : public Item/*, public iItemCollection_iface*/
	{
	private:

		ItemList _items;

		Size _wholeSize;

	public:

		ItemCollection(const oCollection& entry);

		ItemCollection* getItemCollection();

		ItemList& getItemList();

		oItem getEntryItem(const oEntry& entry, bool recurse = true);

		bool getEntryItems(const oEntry& entry,
					 ItemList &list);

		bool getEntryItems(const oEntry& entry,
					 ParentItemList &list);

		oItem getItemAt(Point pos, bool recurse = true, bool hitWhole = true);

		void setState(ItemState state);

		void setFlag(ItemFlags flag, bool setting, ListView* lv=0);



		int getItemIndex(const oItem& item);

		bool containsItem(const oItem& item);

		oItemCollection findItem(const oItem& item);

		/**Inserts entry as a subitem of this collection (Even if there IS one already!).
		@param pos - position where entry should be inserted. 0 means beginning, positive values count places from beginning, -1 means at end, negative means places from end minus 1 (-2 means one before last)
		@return Newly created Item
		*/
		oItem insertEntry(ListView* lv,
					  const oEntry& entry,
					  int pos=-1);



		void removeEntry(ListView* lv,
				   const oEntry& entry, bool recurse = true);

		void removeItem(ListView* lv,
				  const oItem& item);

		void removeAll(ListView* lv);

		void getItemsAt(Point pt,
				  ItemList &items);

		Size getWholeSize(bool accurate = true);

		operator oItemCollection() const {
			return oItemCollection((ItemCollection*)this);
		}

		inline bool isExpandable() {
			return this->getFlag(flagExpandable);
		}

		inline bool isExpanded() {
			return !this->isExpandable() || this->getFlag(flagExpanded);
		}

		void setExpanded(ListView* lv, bool expanded);

		bool containsActiveItem(ListView* lv);

		virtual fCompareItems getCompareFunction() {
			return fCompareItems();
		}

		void sortItems(ListView* lv);

		int countVisible();

	protected:
		/** Refreshes item's subitems.
        Iterates through list of subitems, forcing refresh of passed parameter (size/pos)
		or refreshing depending on flags set on item...
		@param parent - parent of this item, for performance...
		@return true if anything has changed (involves repainting later on)
		@warning Shouldn't be ever called directly, especially from another thread... Use ListView::refreshItems.
		*/
		bool refreshItems(ListView* lv,
					RefreshFlags refresh, const oItemCollection& parent);

		/**Repaints item and it's subitems.
		@param force Forces to repaint every item currently in view... Otherwise only items with flagRepaint set are repainted.
		*/
		void repaintItems(ListView* lv);

		virtual oItem insertItem(ListView* lv, const oItem& item, int pos);

		bool setWholeSize(Size size);

		void setLevel(char level);

		friend class ItemPlacer;

	};/* END CLASS DEFINITION ItemCollection */
	// ---------------------------------------------------

	

	inline bool operator < (const oItem& a, const oItem& b) {
		return *a < *b;
	}

	inline bool operator > (const oItem& a, const oItem& b) {
		return *a > *b;
	}




} /* ListWnd */

} /* Stamina */


#endif /* __ITEM__ */
