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

#include "Item.h"
using namespace std;
namespace Stamina {
namespace ListWnd {
/*
	ItemList::iterator ItemList::insert(ItemList::iterator where, oItem& item) {
		this->prepareItem(item);
		return this->list<oItem>::insert(where, item);
	}
	void ItemList::push_front(oItem& item) {
		this->prepareItem(item);
		this->list<oItem>::push_front(item);
	}
	void ItemList::push_back(oItem& item) {
		this->prepareItem(item);
		this->list<oItem>::push_back(item);
	}
	void ItemList::prepareItem(oItem& item) {
		item->setRefreshFlag(refreshSize);
		item->setRefreshFlag(refreshPos);
		item->setRefreshFlag(refreshPaint);
	}
*/
	unsigned int ItemList::count() {
		unsigned int count = 0;
		for (iterator it = this->begin(); it != this->end(); ++it) {
			count ++;
		}
		return count;
	}

} /* ListWnd */

} /* Stamina */

