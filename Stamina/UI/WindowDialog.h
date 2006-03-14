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

$Id: CriticalSection.cpp 160 2006-01-29 13:54:08Z hao $

*/

#pragma once

#include "Window.h"

namespace Stamina { namespace UI {

	class WindowDialog: public Window {
	public:

		WindowDialog();

		void create(HINSTANCE instance, LPCTSTR id, HWND parent);
		int createModal(HINSTANCE instance, LPCTSTR id, HWND parent);

		static WindowDialog* fromHWND(HWND wnd);

		virtual void onCreateWindow();

		virtual int windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam, bool& handled);

		static int CALLBACK staticDialogProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);

		int getReturn() const {
			return _return;
		}

		void endDialog(int ret) {
			if (_isModal) {
				EndDialog(_hwnd, ret);
			} else {
				_return = ret;
				this->destroyWindow();
			}
		}

	protected:

		bool _isModal;
		int _return;

	};


}}