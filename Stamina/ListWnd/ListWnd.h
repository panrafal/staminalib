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


#pragma once




namespace Stamina
{
namespace ListWnd
{
	class ListView;

	enum ItemFlags
	{
		flagNone = 0,
		flagRepaint = 1, 
		flagResize = 2, 
		flagRepos = 4, 
		flagDimensionsChanged = 8,
		flagChanged = 0x10,
		flagSubitemsChanged = 0x20, /// Means that the "WholeSize" of a collection should be refreshed
		flagRepaintWhole = 0x40 | flagRepaint, 
		//flagRecalc = 8,
		flagHidden = 0x100, 
		flagExpanded = 0x200, 
		flagChecked = 0x400,
		flagSelected = 0x800,
		flagHiddenInCollection = 0x1000, /// Means that one of the parents is contracted and hides the item
		flagExpandable = 0x2000, /// Means that collection can be expanded
		flagForExpand = 0x4000, /// Means that visibility of an item is affected by expand of it's parent
		flagActive = 0x8000,
	};

	enum RefreshFlags
	{
		refreshAuto = 0, 
		refreshPaint = 1, 
		refreshSize = 2, 
		refreshPos = 4, 
//		refreshWholeSize = 8,
//		refreshSubitems = 16,
		refreshDimensionsChanged = 8,
		refreshPaintWhole = 0x40 | refreshPaint, 

		refreshInserted = refreshPos | refreshSize,
		refreshAll = refreshPos | refreshSize | refreshPaintWhole | refreshDimensionsChanged,
	};

	enum ItemState
	{
		stateNone = 0,
		stateExcluded = 1, /// Item is excluded from view (it's size and position are not set).
		stateNormal = 2, 
	};

	enum InView
	{
		inviewNone=0,
		inviewPartial=1,
		inviewWidth=2, /// Width is in view (height is partial)
		inviewHeight=4,
		inviewWhole=inviewWidth | inviewHeight | inviewPartial,
		inviewWidthOversized=8,
		inviewHeightOversized=0x10,
		inviewOversized=inviewWidthOversized | inviewHeightOversized | inviewPartial,
	};
	inline bool inViewWidthPartial(InView in) {
		return (in & inviewPartial) && !(in & inviewWidth) && !(in & inviewWidthOversized);
	}
	inline bool inViewHeightPartial(InView in) {
		return (in & inviewPartial) && !(in & inviewHeight) && !(in & inviewHeightOversized);
	}
} /* ListWnd */

} /* Stamina */
