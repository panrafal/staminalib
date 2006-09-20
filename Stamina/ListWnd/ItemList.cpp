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

