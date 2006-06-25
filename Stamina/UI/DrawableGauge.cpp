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
#include <math.h>
#include "DrawableGauge.h"

namespace Stamina {

	void DrawableGaugeBasic::draw(HDC dc, const Point& origin) {
		this->_drawn = true;
		Rect rc = _rect;
		rc.setPos(rc.getPos() + origin);
		if (_border) {
			_border->drawStretched(dc, rc);
		}
		if (_gauge) {
			float part = (float)(_value - _min) / (_max - _min);
			Image::DrawOpts opts = Image::optSource(Rect(Point(), _gauge->getSize()));
			int rightMargin = (part == 1) ? _margin : 0;
			opts.source.right = max(0, (opts.source.getWidth()-2*_margin)*part) + _margin + rightMargin;
			_gauge->drawStretched(dc, Rect(rc.getPos(), Size((rc.getWidth()-2*_margin)*part + _margin + rightMargin, rc.getHeight())), &opts);
		}
		if (_thumb == true && _showThumb == true) {
			int margin = ceil((float)_thumb->getSize().w / 2);
			float part = (float)(_thumbPos - _min) / (_max - _min);
			_thumb->drawCentered(dc, Rect(Point((rc.getWidth()-2*margin)*part + margin + rc.left, rc.top), Size(1, rc.getHeight())));
		}
	}

	bool DrawableGaugeBasic::onMouseDown(int vkey, const Point& pt) {
		bool hit = this->hitTest(pt);
		if (hit && ((vkey & mouseButtons) == mouseLButton) && _thumb.isValid() && _showThumb) {
			this->onMouseMove(vkey | mouseCaptured, pt);
		}
		return hit;
	}


	bool DrawableGaugeBasic::onMouseMove(int vkey, const Point& pt) {
		bool hit = this->hitTest(pt);
		if (vkey == 0) {
			if (hit) {
				DrawableControl::getTipTarget()->attachControl(this);
			} else {
				DrawableControl::getTipTarget()->detachControl(this);
			}
		} else if ((vkey == (mouseLButton | mouseCaptured)) && hit && _thumb.isValid() && _showThumb) {
			// ustawiamy thumb
			int margin = _thumb->getSize().w / 2;
			float part = (float)(pt.x - this->_rect.left - margin) / (this->_rect.getWidth() - 2*margin);
			this->setThumbPos((this->_max - this->_min)*part + this->_min);
		}
		return hit;
	}



};