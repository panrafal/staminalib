/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#ifndef __STAMINA_ITOOLTIPX__
#define __STAMINA_ITOOLTIPX__

#include <boost/intrusive_ptr.hpp>
#include <deque>
#include "RichHtml.h"
#include "Rect.h"
#include "Region.h"
#include "Image.h"
#include "Object.h"

namespace Stamina { namespace ToolTipX {

	class ToolTip;

	enum enPositioning {
		positionAuto=0,
		/// Tooltip "przyklejony" do myszki
		positionFloat = 1,
		positionFloatSlow = 2 | positionFloat,
		/// Zablokowany na pozycji "otwarcia"
		positionFirst = 0x10000,
		positionRefresh = 0x20000 | positionFirst,
		/// Ustalona pozycja
		positionFixed = 0x40000,
	};

	enum enPlacement {
		pNone = 0,
		pRight = 1,
		pLeft = 2,
		pBottom = 4,
		pTop = 8,
		pRightBottom = 5,
		pLeftBottom = 6,
		pRightTop = 9,
		pLeftTop = 10,
	};

	/** Tooltip's graphical representation.
	iTip objects are used to determine the size of tooltip's window, and to draw it's contents.

	There are several stock iTip objects to choose from.
	*/
	class iTip: public iSharedObject {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::iTip, iSharedObject);

		virtual ~iTip(){};
		/** The tip should adjust size of it's bounding box.
		Tip should try to respect at least width given in @a rc.
		*/
		virtual void adjustSize(HDC hdc, Rect& rc)=0;
		/** The tip should be painted on canvas (@a hdc). 
		@param rc tip's size set in adjustSize.
		*/
		virtual void paint(HDC hdc, const Rect& rc)=0;
		/** This tip is being attached to the Tooltip window */
		virtual void attach(HWND hwnd) {}
		/** This tip is being detached from the Tooltip window */
		virtual void detach(HWND hwnd) {}
		virtual int parentMessageProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam, bool& returned) {returned = false; return 0;}
		static const margin = 2;
	};

	typedef boost::intrusive_ptr<iTip> oTip;

	/** Specifies tooltip target which will be queried on mouse movement.
	iTarget objects are used to bind screen areas/controls/etc. with iTip objects (which can be set statically or given on demand).
	iTarget controls also some general tip behaviour like window placement and popup delay.

	There are several stock iTarget objects to choose from.
	*/
	class iTarget: public iSharedObject {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::iTarget, iSharedObject);

		virtual ~iTarget(){}
		/**Return true if mouse is over this target.
		If true, this target will become active (attached).
		*/
		virtual bool mouseAtTarget(ToolTip * ttx,HWND window, const Point& screen, const Point& local)=0;
		virtual bool operator==(const iTarget & b) const;
		/**Should return an iTip object*/
		virtual oTip getTip(ToolTip * ttx)=0;
		/**Tooltip window is being hidden*/
		virtual void onHide(ToolTip * ttx) {};
		/**Tooltip window is being showed up*/
		virtual void onShow(ToolTip * ttx) {};
		/**Returns the target's preferred positioning*/
		virtual enPositioning getPositioning() {return positionAuto;}
		/**Returns delay between mouseAtTarget() == true and onShow()*/
		virtual int getDelay(ToolTip * ttx);
		/**This target is being activated*/
		virtual void attach(ToolTip * ttx);
		/**This target is being deactivated*/
		virtual void detach(ToolTip * ttx);

	};

	typedef boost::intrusive_ptr<iTarget> oTarget;


} };
#endif
