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


#ifndef __ENTRY__
#define __ENTRY__


/* Include files */
#include "iEntry.h"
#include "..\ObjectImpl.h"

namespace Stamina
{
namespace ListWnd
{
	class Entry : public SharedObject<iCollection>
	{
	public:
		bool isCollection();
		iCollection* getICollection();
		void refreshEntry(ListView* lv, RefreshFlags refresh);
		void repaintEntry(ListView* lv) {
			this->refreshEntry(lv, refreshPaint);
		}
		oItem createItem(ListView* lv, const oItemCollection& collection);
		/**Removes all occurences of this entry in the given listview*/
		void remove(ListView* lv);

		bool onMouseMove(ListView* lv, const oItem& li, int level, int vkey, const Point& pos) {return true;}
		bool onMouseDown(ListView* lv, const oItem& li, int level, int vkey, const Point& pos) {return true;}
		bool onMouseUp(ListView* lv, const oItem& li, int level, int vkey, const Point& pos) {return true;}
		bool onMouseDblClk(ListView* lv, const oItem& li, int level, int vkey, const Point& pos) {return true;}
		bool onKeyDown(ListView* lv, const oItem& li, int level, int vkey, int info) {return true;}
		bool onKeyUp(ListView* lv, const oItem& li, int level, int vkey, int info) {return true;}

		virtual bool onContextMenu(ListView* lv, const oItem& li, int level, int vkey, const Point& pos, const oMenu& menu) {
			return true;
		}


		void onFlagChanged(ListView* lv, iItem* li, ItemFlags flag) {
		}

		operator oEntry() {
			return oEntry(this);
		}
	};/* END CLASS DEFINITION Entry */

	class EntryImpl : public Entry {
	private:
		void insertEntry(ListView* lv, const oEntry& entry, int pos, class ItemList* created) {
			S_ERROR("placeholder");
		}
		void removeEntry(ListView* lv, const oEntry& entry, bool recurse) {
			S_ERROR("placeholder");
		}
		void removeAll(ListView* lv) {
			S_ERROR("placeholder");
		}
		void updateItemsList(ListView* lv, const oItemCollection& item) {
			S_ERROR("placeholder");
		}
		void onItemInsert(ListView* lv, const oItemCollection& coll, const oItem& item) {
			S_ERROR("placeholder");
		}

		oItemPlacer getItemPlacer(ListView* lv, const oItemCollection& coll) {
			S_ERROR("placeholder");
			return oItemPlacer();
		}
		Rect getItemsRect(ListView* lv, const oItemCollection& item) {
			S_ERROR("placeholder");
			return Rect();
		}

	};

	// -----------------------------------------------------

    /** Collection */
	class Collection : public Entry/*, public iCollection_iface*/
	{
	public:
		iCollection* getICollection();
		oItem createItem(ListView* lv, const oItemCollection& collection);
		/**Inserts subentry into all occurences of this entry in given ListView*/
		void insertEntry(ListView* lv, const oEntry& entry, int pos, ItemList* created=0);
		/**Removes subentry from all occurences of this entry in given ListView*/
		void removeEntry(ListView* lv, const oEntry& entry, bool recurse);
		/**Removes all subentrys from all occurences of this entry in given ListView*/
		void removeAll(ListView* lv);
		void updateItemsList(ListView* lv, const oItemCollection& item) {
			/*intentionally left empty*/
		}
		void onItemInsert(ListView* lv,	const oItemCollection& coll, const oItem& item) {
			/*intentionally left empty*/
		}


		oItemPlacer getItemPlacer(ListView* lv, const oItemCollection& coll);
		Rect getItemsRect(ListView* lv, const oItemCollection& item);

		operator oCollection() {
			return oCollection(this);
		}

	};/* END CLASS DEFINITION Collection */


} /* ListWnd */

} /* Stamina */


#endif /* __ENTRY__ */
