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

The Initial Developer of the Original Code is "STAMINA" - Rafa- Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa- Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

*/

#include "stdafx.h"
#include "WindowDialog.h"

namespace Stamina { namespace UI {

	WindowDialog::WindowDialog():Window() {
		_return = 0;
	}

	void WindowDialog::create(HINSTANCE instance, LPCTSTR id, HWND parent) {
		_isModal = false;
		CreateDialogParam(instance, id, parent, (DLGPROC)WindowDialog::staticDialogProc, (LPARAM)this);
	}
	int WindowDialog::createModal(HINSTANCE instance, LPCTSTR id, HWND parent) {
		_isModal = true;
		_return = DialogBoxParam(instance, id, parent, (DLGPROC)WindowDialog::staticDialogProc, (LPARAM)this);
		return _return;
	}

	WindowDialog* WindowDialog::fromHWND(HWND hwnd) {
		return (WindowDialog*) GetProp(hwnd, "Stamina::WindowDialog*");
	}

	void WindowDialog::onCreateWindow() {
		SetProp(_hwnd, "Stamina::WindowDialog*", this);
		__super::onCreateWindow();
	}


	int WindowDialog::windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam, bool& handled) {
		return __super::windowProc(hwnd, message, wParam, lParam, handled);
	}

	int CALLBACK WindowDialog::staticDialogProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		WindowDialog* window;
		if (message == WM_INITDIALOG) {
			window = (WindowDialog*) lParam;
			window->_hwnd = hwnd;
			window->onCreateWindow();
			return 1;
		}
		if (message == WM_NCDESTROY)
			return 0;
		window = fromHWND(hwnd);
		if (!window) return 0;
		bool handled;
		return window->windowProc(hwnd, message, wParam, lParam, handled);
	}


}}