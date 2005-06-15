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


#ifndef __ENTRYSMALL__
#define __ENTRYSMALL__


/* Include files */
#include <string>
#include "Entry.h"
namespace Stamina
{
namespace ListWnd
{
	class EntrySmall : public EntryImpl
	{
	public:
		EntrySmall(const std::string& text);

		void setText(ListView* lv, const std::string& text, bool repaint);
		std::string getText() {
			return _text;
		}

	protected:
		Size getMinSize();
		Size getMaxSize();
		Size getQuickSize();
		Size getEntrySize(ListView* lv, const oItem& li,			const oItemCollection& parent, Size fitIn);
		void paintEntry(ListView* lv, const oItem& li,				const oItemCollection& parent);

	private:
		std::string _text;
	};/* END CLASS DEFINITION EntrySmall */
} /* ListWnd */

} /* Stamina */


#endif /* __ENTRYSMALL__ */
