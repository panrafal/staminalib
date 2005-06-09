/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#include "stdafx.h"
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x600
#define _WIN32_WINDOWS 0x0490
#include <windows.h>
#include <commctrl.h>
#include <stdstring.h>
#include <math.h>
#include <map>
#include "Helpers.h"
#include "WinHelper.h"

using namespace std;
#include "ToolTipX.h"



namespace Stamina {	namespace ToolTipX {
	//    ------------------------------------------------------------
	ToolTip::ToolTip(HWND parent, HINSTANCE hInst) {
		this->create(parent, hInst);
	}
	ToolTip::ToolTip(){
		this->_hwnd = 0;
		this->_active = false;
	}
	void ToolTip::create(HWND parent, HINSTANCE hInst){
		this->registerClass(hInst);
		this->_timer = CreateWaitableTimer(0 , 0 , 0);
		_hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,"Stamina::ToolTip","",
			WS_POPUP/* | WS_BORDER*/,
			0,0,100,108,
			parent,NULL,hInst,NULL);
		if (!_hwnd) {
			//ShowLastError();
			MessageBox(0, "Nie utworzy³em tooltipa!", "", MB_OK);
		}
		SetProp(_hwnd , "ToolTip*" , (HANDLE)this);
		this->_active = false;
		this->hideTimeout = 500;
		this->enterWait = 500;
		this->enterSiblingTimeout = 500;
		this->moveDelay = 10;
		this->_lastShown = 0;
		this->_lastPositioned = 0;
		this->_region = 0;
		this->_defaultPosition = enPositioning(positionFloatSlow | positionFirst | positionRefresh);
		this->_automated = false;
		this->_attached = false;
		this->_placement = pLeftBottom;
		this->_size.cx = 0;
		this->_size.cy = 0;
		this->_lastPlacementSwitch = 0;
		this->_dontReset = 0;

	}
	ToolTip::~ToolTip() {
		unregisterTooltip(this);
		DestroyWindow(_hwnd);
		CloseHandle(_timer);
		if (this->_region)
			DeleteObject(this->_region);
	}
	ToolTip * ToolTip::fromHWND(HWND hwnd) {
		return (ToolTip*)GetProp(hwnd, "ToolTip*");
	}


	void ToolTip::mouseMovement(HWND window, int scrollX, int scrollY) {
		mouseMovement(window, getScreenPoint(), scrollX, scrollY);
	}

	void ToolTip::mouseMovement(HWND window, const Point& screen, int scrollX, int scrollY) {
		if (!this->_hwnd) return;
		mouseMovement(window, screen, getLocalPoint(window, screen, scrollX, scrollY));
	}
	Point ToolTip::getPlacementPos(const Point& screen, int offset, enPlacement place) {
		Point pt;
		if (place & pLeft)
			pt.x = screen.x - this->_size.cx - offset;
		else
			pt.x = screen.x + offset;
		if (place & pTop)
			pt.y = screen.y - this->_size.cy - offset;
		else
			pt.y = screen.y + offset;
		return pt;
	}

