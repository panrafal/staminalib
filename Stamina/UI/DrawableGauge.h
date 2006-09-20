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



namespace Stamina { namespace UI {

	typedef SharedPtr <class DrawableGaugeBasic> oDrawableGauge;

	class DrawableGaugeBasic: public DrawableControlRect {
	public:

		DrawableGaugeBasic(const Rect& rect, const oImage& border, const oImage& gauge, const oImage& thumb = oImage(), int margin = 0, int value = 0, int thumbPos = 0, int min = 0, int max = 255) : DrawableControlRect(rect), _border(border), _gauge(gauge), _thumb(thumb), _margin(margin), _value(value), _thumbPos(thumbPos), _min(min), _max(max) 
		{
			_drawn = false;
			_showThumb = true;
		}
		
		void draw(HDC dc, const Point& origin);
		
		bool onMouseDown(int vkey, const Point& pt);
		bool onMouseMove(int vkey, const Point& pt);

		void setThumbPos(int pos) {
			if (pos < _min) pos = _min;
			if (pos > _max) pos = _max;
			this->onThumbMove(pos);
		}
		int getThumbPos() {
			return _thumbPos;
		}
		void showThumb(bool show) {
			if (show != _showThumb)
				_drawn = false;
			_showThumb = show;
		}
		void setValue(int newValue) {
			if (newValue < _min) newValue = _min;
			if (newValue > _max) newValue = _max;
			if (_value != newValue) {
				_value = newValue;
				_drawn = false;
			}
		}
		int getValue() {
			return _value;
		}

		virtual bool needsRepaint() {
			return !_drawn;
		}

		virtual bool onThumbMove(int newPos) {
			if (newPos < _min) newPos = _min;
			if (newPos > _max) newPos = _max;
			bool change = _thumbPos != newPos;
			if (change) {
				_thumbPos = newPos;
				_drawn = false;
			}
			return change;
		}

	protected:
		oImage _border;
		oImage _gauge;
		oImage _thumb;
		int _margin, _min, _max, _value, _thumbPos;
		bool _drawn;
		bool _showThumb;

	};


	// -------------------------------------

	class DrawableGaugeEvent: public DrawableGaugeBasic {
	public:
		DrawableGaugeEvent(const Rect& rect, const oImage& border, const oImage& gauge, const oImage& thumb = oImage(), int margin = 0, int value = 0, int thumbPos = 0, int min = 0, int max = 255) : DrawableGaugeBasic(rect, border, gauge, thumb, margin, value, thumbPos, min, max) {
		}
		virtual bool onThumbMove(int newPos) {
			if (_thumbPos != newPos && evtThumbChange) {
				this->_drawn = false;
				if (evtThumbChange(this, newPos) == false)
					return false;
			}
			return DrawableGaugeBasic::onThumbMove(newPos);
		}

		virtual ToolTipX::oTip getTipObject() {
			if (evtGetTipObject) {
				return evtGetTipObject(this);
			} else {
				return Control::getTipObject();
			}
		}

		boost::function<bool (DrawableGaugeEvent*, int newValue)> evtThumbChange;
		boost::function<ToolTipX::oTip (DrawableGaugeEvent*)> evtGetTipObject;
	};


} };