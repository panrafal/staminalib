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

#include "Control.h"

namespace Stamina { namespace UI {

	typedef SharedPtr<class Window> oWindow;

	class Window: public Control {
	protected:

		Window(HWND hwnd = 0);

	public:

		static oWindow fromHWND(HWND hwnd) {
			return new Window(hwnd);
		}
		
		virtual void destroyWindow();

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

		virtual HWND getChild(int id) {
			return GetDlgItem(_hwnd, id);
		}

		virtual oWindow getChildWindow(int id) {
			return fromHWND(GetDlgItem(_hwnd, id));
		}
        
		virtual void setText(const StringRef& text) {
			SetWindowTextW(_hwnd, text.w_str());
		}

		virtual String getText() {
		 	String txt;
			int size = GetWindowTextLength(_hwnd);
            GetWindowTextW(_hwnd, txt.useBuffer<wchar_t>(size), size);
			txt.releaseBuffer<wchar_t>(size);
			return PassStringRef(txt);
		}

		virtual void enableWindow(bool state) {
			EnableWindow(_hwnd, state);
		}

		virtual bool isEnabled() {
			return IsWindowEnabled(_hwnd) != 0;
		}

		virtual void showWindow(int show) {
			ShowWindow(_hwnd, show);
		}

		virtual bool isVisible() {
			return IsWindowVisible(_hwnd) != 0;
		}


		virtual int sendMessage(int message, WPARAM wParam, LPARAM lParam) {
			return SendMessage(_hwnd, message, wParam, lParam);
		}

		virtual bool postMessage(int message, WPARAM wParam, LPARAM lParam) {
			return PostMessage(_hwnd, message, wParam, lParam) != 0;
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

		virtual void onCommand(int code, int id, HWND sender) {}

		virtual void onClose() {
			this->destroyWindow();
		}

		virtual int windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam, bool& handled);

		inline HWND getHwnd() {
			return _hwnd;
		}
		inline HWND hwnd() {
			return _hwnd;
		}

	protected:
		HWND _hwnd;

	};





} };