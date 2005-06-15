/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
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
		if (_thumb && _showThumb) {
			int margin = ceil((float)_thumb->getSize().w / 2);
			float part = (float)(_thumbPos - _min) / (_max - _min);
			_thumb->drawCentered(dc, Rect(Point((rc.getWidth()-2*margin)*part + margin + rc.left, rc.top), Size(1, rc.getHeight())));
		}
	}

	bool DrawableGaugeBasic::onMouseDown(int vkey, const Point& pt) {
		bool hit = this->hitTest(pt);
		if (hit && ((vkey & mouseButtons) == mouseLButton) && _thumb && _showThumb) {
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
		} else if ((vkey == (mouseLButton | mouseCaptured)) && hit && _thumb && _showThumb) {
			// ustawiamy thumb
			int margin = _thumb->getSize().w / 2;
			float part = (float)(pt.x - this->_rect.left - margin) / (this->_rect.getWidth() - 2*margin);
			this->setThumbPos((this->_max - this->_min)*part + this->_min);
		}
		return hit;
	}



};