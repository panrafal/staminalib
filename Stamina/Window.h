#pragma once

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */



#include "Control.h"

namespace Stamina {

	class Window: public Control {
	public:

		Window(const char* text, int style, int styleEx, const Rect& rect , HWND parent , int id);
		void create(const char* text, int style, int styleEx, const Rect& rect , HWND parent , int id);
		Window(HWND hwnd = 0);
		void init();

		static void registerWindowClass();

		static Window* fromHWND(HWND wnd);

		virtual bool isVirtualWindow() {
			return false;
		}

		inline void mouseCapture() {
			SetCapture(this->_hwnd);
		}
		inline void mouseRelease() {
			ReleaseCapture();
		}
		inline bool isMouseCaptured() {
			return GetCapture() == this->_hwnd;
		}

		MouseStates getMouseState(int vkey) {
			if (isMouseCaptured()) vkey |= mouseCaptured;
			return (MouseStates) vkey;
		}

		virtual void setRect(const Rect& rc) {
			SetWindowPos(_hwnd, 0, rc.left, rc.top, rc.getWidth(), rc.getHeight(), SWP_NOZORDER);
		}
		virtual void setPos(const Point& pt) {
			SetWindowPos(_hwnd, 0, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		virtual void setSize(const Size& sz) {
			SetWindowPos(_hwnd, 0, 0, 0, sz.w, sz.h, SWP_NOZORDER | SWP_NOMOVE);
		}
		virtual Rect getRect() {
			Rect rc;
			GetClientRect(_hwnd, rc.ref());
			return rc;
		}

		virtual void repaint(bool whole) {
			if (whole) {
				RedrawWindow(_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
			}

		}


		virtual void onCreateWindow();
		virtual void onDestroyWindow();

		virtual int onPaint() {
			return 0;
		}

		virtual int onEraseBackground() {
			return 0;
		}

		virtual void onSize(const Size& newSize) {}
		virtual void onMouseWheel(short distance, short vkey, short x, short y) {}
		virtual void onVScroll(short pos, short request, HWND ctrl) {}
		virtual void onHScroll(short pos, short request, HWND ctrl) {}
		virtual void onMouseDown(int vkey, const Point& pos) {
			this->mouseCapture();
		}
		virtual void onMouseUp(int vkey, const Point& pos) {
			this->mouseRelease();
		}
		virtual void onMouseDblClk(int vkey, const Point& pos) {}
		virtual void onKeyDown(int vkey, int info) {}
		virtual void onKeyUp(int vkey, int info) {}
		virtual void onMouseMove(int vkey, const Point& pos);
		virtual int onGetDialogCode() {
			return 0;
		}

		virtual int windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);

		static int CALLBACK staticWindowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);

		inline HWND getHwnd() {
			return _hwnd;
		}
		inline HWND hwnd() {
			return _hwnd;
		}

	protected:
		HWND _hwnd;

	};

	/** Groups several controls */
	class WindowDrawableControl: public Window {
	public:

		WindowDrawableControl(const oDrawableControl& ctrl) : _ctrl(ctrl) {
			bufferedPaint = true;
			eraseBrush = GetSysColorBrush(COLOR_BTNFACE);
		}

		void createAuto(const Point& pos, HWND parent, int id = 0, int style = WS_CHILD | WS_VISIBLE , int styleEx = 0) {
			S_ASSERT(_ctrl);
			Window::create("", style, styleEx, Rect(pos, pos + _ctrl->getRect().getRB()), parent, id);
		}

		void setControl(const oDrawableControl& ctrl) {
			_ctrl = ctrl;
		}
		oDrawableControl getControl() {
			return _ctrl;
		}

		virtual void repaint(bool whole) {
			if (whole) {
				Window::repaint(whole);
			} else {
				Region rgn;
				_ctrl->getRepaintRegion(rgn, whole);
				RedrawWindow(_hwnd, NULL, rgn, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
			}

		}

	public:
		bool bufferedPaint;
		HBRUSH eraseBrush;

	protected:

		virtual int onPaint();
		virtual int onEraseBackground();
		virtual void onMouseDown(int vkey, const Point& pos);
		virtual void onMouseMove(int vkey, const Point& pos);
		virtual void onMouseUp(int vkey, const Point& pos);

		oDrawableControl _ctrl;
	};

};