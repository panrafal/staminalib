/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once
#ifndef __STAMINA_LISTWND_IITEM__
#define __STAMINA_LISTWND_IITEM__

#include "..\Object.h"

namespace Stamina
{
namespace ListWnd
{
	class iItem: public iSharedObject
	{
	public:
		STAMINA_OBJECT_CLASS(Stamina::ListWnd::iItem, iSharedObject)

	};

	class iItemCollection: public iItem
	{
		STAMINA_OBJECT_CLASS(Stamina::ListWnd::iItemCollection, iItem)
	};

/*	class iItemCollection: public iItem, public iItemCollection_iface
	{
	};*/


	// If we don't know Item
#ifndef __STAMINA_LISTWND_ITEM__
#define __STAMINA_LISTWND_IITEM__OBJECT_BY_IFACE__
/*	class oItem : public SharedPtr<iItem>
	{
	};
	class oItemCollection : public SharedPtr<iItemCollection>
	{
	};*/
	typedef SharedPtr<iItem> oItem;
	typedef SharedPtr<iItemCollection> oItemCollection;

#endif

};
};

#endif