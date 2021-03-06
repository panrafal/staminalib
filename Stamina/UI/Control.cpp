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

The Initial Developer of the Original Code is "STAMINA" - Rafa� Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa� Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

*/

#include "stdafx.h"
#include "ToolTipX.h"
#include "Control.h"

namespace Stamina {  namespace UI {

	void DrawableTipTarget::detachControl(const oDrawableControl& ctrl) {
		ObjLocker lock(this, lockWrite);
		if (_local == ctrl->getRect()) {
			_local = Rect();
			_tip = ToolTipX::oTip();
		}
	}
	void DrawableTipTarget::attachControl(const oDrawableControl& ctrl) {
		ObjLocker lock(this, lockWrite);
		if (ctrl->getRect() != _local)
			_needsRefresh = true;
		else
			return;
		_local = ctrl->getRect();
		_tip = ctrl->getTipObject();
	}
	ToolTipX::oTip DrawableTipTarget::getTip(ToolTipX::ToolTip * ttx) {
		ObjLocker lock(this, lockRead);
		return _tip;
	}
	// zawsze w obr�bie, �eby automat nam go nie wywali�
	bool DrawableTipTarget::mouseAtTarget(ToolTipX::ToolTip * ttx,HWND window, const Point& screen, const Point& local) {
		ObjLocker lock(this, lockWrite);
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



} };
