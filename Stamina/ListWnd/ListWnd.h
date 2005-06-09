/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
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
