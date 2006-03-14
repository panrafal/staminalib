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
#include "../WinHelper.h"

namespace Stamina { namespace UI {

	Window::Window(HWND hwnd) {
		_hwnd = hwnd;
	}

	void Window::destroyWindow() {
		DestroyWindow(_hwnd);
	}


	void Window::onCreateWindow() {
		this->hold();
	}
	void Window::onDestroyWindow() {
		this->_hwnd = 0;
		this->release();
	}

	int Window::windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam, bool& handled) {
		handled = true;
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

			case WM_COMMAND:
				this->onCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
				return 0;

			case WM_CLOSE:
				this->onClose();
				return 0;

		};
		handled = false;
		return 0;
	}

	void Window::onMouseMove(int vkey, const Point& pos) {
		ToolTipX::mouseMovement(this->_hwnd);
	}



} };