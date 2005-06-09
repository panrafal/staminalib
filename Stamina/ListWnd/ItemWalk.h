/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

/* Model statyczny */


#pragma once

/* Include files */
#include "Item.h"
#include "Entry.h"
#include <stack>

namespace Stamina {
namespace ListWnd {

	class ItemWalk {
	public:
		typedef std::stack<ItemCollection*> Stack;
	/** Walks through items. */
		template <class F> 
		static bool walk(
			ListView* lv, /// ListView to pass to the function object
			const ItemList& list, /// The list to start walking
			F& func, /** Function object with @b void operator()(const @b oItem&, const @b ItemWalk&) method. 
					*/
			const oItem& begin, /** Item to start with (or empty to start from beginning) */
			const oItem& end, /** Item to stop on (or empty) */
			bool recursive=true,
			bool onlyVisible=true) 
		{
			S_ASSERT(lv);
			ItemWalk iw(lv, begin, end, recursive, onlyVisible);   
			lv->lockPaint();
			lv->lockRefresh();
			bool res = iw._walk<F>(list, func);
			lv->unlockRefresh();
			lv->unlockPaint();
			return res;
		}
		template <class F> 
		static bool walk(
			ListView* lv, F& func, const oItem& begin = oItem(), const oItem& end = oItem(), bool recursive=true, bool onlyVisible=true) 
		{
			S_ASSERT(lv);
			return walk(lv, oItemCollection(lv->getRootItem().get()), func, begin, end, recursive, onlyVisible);
		}
		template <class F> 
		static bool walk(
			ListView* lv, oItemCollection coll, F& func, const oItem& begin, const oItem& end, bool recursive=true, bool onlyVisible=true) 
		{
			S_ASSERT(lv);
			ItemWalk iw(lv, begin, end, recursive, onlyVisible);   
			lv->lockPaint();
			lv->lockRefresh();
			iw._stack.push(coll.get());
			bool res = iw._walk<F>(coll->getItemList(), func);
			iw._stack.pop();
			lv->unlockRefresh();
			lv->unlockPaint();
			return res;
		}

		inline ListView* getListView() {
			return _lv;
		}
		inline ItemCollection* getParent() {
			if (_stack.empty()) return 0;
			return _stack.top();
		}
		inline oItem getBegin() {
			return _begin;
		}
		inline oItem getEnd() {
			return _end;
		}
		inline int getLevel() {
			return _stack.size();
		}
		inline int getCount() {
			return _count;
		}
		inline void stop() {
			this->_running = false;
		}

	protected:

		ItemWalk(
			ListView* lv, /// ListView to pass to function object
			const oItem& begin, /** Item to start with (or empty to start from beginning) */
			const oItem& end, /** Item to stop on (or empty) */
			bool recursive=true, bool onlyVisible=true 
			):_lv(lv), _begin(begin), _end(end), _recursive(recursive), _count(0),_onlyVisible(onlyVisible)
		{
			_running = begin ? false : true;				
		}

		bool isRunning(const oItem& item) {
			if (!_running) {
				if (_begin == item) {
					_running = (_end != _begin);
				} else if (_end == item) {
					_running = (_end != _begin);
					_end = _begin;
				} else {
	                return false;
				}
				_begin.reset();
				return true;
			} else {
				if (_end == item) {
					_running = false;
				} // ostatni powinien siê wykonaæ...
				return true;
			}
		}
		inline bool isBroken() {
			return !_running && !_begin;
		}


		template <class F> 
			bool _walk(const ItemList& list, F& func) 
		{
			for (ItemList::const_iterator it = list.begin(); it != list.end(); ++it) {
				if (_onlyVisible && !(*it)->isVisible())
					continue;
				if (isRunning(*it)) {
					func(*it, *this);
					_count++;
					if (isBroken())
						return false;
				}
				if (_recursive) {
					ItemCollection* ic = (*it)->getItemCollection();
					if (ic) {
						ic->lock();
						_stack.push(ic);
						if (!_walk(ic->getItemList(), func))
							return false;
						_stack.pop();
						ic->unlock();
					}
				}
				
			}
			return true;
		}


		ListView* _lv;
		bool _recursive;
		bool _onlyVisible;
		bool _running;
		oItem _begin;
		oItem _end;
		int _count;
		Stack _stack;
			
	};

	template <class IF>
	class WalkFindFirst {
	public:
		WalkFindFirst(IF& func):_func(func) {
		};
		void operator()(const oItem& item, ItemWalk& walk) {
			ListView* lv = walk.getListView();
			if (_func(*item.get(), lv)) {
				_found = item;
				walk.stop();
			}
		}
		oItem result() {
			return _found;
		}
	private:
		IF& _func;
		oItem _found;
	};
	template <class IF>
	class WalkFindLast {
	public:
		WalkFindLast(IF& func):_func(func), _found(0) {
		};
		void operator()(const oItem& item, ItemWalk& walk) {
			ListView* lv = walk.getListView();
			if (_func(*item.get(), lv)) {
				_found = item.get();
			}
		}
		oItem result() {
			return _found;
		}
	private:
		IF& _func;
		Item* _found; // dla szybkoœci...
	};
	class WalkFind_nullOp {
	public:
		bool operator()(Item& a, ListView* b) {
			return true;
		}
	};
	template <class IF>
	oItem walkFind(ListView*lv, IF& f, bool first=true, const oItem& begin = oItem(),  const oItem& end = oItem(), bool onlyVisible = true) {
		if (first) {
			WalkFindFirst<IF> wff(f);
			ItemWalk::walk(lv, wff, oItem(0), oItem(0), true, onlyVisible);
			return wff.result();
		} else {
			WalkFindLast<IF> wfl(f);
			ItemWalk::walk(lv, wfl, oItem(0), oItem(0), true, onlyVisible);
			return wfl.result();
		}
	}


};
};
