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

#include "windowFrame.h"
#include "../WinHelper.h"

namespace Stamina { namespace UI {

	// ----------------------------------------------------------------

	const char * const windowClassName = "Stamina::WindowFrame";

	WindowFrame::WindowFrame(const char* text, int style, int styleEx, const Rect& rect , HWND parent , int id)
	{
		create(text, style, styleEx, rect, parent, id);
	}

	WindowFrame::WindowFrame(HWND hwnd):Window(hwnd)
	{
	}


	void WindowFrame::create(const char* text, int style, int styleEx, const Rect& rect , HWND parent , int id) {
		registerWindowClass();
		S_ASSERT(this->_hwnd == 0);
		CreateWindowEx(styleEx, windowClassName, text, 
			style, 
			rect.left, rect.top, rect.width(), rect.height(), parent, (HMENU)id, Stamina::getInstance(), this);
	}

	void WindowFrame::registerWindowClass() {
		static bool once = false;
		if (once) return;
		once = true;
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style		= CS_DBLCLKS/* | CS_PARENTDC*/;
		wcex.lpfnWndProc	= (WNDPROC)staticWindowProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 4;
		wcex.hInstance		= Stamina::getInstance();
		wcex.hIcon			= 0;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= GetSysColorBrush(COLOR_WINDOW);
		wcex.lpszMenuName	= "";
		wcex.lpszClassName	= windowClassName;
		wcex.hIconSm = 0 ;
		RegisterClassEx(&wcex);
	}

	WindowFrame* WindowFrame::fromHWND(HWND wnd) {
		WindowFrame* w = (WindowFrame*)GetWindowLong(wnd, GWL_USERDATA);
		//S_ASSERT_MSG(w, "Stamina::Window control is not initialized properly!");
		return w;
	}

	void WindowFrame::onCreateWindow() {
		SetWindowLong(this->_hwnd, GWL_USERDATA, (LONG) this);
		__super::onCreateWindow();
	}
	void WindowFrame::onDestroyWindow() {
		__super::onDestroyWindow();
	}


	int WindowFrame::windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam, bool& handled) {
		handled = false;
		int ret = __super::windowProc(hwnd, message, wParam, lParam, handled);
		if (handled == true) {
			return ret;
		} else {
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	int CALLBACK WindowFrame::staticWindowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		WindowFrame* window;
		if (message == WM_NCCREATE) {
			CREATESTRUCT * cs = (CREATESTRUCT*)lParam;
			window = (WindowFrame*) cs->lpCreateParams;
			window->_hwnd = hwnd;
			window->onCreateWindow();
			return 1;
		}
		if (message == WM_NCDESTROY)
			return 0;
		window = fromHWND(hwnd);
		if (message != WM_GETMINMAXINFO) { // przed WM_NCCREATE lubi sobie przyjœæ ten komunikat...
			S_ASSERT(window);
		}
		if (!window) return 0;
		bool handled;
		return window->windowProc(hwnd, message, wParam, lParam, handled);
	}




// -----------------------------------------------------------------------

	int WindowDrawableControl::onPaint() {
		PAINTSTRUCT ps;
		Region rgn;
		GetUpdateRgn(this->_hwnd, rgn, false);
		HDC dc = BeginPaint(this->_hwnd, &ps);
		HDC origDC = dc;
		Rect rc;
		HBITMAP memBmp;
		if (bufferedPaint) {
		    dc = CreateCompatibleDC(origDC);
			rc = this->getRect();
			memBmp = CreateCompatibleBitmap(origDC , rc.getWidth(), rc.getHeight());
			HGDIOBJ oldBmp = SelectObject(dc , memBmp);
			if (oldBmp) DeleteObject(oldBmp);
		}

		if (this->eraseBrush)
			rgn.dcFill(dc, this->eraseBrush);
		_ctrl->draw(dc, Point());

		if (this->bufferedPaint) {
			BitBlt(origDC , 0 , 0 , rc.getWidth() , rc.getHeight()
				, dc , 0 , 0 , SRCCOPY);
			DeleteDC(dc);
			DeleteObject(memBmp);
		}
		EndPaint(this->_hwnd, &ps);
		return 0;
	}
	int WindowDrawableControl::onEraseBackground() {
		return 0;
	}
	void WindowDrawableControl::onMouseDown(int vkey, const Point& pos) {
		__super::onMouseDown(vkey, pos);
		_ctrl->onMouseDown(vkey, pos);
	}
	void WindowDrawableControl::onMouseMove(int vkey, const Point& pos) {
		_ctrl->onMouseMove(vkey, pos);
		__super::onMouseMove(vkey, pos);
	}

	void WindowDrawableControl::onMouseUp(int vkey, const Point& pos) {
		_ctrl->onMouseUp(vkey, pos);
		__super::onMouseUp(vkey, pos);
	}



} };