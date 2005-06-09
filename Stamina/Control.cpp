/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#include "stdafx.h"
#include "ToolTipX.h"
#include "Control.h"

namespace Stamina {

	void DrawableTipTarget::detachControl(const oDrawableControl& ctrl) {
		ObjLocker lock(this);
		if (_local == ctrl->getRect()) {
			_local = Rect();
			_tip = ToolTipX::oTip();
		}
	}
	void DrawableTipTarget::attachControl(const oDrawableControl& ctrl) {
		ObjLocker lock(this);
		if (ctrl->getRect() != _local)
			_needsRefresh = true;
		else
			return;
		_local = ctrl->getRect();
		_tip = ctrl->getTipObject();
	}
	ToolTipX::oTip DrawableTipTarget::getTip(ToolTipX::ToolTip * ttx) {
		ObjLocker lock(this);
		return _tip;
	}
	// zawsze w obrêbie, ¿eby automat nam go nie wywali³
	bool DrawableTipTarget::mouseAtTarget(ToolTipX::ToolTip * ttx,HWND window, const Point& screen, const Point& local) {
		ObjLocker lock(this);
		if (_local && _tip && _local.contains(local)) {
			if (_needsRefresh) {
				_needsRefresh = false;
				ttx->refresh();
			}
			return true;
		} else
			return false;
    }

	const oDrawableTipTarget _tipTarget = new DrawableTipTarget(); 

	oDrawableTipTarget DrawableControl::getTipTarget() {
		return _tipTarget;
	}

	ToolTipX::oTip Control::getTipObject() {
		return ToolTipX::oTip();
	}


// -----------------------------------------------------------------------
	void DrawableCollection::setRect(const Rect& rc) {
		Rect current = this->getRect();
		if (current.getPos() == rc.getPos())
			return;
		Point diff = rc.getPos() - current.getPos();
		for (tItems::iterator it = _items.begin(); it != _items.end(); ++it) {
			(*it)->setPos( (*it)->getPos() + diff );
		}
	}
	Rect DrawableCollection::getRect() {
		if (_items.empty())
			return Rect();
		Rect rc = _items.front()->getRect();
		for (tItems::iterator it = ++_items.begin(); it != _items.end(); ++it) {
			rc.include((*it)->getRect());
		}
        return rc;
	}
	void DrawableCollection::getRepaintRegion(Region& rgn, bool whole) {
		for (tItems::iterator it = _items.begin(); it != _items.end(); ++it) {
			(*it)->getRepaintRegion(rgn, whole);
		}
	}


	void DrawableCollection::draw(HDC dc, const Point& origin) {
		for (tItems::iterator it = _items.begin(); it != _items.end(); ++it) {
			(*it)->draw(dc, origin);
		}
	}
	bool DrawableCollection::onMouseDown(int vkey, const Point& pos) {
		for (tItems::iterator it = _items.begin(); it != _items.end(); ++it) {
			if ((*it)->onMouseDown(vkey | mouseCaptured, pos)) {
				_capture = it->get();
				return true;
			}
		}
		return false;
	}
	bool DrawableCollection::onMouseMove(int vkey, const Point& pos) {
		for (tItems::iterator it = _items.begin(); it != _items.end(); ++it) {
			if (vkey & mouseCaptured && _capture && it->get() != _capture) continue;
			if ((*it)->onMouseMove(vkey, pos) || vkey & mouseCaptured) {
				return true;
			}
		}
		_capture = 0;
		return false;
	}
	bool DrawableCollection::onMouseUp(int vkey, const Point& pos) {
		for (tItems::iterator it = _items.begin(); it != _items.end(); ++it) {
			if (vkey & mouseCaptured && _capture && it->get() != _capture) continue;
			if ((*it)->onMouseMove(vkey, pos) || vkey & mouseCaptured) {
				_capture = 0;
				return true;
			}
		}
		_capture = 0;
		return false;
	}



};
