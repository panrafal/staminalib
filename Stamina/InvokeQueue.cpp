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

#include "stdafx.h"
#include "InvokeQueue.h"


namespace Stamina {
/*
	void InvokeQueue::runQueue() {
		QueueItem* list;
		unsigned int count = 0, i = 0;
		{
			LockerCS lock(_cs);
			_isQueued.reset();
			if (_queue.empty()) return;
			count = _queue.size();
			list = new QueueItem [count];
			while (!_queue.empty()) {
				list[i++] = _queue.front();
				_queue.pop();
			}
		}
		for (i = 0; i < count; i++) {
			QueueItem& item = list[i];
			item.func((ULONG_PTR)item.param);
		}
		delete [] list;
	}
*/

	void InvokeQueue::runQueue() {
		_isQueued.reset();
		while (1) {
			QueueItem item;
			{
				LockerCS lock(_cs);
				if (_queue.empty()) break;
				item = _queue.front();
				_queue.pop();
			}
			item.func((ULONG_PTR)item.param);
		}
	}


};