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
