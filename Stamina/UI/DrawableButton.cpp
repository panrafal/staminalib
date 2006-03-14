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
#include "ToolTipX.h"
#include "DrawableButton.h"

namespace Stamina { namespace UI {





	// -------------------------------------------------------------


	DrawableButtonBasic::DrawableButtonBasic(const Rect& rect, const oImage& image, const oImage&  face, Image::DrawState startState): DrawableControlRect(rect), _face(face), _image(image), _state(startState)
	{
		_drawn = false;
	}

	void DrawableButtonBasic::draw(HDC dc, const Point& origin) {
		_drawn = true;
		Rect rc = _rect;
		rc.setPos(rc.getPos() + origin);
		Image::DrawOpts opts = Image::optState(_state);
		if (_face) {
			_face->drawStretched(dc, rc, &opts);
		}
		if (_image) {
			_image->drawCentered(dc, rc, &opts);
		}
	}
	bool DrawableButtonBasic::onMouseDown(int vkey, const Point& pt) {
		bool hit = this->hitTest(pt);
		if (hit)
			this->onButtonPressed(pt, vkey);
		return hit;
	}
	bool DrawableButtonBasic::onMouseMove(int vkey, const Point& pt) {
		bool hit = this->hitTest(pt);
		if (vkey == 0) {
			if (hit)
				DrawableControl::getTipTarget()->attachControl(this);
			else
				DrawableControl::getTipTarget()->detachControl(this);
		}

		return hit;
	}



} };