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


#ifndef __STAMINA_LISTWND_IENTRY__
#define __STAMINA_LISTWND_IENTRY__


/* Include files */
#include "..\Rect.h"
#include "..\Menu.h"
#include "ListWnd.h"
#include "iItem.h"
#include "iItemPlacer.h"
namespace Stamina {
namespace ListWnd {
	class ListView;
	class iCollection;
	class iEntry: public iSharedObject
	{

	public:

		STAMINA_OBJECT_CLASS(Stamina::ListWnd::iEntry, iSharedObject);

		virtual bool operator < (iEntry& b) {
			return false;
		}
		virtual bool operator > (iEntry& b) {
			return !(*this < b);
		}

		virtual Size getMinSize()=0;

		virtual Size getMaxSize()=0;

		virtual Size getQuickSize()=0;

		/**Returns size of entry to be set in Item object.
		For collections it shouldn't include sizes of sub elements
		@fitIn Proposed rectangle to fit in
		*/
		virtual Size getEntrySize(ListView* lv,
						   const oItem& li,
						   const oItemCollection& parent,
						   Size fitIn)=0;

		/**Paints the entry in a view*/
		virtual void paintEntry(ListView* lv,
						 const oItem& li,
						 const oItemCollection& parent
						 )=0;

		virtual bool isCollection()=0;

		virtual iCollection* getICollection()=0;

		virtual void repaintEntry(ListView* lv)=0;


		virtual void refreshEntry(ListView* lv,
					   RefreshFlags refresh)=0;

		virtual oItem createItem(ListView* lv,
							 const oItemCollection& collection)=0;

		virtual bool onMouseMove(ListView* lv, const oItem& li, int level, int vkey, const Point& pos)=0;
		virtual bool onMouseDown(ListView* lv, const oItem& li, int level, int vkey, const Point& pos)=0;
		virtual bool onMouseUp(ListView* lv, const oItem& li, int level, int vkey, const Point& pos)=0;
		virtual bool onMouseDblClk(ListView* lv, const oItem& li, int level, int vkey, const Point& pos)=0;
		virtual bool onKeyDown(ListView* lv, const oItem& li, int level, int vkey, int info)=0;
		virtual bool onKeyUp(ListView* lv, const oItem& li, int level, int vkey, int info)=0;

		virtual bool onContextMenu(ListView* lv, const oItem& li, int level, int vkey, const Point& pos, const oMenu& menu)=0;

		virtual void onFlagChanged(ListView* lv, iItem* li, ItemFlags flag)=0;


	};/* END INTERFACE DEFINITION IEntry */


	// -----------------------------------------------------------

	class oEntry: public SharedPtr<class iEntry> {
	public:
		oEntry():SharedPtr<class iEntry>(){}
		oEntry(const oEntry& b):SharedPtr<class iEntry>(b){}
		oEntry(iEntry* b):SharedPtr<class iEntry>(b){}
	};
//	typedef SharedPtr<class iEntry> oEntry;
//	typedef SharedPtr<class iCollection> oCollection;

/*	operator oCollection(const oEntry& b) {
		S_ASSERT(b->getICollection());
		return oCollection((iCollection*)*b);
	}*/

	// -----------------------------------------------------------

	class iCollection: public iEntry
	{

	public:
		STAMINA_OBJECT_CLASS(Stamina::ListWnd::iCollection, iEntry)

		virtual void insertEntry(ListView* lv,
							  const oEntry& entry,
							  int pos=-1, class ItemList* created=0)=0;

		virtual void removeEntry(ListView* lv,
						   const oEntry& entry, bool recurse)=0;

		virtual void removeAll(ListView* lv)=0;

		virtual void updateItemsList(ListView* lv,
							   const oItemCollection& item)=0;

		virtual void onItemInsert(ListView* lv,
							   const oItemCollection& coll, const oItem& item)=0;

		virtual oItemPlacer getItemPlacer(ListView* lv,
									  const oItemCollection& coll)=0;

		virtual Rect getItemsRect(ListView* lv, const oItemCollection& item)=0;
	};/* END INTERFACE DEFINITION ICollection */

//	class iCollection: public iEntry, public iCollection_iface {
//	};
	class oCollection: public SharedPtr<class iCollection> {
	public:
		oCollection():SharedPtr<class iCollection>(){}
		oCollection(const oCollection& b):SharedPtr<class iCollection>(b){}
		oCollection(iCollection* b):SharedPtr<class iCollection>(b){}
		oCollection(const oEntry& b){
			S_ASSERT(b->getICollection());
			this->set((iCollection*)&*b);
		} 
		operator oEntry() const{
			return this->get();
		} 
	};


	// -----------------------------------------------------------




} /* ListWnd */

} /* Stamina */


#endif /* __IENTRY__ */
