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

#pragma once

#include <boost\function.hpp>

#include "Control.h"
#include "Image.h"
#include "iToolTipX.h"



namespace Stamina { namespace UI {

	typedef SharedPtr <class DrawableButtonBasic> oDrawableButton;

	class DrawableButtonBasic: public DrawableControlRect {
	public:

		DrawableButtonBasic(const Rect& rect, const oImage& image, const oImage& face = oImage(), Image::DrawState startState = Image::stateNormal);
		

		void draw(HDC dc, const Point& origin /**The origin point of button's coords relative to HDC*/);
		//bool mouseMove(Point pt, int vkey);
		
		bool onMouseDown(int vkey, const Point& pt /**Coords local to the button*/);
		bool onMouseMove(int vkey, const Point& pt /**Coords local to the button*/);
		void setDrawState(Image::DrawState state) {
			_state = state;
		}
		Image::DrawState getDrawState() {
			return _state;
		}

		virtual void onButtonPressed(const Point& pt, int vkey) {}

		virtual bool needsRepaint() {
			return !_drawn;
		}

		void setImage(const oImage& image) {
			_image = image;
			_drawn = false;
		}

	protected:
		bool _drawn;
		oImage _image;
		oImage _face;
		Image::DrawState _state;

	};

	class DrawableButtonEvent: public DrawableButtonBasic {
	public:
		DrawableButtonEvent(const Rect& rect, const oImage& image, const oImage& face = oImage(), Image::DrawState startState = Image::stateNormal)
			:DrawableButtonBasic(rect, image, face, startState) {
		}
		void onButtonPressed(const Point& pt, int vkey) {
			if (evtClick)
				evtClick(this, pt, vkey);
		}

		virtual ToolTipX::oTip getTipObject() {
			if (evtGetTipObject) {
				return evtGetTipObject(this);
			} else {
				return Control::getTipObject();
			}
		}

		boost::function<void (DrawableButtonEvent*, const Point& pt, int vkey)> evtClick;
		boost::function<ToolTipX::oTip (DrawableButtonEvent*)> evtGetTipObject;
	};


} };