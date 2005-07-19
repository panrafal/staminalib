/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once
#include <boost\intrusive_ptr.hpp>
#include <list>

#include "Rect.h"
#include "Region.h"
#include "ToolTipX.h"


namespace Stamina {

	class Control: public SharedObject<iSharedObject> {
	public:

		enum MouseStates {
			mouseLButton = MK_LBUTTON,
			mouseRButton = MK_RBUTTON,
			mouseShift = MK_SHIFT,
			mouseControl = MK_CONTROL,
			mouseMButton = MK_MBUTTON,
//			mouseXButton1 = MK_XBUTTON1,
//			mouseXButton2 = MK_XBUTTON2,

			mouseWinFlags = 0xFF,
			mouseButtons = mouseWinFlags,

			mouseCaptured = 0x100,
		};

		STAMINA_OBJECT_CLASS_VERSION(Stamina::Control, iSharedObject, Version(1,0,0,0));

	public:

		/**  */
		virtual bool isVirtualWindow() = 0;

		virtual void setRect(const Rect& rc) = 0;
		virtual void setPos(const Point& pt) {
			this->setRect(Rect(pt, this->getSize()));
		}
		virtual void setSize(const Size& sz) {
			this->setRect(Rect(this->getPos(), sz));
		}

		virtual Rect getRect() = 0;
		virtual Point getPos() {
			return this->getRect().getPos();
		}
		virtual Size getSize() {
			return this->getRect().getSize();
		}

		virtual ToolTipX::oTip getTipObject();

		virtual bool hitTest(const Point& pt) {
			return this->getRect().contains(pt);
		}


	};

	typedef SharedPtr<class DrawableTipTarget> oDrawableTipTarget;

	class DrawableControl: public Control {
	public:
		virtual bool isVirtualWindow() {
			return true;
		}

		virtual void draw(HDC dc, const Point& origin /**The origin point of button's coords relative to HDC*/) = 0;
		virtual bool onMouseDown(int vkey, const Point& pt /**Coords local to the button*/) {return false;}
		virtual bool onMouseMove(int vkey, const Point& pt /**Coords local to the button*/) {return false;}
		virtual bool onMouseUp(int vkey, const Point& pt /**Coords local to the button*/) {return false;}

		static oDrawableTipTarget getTipTarget();

		virtual bool needsRepaint() {
			return false;
		}

		virtual void getRepaintRegion(Region& rgn, bool whole=false) {
			if (whole || this->needsRepaint())
				rgn.or(this->getRect());
		}

	};

	class DrawableControlRect: public DrawableControl {
	public:
		DrawableControlRect(const Rect& rc) {
			_rect = rc;
		}
		void setRect(const Rect& rc) {
			_rect = rc;
		}
		void setPos(const Point& pt) {
			_rect.setPos(pt);
		}
		Rect getRect() {
			return _rect;
		}
	protected:
		Rect _rect;
	};

	typedef boost::intrusive_ptr<Control> oControl;
	typedef boost::intrusive_ptr<DrawableControl> oDrawableControl;


	class DrawableImage: public DrawableControl {
	public:
		DrawableImage(const oImage& image) : _image(image) {
		}

		void setRect(const Rect& rc)  {
			int notImplemented=false;
			S_ASSERT(notImplemented);
		}

		Rect getRect() {
			return Rect(Point(), _image->getSize());
		}
		virtual void draw(HDC dc, const Point& origin) {
			_image->draw(dc, origin);
		}
	protected:

		oImage _image;
	};


	/** Groups several controls */
	class DrawableCollection: public DrawableControl {
	public:
		typedef std::list<oDrawableControl> tItems;

	public:

		DrawableCollection():_capture() {}

		void add(const oDrawableControl& ctrl) {
			_items.push_back(ctrl);
		}
		void remove(const oDrawableControl& ctrl) {
			tItems::iterator found = std::find(_items.begin(), _items.end(), ctrl);
			if (found != _items.end())
				_items.erase(found);
		}
		const tItems& getItems() {
			return _items;
		}

		virtual void setRect(const Rect& rc);
		virtual Rect getRect();
		virtual void getRepaintRegion(Region& rgn, bool whole=false);

	protected:

		virtual void draw(HDC dc, const Point& origin);
		virtual bool onMouseDown(int vkey, const Point& pos);
		virtual bool onMouseMove(int vkey, const Point& pos);
		virtual bool onMouseUp(int vkey, const Point& pos);

		tItems _items;
		DrawableControl* _capture;

	};


// ----------------------------------------------

	class DrawableTipTarget: public ToolTipX::TargetImpl {
	public:
		DrawableTipTarget():_needsRefresh(false) {
		}
		void detachControl(const oDrawableControl& ctrl);
		void attachControl(const oDrawableControl& ctrl);
		ToolTipX::oTip getTip(ToolTipX::ToolTip * ttx);
		bool mouseAtTarget(ToolTipX::ToolTip * ttx,HWND window, const Point& screen, const Point& local);
	protected:
		Rect _local;
		ToolTipX::oTip _tip;
		bool _needsRefresh;
	};


	STAMINA_REGISTER_CLASS_VERSION(Control);

};