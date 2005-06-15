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

#include "window.h"
#include "WinHelper.h"

namespace Stamina {

	const char * const windowClassName = "Stamina::Window";

	Window::Window(const char* text, int style, int styleEx, const Rect& rect , HWND parent , int id)
	{
		this->init();
		create(text, style, styleEx, rect, parent, id);
	}

	Window::Window(HWND hwnd)
	{
		this->init();
		this->_hwnd = hwnd;
	}

	void Window::init() {
		this->_hwnd = 0;
	}

	void Window::create(const char* text, int style, int styleEx, const Rect& rect , HWND parent , int id) {
		registerWindowClass();
		S_ASSERT(this->_hwnd == 0);
		CreateWindowEx(styleEx, windowClassName, text, 
			style, 
			rect.left, rect.top, rect.width(), rect.height(), parent, (HMENU)id, Stamina::getInstance(), this);
	}

	void Window::registerWindowClass() {
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

	Window* Window::fromHWND(HWND wnd) {
		Window* w = (Window*)GetWindowLong(wnd, GWL_USERDATA);
		S_ASSERT_MSG(w, "Stamina::Window control is not initialized properly!");
		return w;
	}

	void Window::onCreateWindow() {
		SetWindowLong(this->_hwnd, GWL_USERDATA, (LONG) this);
		this->hold();
	}
	void Window::onDestroyWindow() {
		this->_hwnd = 0;
		this->release();
	}


	int Window::windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		switch (message) {
			case WM_ERASEBKGND:
				return this->onEraseBackground();
			case WM_PAINT:
				return this->onPaint();

			case WM_SIZE:
				this->onSize(Size(LOWORD(lParam), HIWORD(lParam)));
			    break;
			case WM_DESTROY:
				this->onDestroyWindow();
				break;
			case WM_MOUSEWHEEL:
				this->onMouseWheel(HIWORD(wParam), LOWORD(wParam), HIWORD(lParam), LOWORD(lParam));
				return 0;
			case WM_VSCROLL:
				this->onVScroll(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
				return 0;
			case WM_HSCROLL:
				this->onHScroll(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
				return 0;
			case WM_GETDLGCODE:
				return this->onGetDialogCode();

			case WM_LBUTTONDOWN:
				this->onMouseDown(this->getMouseState(wParam), Point(LOWORD(lParam), HIWORD(lParam)));
				return 0;
			case WM_LBUTTONUP:
				this->onMouseUp(this->getMouseState(wParam), Point(LOWORD(lParam), HIWORD(lParam)));
				return 0;
			case WM_LBUTTONDBLCLK:
				this->onMouseDblClk(this->getMouseState(wParam), Point(LOWORD(lParam), HIWORD(lParam)));
				return 0;
			case WM_MOUSEMOVE:
				this->onMouseMove(this->getMouseState(wParam), Point(LOWORD(lParam), HIWORD(lParam)));
				return 0;

			case WM_KEYDOWN:
				this->onKeyDown(wParam, lParam);
				return 0;
			case WM_KEYUP:
				this->onKeyUp(wParam, lParam);
				return 0;

		};
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	int CALLBACK Window::staticWindowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		Window* window;
		if (message == WM_NCCREATE) {
			CREATESTRUCT * cs = (CREATESTRUCT*)lParam;
			window = (Window*) cs->lpCreateParams;
			window->_hwnd = hwnd;
			window->onCreateWindow();
			return 1;
		}
		if (message == WM_NCDESTROY)
			return 0;
		window = fromHWND(hwnd);
		S_ASSERT(window);
		return window->windowProc(hwnd, message, wParam, lParam);
	}

	void Window::onMouseMove(int vkey, const Point& pos) {
		ToolTipX::mouseMovement(this->_hwnd);
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
		Window::onMouseDown(vkey, pos);
		_ctrl->onMouseDown(vkey, pos);
	}
	void WindowDrawableControl::onMouseMove(int vkey, const Point& pos) {
		_ctrl->onMouseMove(vkey, pos);
		Window::onMouseMove(vkey, pos);
	}

	void WindowDrawableControl::onMouseUp(int vkey, const Point& pos) {
		_ctrl->onMouseUp(vkey, pos);
		Window::onMouseUp(vkey, pos);
	}


};