	void ToolTip::setPos(const Point& screen, bool offset, enPositioning position, enPlacement place) {
		if (!this->_currentTarget) return;
		enPositioning allowPos = this->_currentTarget->getPositioning();
		if (allowPos == positionAuto) { 
			allowPos = this->_defaultPosition;
		}
		if ((position & positionFloat) && (allowPos & positionFloatSlow)) {
			if (GetTickCount() - this->_lastPositioned < this->moveDelay) 
				return;
			this->_lastPositioned = GetTickCount();
		}
		if (!(position & allowPos)) return;

		Rect rc;
		SystemParametersInfo(SPI_GETWORKAREA,0,rc.ref(),0);
		rc.expand(-5, -5);
		int offsetV = offset ? (this->_defaultPosition & positionFloat ? cursorOffset : cursorOffset / 2) : 0;
		Point pt = getPlacementPos(screen, offsetV, place == pNone ? this->_placement : place);
		Rect trc (pt, _size);
		//fitInRect(trc, rc);
		if (place == pNone && (!rc.contains(trc) || this->_placement == pNone)) {
			DWORD ticks = GetTickCount();
			/*		if (this->_placement != pNone && abs(ticks - this->_lastPlacementSwitch) < 500) {
			fitInRect(trc, rc);
			} else { */
			// Zmieniamy pozycjonowanie...
			Point pt1 = getPlacementPos(screen, offsetV, pLeftTop);
			Point pt2 = getPlacementPos(screen, offsetV, pRightBottom);
			Rect wrc (pt1.x, pt1.y, pt2.x + _size.cx, pt2.y + _size.cy);
			this->_lastPlacementSwitch = ticks;
			place = pNone;
			place = (enPlacement) (place | ((wrc.right > rc.right) ? pLeft : pRight));
			place = (enPlacement) (place | ((wrc.bottom > rc.bottom) ? pTop : pBottom));
			//		}
		}
		if (place) {
			pt = getPlacementPos(screen, offsetV, place);
			this->_placement = place;
			trc = Rect(pt, _size);
			trc.fitIn(rc);
			this->refreshRegion();
		}
		//CStdString debug;
		//debug.Format("SWP %d - %d %d\n", position, trc.left, trc.top);
		//OutputDebugString(debug);
		SetWindowPos(this->_hwnd, 0, trc.left, trc.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	void ToolTip::mouseMovement(HWND window, const Point& screen, const Point& local) {
		if (!this->_hwnd) return;
		/*if (screen.x != -1 && this->_lastScreen == screen)
			return; // powtórka!*/
		if (this->_automated && screen.x != -1 && sqrt(pow(screen.x - this->_lastScreen.x, 2)+pow(screen.y - this->_lastScreen.y, 2)) > 10 ) {
			this->hide(true);
			//OutputDebugString("Speeding\n");
			this->_lastScreen = screen;
			this->_lastShown = 0;
			return;
		}
		this->_lastScreen = screen;
		if (this->_currentTarget) {
			if (this->_defaultPosition & positionFloat) {
				this->setPos(screen, true, positionFloat);
			}
			// sprawdzamy czy aktualny siê mieœci...
			if ((!this->_currentWindow || this->_currentWindow == window)
				&& this->_currentTarget->mouseAtTarget(this, window,screen, local)) 
			{
				//this->_lastScreen = screen;
				if (!this->visible() && !this->_active) {
					//OutputDebugString(_sprintf("\nShowActive %d < %d", (GetTickCount() - this->_lastShown), this->enterSiblingTimeout));
					this->show((GetTickCount() - this->_lastShown) > this->enterSiblingTimeout);
				}
				return; // skoro siê mieœci - zostawiamy aktualny...
			}
		}
		// szukamy nowego do pokazania...
		tTargets::iterator it = _targets.find(window);
		while (it != _targets.end() && it->first == window) {
			if (it->second->mouseAtTarget(this, window,screen, local)) {
				//OutputDebugStr("CancelHideTimer1\n");
				CancelWaitableTimer(this->_timer);
				//this->_lastScreen = screen;
				bool currentlyShowing = this->_currentTarget && this->visible();
				// wybieramy go i nie szukamy dalej...
				this->setTip(window, it->second, true);
				this->show(!currentlyShowing && ((GetTickCount() - this->_lastShown) > this->enterSiblingTimeout));
				//OutputDebugString("ShowNew\n");
				return;
			}
			it++;
		}
		// nic nie znalaz³ wiêc chowamy...
		//this->_lastScreen = Point(-1,-1);
		if (this->_defaultPosition & positionFloat) {
			//		CancelWaitableTimer(this->_timer);
			this->hide(true);
		}  else if (!this->visible()) {
			this->_active = false;
		}
		// mo¿emy nie kasowaæ poprzedniego - mo¿e jeszcze siê przydaæ...
	}


	VOID CALLBACK  ToolTip::timerShowHideProc(ToolTip * tip , DWORD low , DWORD high) {
		if (!tip || !tip->_currentTarget) {
			//OutputDebugString("Timer-notip\n");
			return;
		}
		Point screen = getScreenPoint();
		HWND window = tip->_currentWindow;
		Point local = getLocalPoint(window, screen);
		if (tip->visible()) { // HIDE proc
			if (((tip->_lastScreen.x != screen.x || tip->_lastScreen.y != screen.y) &&  !windowBelongsTo(WindowFromPoint(screen), tip->_hwnd) &&  !tip->_currentTarget->mouseAtTarget(tip, window,screen, local))) {
				tip->hide(true);
				//OutputDebugString("HideTimer-hide\n");
			} else {
				//OutputDebugString("HideTimer\n");
			}
		} else if (tip->_active) { // SHOW proc
			if (tip->_currentTarget->mouseAtTarget(tip, window,screen, local)) {
				//OutputDebugString("ShowTimer\n");
				tip->show(false);
			} else {
				OutputDebugString("ShowTimer-Hide\n");
				//tip->reset();
				tip->_active = false;
				//OutputDebugStr("CancelHideTimer2\n");
				CancelWaitableTimer(tip->_timer);
			}
		}  else {
			//OutputDebugStr("EmptyTimer\n");
		}
		tip->_lastScreen = screen;
	}


	//void ToolTip::mouseMovementInTarget(HWND window, int x, int y) {
	/*TODO: mouseMovementInTarget*/
	//}
	void ToolTip::hide(bool deactivate){
		if (this->visible()) {
			//OutputDebugStr("CancelHideTimer3\n");
			CancelWaitableTimer(this->_timer);
			ShowWindow(this->_hwnd, SW_HIDE);
			if (deactivate)
				this->_active = false;
			this->_lastShown = GetTickCount();
			if (this->_currentTarget)
				this->_currentTarget->onHide(this);
		}
	}// ukrywa tip'a
	void ToolTip::hideAuto() {
		if (this->_automated) {
			hide(false);
			this->_lastShown = 0;
		}
	}
	void ToolTip::attach() {
		if (!this->_attached && this->_currentTarget && this->_currentTarget->getTip(this)) {
			this->_currentTarget->attach(this);
			this->_attached = true;
			this->refresh();
		}/* else if (this->_attached) {
			this->_attached = false;
			this->refresh();
		}*/
	}
	void ToolTip::show(bool wait){
		this->_active = true;
		LARGE_INTEGER lDueTime;
		if (wait) {
			// Odpala wait'a, który sprawdzi czy wybrana kontrolka spe³nia jeszcze warunek...
			if (!this->_currentTarget) return;
			lDueTime.QuadPart = -10000 * this->_currentTarget->getDelay(this);
			SetWaitableTimer(this->_timer , &lDueTime , 0,(PTIMERAPCROUTINE)timerShowHideProc, this , FALSE);
		} else {
			this->attach();
			setPos(getScreenPoint(), true, positionFirst);
			this->_currentTarget->onShow(this);
			ShowWindow(this->_hwnd, SW_SHOWNOACTIVATE);
			SetWindowPos(this->_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			// Odpala wait'a, który sprawdzi czy wybrana kontrolka spe³nia jeszcze warunek...
			lDueTime.QuadPart = -10000 * this->hideTimeout;
			CancelWaitableTimer(this->_timer);
			if (SetWaitableTimer(this->_timer , &lDueTime , this->hideTimeout,(PTIMERAPCROUTINE)timerShowHideProc, this , FALSE)) {
				//OutputDebugStr("SetHideTimer\n");
			} else {
				int r = GetLastError();
				//OutputDebugStr(_sprintf("SetHideTimer failed! %d - %s\n", r, GetLastErrorMsg(r)));
			}

		}
	}// pokazuje tip'a
	bool ToolTip::visible() {
		return IsWindowVisible(this->_hwnd) != 0;
	}
	ToolTip::tTargets::iterator ToolTip::findTarget(HWND window, const iTarget & target) {
		tTargets::iterator it = _targets.find(window);
		while (it != _targets.end() && it->first == window) {
			if (it->second.get()->operator==(target)) return it;
			it++;
		}
		return _targets.end();
	}

	void ToolTip::addTarget(HWND window, oTarget & target) {
		tTargets::iterator it = this->findTarget(window, target);
		if (it != this->_targets.end()) {
			this->_targets.erase(it);
		}
		this->_targets.insert(pair<HWND, oTarget>(window, target));
	}
	void ToolTip::removeTarget(HWND window, const oTarget & target) {
		tTargets::iterator it = this->findTarget(window, target);
		if (it != this->_targets.end()) {
			this->_targets.erase(it);
		}
	}
	void ToolTip::clearTargets(HWND window) {
		/*tTargets::iterator start = _targets.find(window);
		if (start == _targets.end) return;
		tTargets::iterator end = start;
		while (end != _targets.end() && end->first == window) {
		end++;
		}*/
		_targets.erase(window);
	}

	void ToolTip::refresh() {
		if (!this->_currentTarget || !this->_currentTarget->getTip(this) || !this->_attached) {
			bool brake = this->_currentTarget && this->_currentTarget->getTip(this) && !this->_attached;
			return;
		}

		bool setBackVisible = false;
		/*	if (!this->_active)
		return;*/
		this->_dontReset ++;
		if (this->visible()) {
			setBackVisible = true;
			ShowWindow(this->_hwnd, SW_HIDE);
		}

		HDC hdc = GetDC(this->_hwnd);
		Rect rc (0,0,250,18);
		this->_currentTarget->getTip(this)->adjustSize(hdc, rc);
		this->adjustTipRect(rc);
		ReleaseDC(this->_hwnd, hdc);
		this->_size.cx = rc.right;
		this->_size.cy = rc.bottom;
		//if (!this->_attached) return;

		SetWindowPos(this->_hwnd, 0, 0, 0, rc.right, rc.bottom, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		if (this->_automated)
			this->_placement = pNone; // ¿eby zrobi³ refresha regionu i pozycji...
		setPos(getScreenPoint(), true, positionRefresh);
		this->refreshRegion();
		//RedrawWindow(this->_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
		if (setBackVisible)
			ShowWindow(this->_hwnd, SW_SHOWNOACTIVATE);
		this->_dontReset --;
	}
	void ToolTip::refreshRegion() {
		this->_region = Region::createRoundRectRgn(Rect(Point(),this->_size), roundness, roundness);
		this->_region.or(Rect(
            (this->_placement & pLeft) ? this->_size.cx - roundness : 0
			,(this->_placement & pLeft) ? this->_size.cx - 1 : roundness
			,(this->_placement & pTop) ? this->_size.cy - roundness : 0
            ,(this->_placement & pTop) ? this->_size.cy - 1 : roundness));
		SetWindowRgn(this->_hwnd, this->_region.getCopy(), true);
		repaintWindow(this->_hwnd);
	}

	void ToolTip::setTip(HWND hwnd, oTarget & target, bool treatAsAuto) {
		if (!target->getTip(this)) return;
		this->reset(); // od³¹czamy aktualny...
		//SetParent(this->_hwnd, hwnd);
		this->_active = true;
		this->_currentWindow = hwnd;
		this->_currentTarget = target;
		this->_automated = treatAsAuto;


		//	this->unlockPos();
		//	this->refresh();
	}
	void ToolTip::setTip(iTip * tip, bool treatAsAuto) {
		if (!tip) {
			if (!this->_currentTarget) return;
			this->_dontReset ++;
			//OutputDebugString("Reset\n");
			this->hide();
			if (this->_attached && this->_currentTarget && this->_currentTarget->getTip(this)) {
				this->_currentTarget->detach(this);
			}
			this->_attached = false;
			this->_active = false;
			this->_currentTarget = oTarget();
			this->_currentWindow = 0;
			this->_dontReset --;
			return;
		}
		setTip(0, oTarget(new Target(oTip(tip))), treatAsAuto);
	}
	void ToolTip::reset() {
		if (this->_dontReset) return;
		this->_lastShown = 0;
		this->setTip(0, false); 
	}


	void ToolTip::adjustClientRect(Rect & rc) {
		rc.left += marginLeft + margin;
		rc.top += margin;
		rc.bottom -= 2*margin;
		rc.right -= 2*margin;
	}
	void ToolTip::adjustTipRect(Rect& rc) {
		rc.right += marginLeft + margin;
		rc.bottom += margin;
		rc.bottom += 2*margin;
		rc.right += 2*margin;
	}
	HWND ToolTip::getTipWindow() {
		return this->_hwnd;
	}


	void ToolTip::registerClass(HINSTANCE hInst) {
		static once = true;
		if (!once) return;
		once = true;
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInst;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= GetSysColorBrush(COLOR_WINDOW);
		wcex.lpszMenuName	= "";
		wcex.lpfnWndProc        = (WNDPROC)windowProc;
		wcex.lpszClassName      = "Stamina::ToolTip";
#define CS_DROPSHADOW 0x00020000
		wcex.style              = isComctl(6 , 0) * CS_DROPSHADOW;
		wcex.hIcon              = 0;
		wcex.hIconSm            = 0;
		RegisterClassEx(&wcex);

	}
	int CALLBACK ToolTip::windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		switch (message) {
		case WM_PAINT:{
			Rect rc;
			GetClientRect(hwnd, rc.ref());
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			//FillRect(hdc, &rc, GetSysColorBrush(COLOR_INFOBK));
			SetTextColor(hdc, GetSysColor(COLOR_INFOTEXT));
			ToolTip * tip = ToolTip::fromHWND(hwnd);
			if (tip && tip->_currentTarget && tip->_currentTarget->getTip(tip)) {
				tip->adjustClientRect(rc);
				tip->_currentTarget->getTip(tip)->paint(hdc, rc);	
			}
			if (tip->_region) {
				HBRUSH brush = GetSysColorBrush(COLOR_INFOTEXT);
				tip->_region.dcFrame(hdc, brush, 1, 1);
			}
			EndPaint(hwnd, &ps);
			return 1;}
		case WM_ERASEBKGND:{
			HDC hdc = (HDC)wParam;
			RECT rc;
			ToolTip * tip = ToolTip::fromHWND(hwnd);
			GetClientRect(hwnd, &rc);
			FillRect(hdc, &rc, GetSysColorBrush(COLOR_INFOBK));

			int ca = GetSysColor(COLOR_INFOBK);
			int cb = GetSysColor(COLOR_INFOTEXT);
			//HBRUSH brush = CreateSolidBrush(RGB_ALPHA(ca, cb, 50));
			HBRUSH brush = CreateSolidBrush(blendRGB(ca, cb, 220));
			SetBkMode(hdc, TRANSPARENT);
			Region rgn1 (tip->_region);
			rgn1 += Point(marginLeft, 0);
			rgn1.diff(tip->_region);
			rgn1.dcFill(hdc, brush);
			DeleteObject(brush);
			return 1;
						   }
		case WM_MOUSEMOVE:
			//fromHWND(hwnd)->hide();
			break;
		case WM_LBUTTONUP:
			fromHWND(hwnd)->reset();
			break;
		case WM_ACTIVATE: {
			//OutputDebugString(_sprintf("\nWM_ACT w=%d %x != %x", LOWORD(wParam), lParam, hwnd));
			if (LOWORD(wParam) == WA_INACTIVE) {
				//fromHWND(hwnd)->reset();
				HWND wnd = (HWND)GetProp(hwnd, "reactivate");
				if (!wnd) return 0;
				if (!lParam || !windowBelongsTo((HWND)lParam, wnd)) {
					fromHWND(hwnd)->reset();
				}
				return 0;
				fromHWND(hwnd)->reset();
				SetProp(hwnd, "reactivate", 0);
				char debug [51];
				GetWindowText(wnd, debug, 50);
				//OutputDebugString(_sprintf("Activating %s\n", debug));
				//SetProp(hwnd, "reactivate", 0);
				fromHWND(hwnd)->_dontReset ++;
				SetActiveWindow((HWND) wnd);
				SetForegroundWindow((HWND) wnd);
				fromHWND(hwnd)->_dontReset --;
			} else { // zapamietujemy okno deaktywowane...
				char debug [51];
				GetWindowText((HWND)lParam, debug, 50);
				//OutputDebugString(_sprintf("Storing %s\n", debug));
				SetProp(hwnd, "reactivate", (HANDLE)lParam);
			}
			return 0;}
		}

		ToolTip * tip = ToolTip::fromHWND(hwnd);
		try {
			if (tip && tip->_currentTarget && tip->_currentTarget->getTip(tip)) {
				bool returned = false;
				int r = tip->_currentTarget->getTip(tip)->parentMessageProc(hwnd, message, wParam, lParam, returned);
				if (returned) return r;
			}	
		} catch (...) {}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}


	TooltipsReg _ttReg;

	void registerTooltip(HWND hwnd, ToolTip* tt) {
		
		_ttReg.insert(TooltipsReg::value_type(hwnd, tt));
	}
	void unregisterTooltip(ToolTip* tt) {
		if (_ttReg.empty())
			return;
		for (TooltipsReg::iterator it=_ttReg.begin(); it != _ttReg.end(); ++it) {
			if (it->second == tt)
				it = _ttReg.erase(it);
		}
	}
	void unregisterTooltip(HWND hwnd) {
		for (TooltipsReg::iterator it=_ttReg.find(hwnd); it != _ttReg.end(); ++it) {
			if (it->first == hwnd)
				it = _ttReg.erase(it);
		}
	}
	void unregisterTooltip(HWND hwnd, ToolTip* tt) {
		for (TooltipsReg::iterator it=_ttReg.find(hwnd); it != _ttReg.end(); ++it) {
			if (it->first == hwnd && it->second == tt)
				it = _ttReg.erase(it);
		}
	}
	
	const TooltipsReg& getRegisteredMap() {
		return _ttReg;
	}

	void mouseMovement(HWND hwnd, Point scroll) {
		Point screen = getScreenPoint();
		Point local = getLocalPoint(hwnd, screen, scroll.x, scroll.y);
		for (TooltipsReg::iterator it=_ttReg.begin(); it != _ttReg.end(); ++it) {
			if (it->first == hwnd || it->first == 0)
				it->second->mouseMovement(hwnd, screen, local);
		}
	}


} } // namespace