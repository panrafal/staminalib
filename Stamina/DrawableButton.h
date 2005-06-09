/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once

#include <boost\function.hpp>

#include "Control.h"
#include "Image.h"
#include "iToolTipX.h"



namespace Stamina {

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


};