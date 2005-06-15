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
