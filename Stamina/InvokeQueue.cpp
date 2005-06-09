/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
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