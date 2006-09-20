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