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


#ifndef __COLLECTIONENTRY__
#define __COLLECTIONENTRY__


/* Include files */
#include "Entry.h"
namespace Stamina
{
namespace ListWnd
{
	class CollectionEntry : public Collection
	{
	public:
		CollectionEntry(const std::string& text, bool expanded);

		void setText(ListView* lv, const std::string& text, bool repaint);
		std::string getText() {
			return _text;
		}

		bool onKeyUp(ListView* lv, const oItem& li, int level, int vkey, int info);
		bool onMouseDblClk(ListView* lv, const oItem& li, int level, int vkey, const Point& pos);


	protected:
		Size getMinSize();
		Size getMaxSize();
		Size getQuickSize();
		Size getEntrySize(ListView* lv, const oItem& li,			const oItemCollection& parent, Size fitIn);
		void paintEntry(ListView* lv, const oItem& li,				const oItemCollection& parent);
		oItem createItem(ListView* lv, const oItemCollection& collection);

	private:
		std::string _text;
		bool _expanded;

	};/* END CLASS DEFINITION CollectionEntry */
} /* ListWnd */

} /* Stamina */


#endif /* __COLLECTIONENTRY__ */
