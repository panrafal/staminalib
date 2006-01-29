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

#include "stdafx.h"
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <commctrl.h>
#include <stdstring.h>

#include "ToolTipX.h"
#include "ButtonX.h"
#include "WinHelper.h"
#include "Rect.h"
#include "Assert.h"

namespace Stamina {

	WNDPROC ButtonX::buttonClassProc = 0;


	class ButtonXTipTarget: public ToolTipX::TargetImpl {
	public:
		ButtonXTipTarget():_needsRefresh(false), _button(0) {
		}
		void detachButton(ButtonX* button) {
			ObjLocker lock(this, lockWrite);
			if (_button == button) {
				_button = 0;
			}
		}
		void attachButton(ButtonX* button) {
			ObjLocker lock(this, lockWrite);
			if (button != _button)
				_needsRefresh = true;
			_button = button;
		}
		ToolTipX::oTip getTip(ToolTipX::ToolTip * ttx) {
			ObjLocker lock(this, lockRead);
			if (!_button)
				return ToolTipX::oTip();
			return _button->getTipObject(ttx);
		}
		// zawsze w obrêbie, ¿eby automat nam go nie wywali³
		bool mouseAtTarget(ToolTipX::ToolTip * ttx,HWND window, const Point& screen,
 const Point& local) {
			ObjLocker lock(this, lockWrite);
			ButtonX* button = ButtonX::fromHWND(WindowFromPoint(screen));
			if (!button || !_button || !button->isEnabled())
				return false;
			if (button == _button) {
				if (_needsRefresh) {
					OutputDebugString("Refresh\n");
					ttx->refresh();
				}
				_needsRefresh = false;
				return _button->getTipObject(ttx);
			} else
				return false;
     }
	protected:
		ButtonX* _button;
		bool _needsRefresh;

	};

	const boost::intrusive_ptr<ButtonXTipTarget> _tipTarget = new ButtonXTipTarget(); 

	ToolTipX::oTarget ButtonX::getTipTarget() {
		return _tipTarget;
	}

	//------------------------------------------------------------

	ButtonX::ButtonX(const char * title ,int style, int x , int y , int w , int h , HWND parent , HMENU id , LPARAM param) {
		this->_hwnd = CreateWindowEx(0,"BUTTON",title , BS_PUSHBUTTON | style | WS_CHILD
			,x,y,w,h,parent,id,getInstance(), (HANDLE)param);
		this->initWindow();

	}
	ButtonX::ButtonX(HWND hwnd) {
		this->_hwnd = hwnd;
		this->initWindow();
	}
	ButtonX::~ButtonX() {
		_tipTarget->detachButton(this);
		if (this->_hwnd) {
			SetProp(this->_hwnd, "ButtonX*", 0);
		}
	}
	void ButtonX::initWindow() {
		this->_duringPaint = false;
		//this->_icon = 0;
		//this->_sharedIcon = false;
		//this->_text = "";
		//this->_iconW = this->_iconH = 0;
		//this->_iconMargin = 5;
		this->_pressed = false;

		int length = GetWindowTextLength(this->_hwnd);
		GetWindowText(this->_hwnd, this->_text.GetBuffer(length+1), length+1); 
		this->_text.ReleaseBuffer(length);
		SetWindowText(this->_hwnd, "");
		this->buttonClassProc = (WNDPROC)GetWindowLong(this->_hwnd, GWL_WNDPROC);
		SetProp(this->_hwnd, "ButtonX*", (HANDLE)this);
		SetWindowLong(this->_hwnd, GWL_WNDPROC, (LONG)ButtonX::windowProc);
		RedrawWindow(this->_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);

	}

	ButtonX * ButtonX::fromHWND(HWND wnd) {
		return (ButtonX *) GetProp(wnd, "ButtonX*");
	}
	void ButtonX::setIcon(HICON icon, bool shared) {
		this->_image = new Icon(icon, shared);
		RedrawWindow(this->_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}
	void ButtonX::setIcon(HIMAGELIST list, int pos) {
		this->_image = new ImageListIndex(list, pos);
	}
	void ButtonX::freeIcon() {
		this->_image.reset();
	}

	void ButtonX::drawCaption(HDC hdc, RECT updRect) {
		//if (!this->_icon) return;
		Rect textRc, clientRc;
		GetClientRect(this->_hwnd, textRc.ref());
		clientRc = textRc;
		Size imageSize = _image ? _image->getSize() : Size();
		bool enabled = IsWindowEnabled(this->_hwnd)!=0;
		int state = SendMessage(this->_hwnd, BM_GETSTATE, 0, 0);
		int style = GetWindowLong(this->_hwnd, GWL_STYLE);
		
		if (this->isFlat() && state == BST_FOCUS) {
			Rect focusRc = clientRc;
			focusRc.expand(-2, -2);
			DrawFocusRect(hdc, focusRc.ref());
		}

		if (!this->_text.empty()) {
			SetBkMode(hdc , TRANSPARENT);
			SetTextColor(hdc, enabled ? GetSysColor(COLOR_BTNTEXT) : GetSysColor(COLOR_BTNSHADOW));
			textRc.bottom = 0;
			textRc.right -=	6;
			if (this->_image)
				textRc.right -=	this->_iconMargin + imageSize.w;
			HFONT oldFont = (HFONT) SelectObject(hdc, (HGDIOBJ)SendMessage(this->_hwnd, WM_GETFONT, 0, 0));
			DrawText(hdc, this->_text, -1, textRc.ref(), DT_LEFT | DT_NOPREFIX | DT_CALCRECT | DT_WORDBREAK);
			Point pt, ptImg;
			bool imgAside = ((style & BS_VCENTER) == BS_VCENTER || (style & BS_VCENTER) == 0);
			int textStyle = 0;
			switch (style & BS_CENTER) {
				case BS_LEFT:
					pt.x = ((imgAside && this->_image.isValid()) ? (imageSize.w + this->_iconMargin) : 0) + 2;
					ptImg.x = 2;
					textStyle = DT_LEFT;
					break;
				case BS_RIGHT:
					pt.x = clientRc.width() - textRc.width() - 2;
					if (!imgAside && _image.isValid()) {
						ptImg.x = clientRc.width() - imageSize.w - 2;
					}
					textStyle = DT_RIGHT;
					break;
				default:
					pt.x = (clientRc.right - textRc.right + ((imgAside && this->_image.isValid()) ? (imageSize.w + this->_iconMargin) : 0))/2;
					if (!imgAside && _image.isValid()) 
						ptImg.x = clientRc.getCenter().x - imageSize.w / 2;
					textStyle = DT_CENTER;
					break;
			}
			if (imgAside) {
				ptImg.x = pt.x - imageSize.w - this->_iconMargin;
			}
			// BS_TOP / BOTTOM oznaczaj¹ pozycjê SAMEGO TEKSTU wzglêdem ikonki
			switch (style & BS_VCENTER) {
				case BS_TOP:
                    pt.y = (clientRc.bottom - textRc.bottom)/2 - (imageSize.h + _iconMargin) / 2;
					ptImg.y = (clientRc.bottom - imageSize.h)/2 + (textRc.bottom + _iconMargin) / 2;
					break;
				case BS_BOTTOM:
                    pt.y = (clientRc.bottom - textRc.bottom)/2 + (imageSize.h + _iconMargin) / 2;
					ptImg.y = (clientRc.bottom - imageSize.h)/2 - (textRc.bottom + _iconMargin) / 2;
					break;
				default: // w jednej linii na œrodku
					pt.y = (clientRc.bottom - textRc.bottom)/2;
					ptImg.y = (clientRc.bottom - imageSize.h)/2;
			}
			textRc.setPos(pt);
			DrawText(hdc, this->_text, -1, textRc.ref(), textStyle | DT_NOPREFIX | DT_WORDBREAK);
			SelectObject(hdc, (HGDIOBJ)oldFont);
			if (this->_image) {
				this->_image->draw(hdc, ptImg);
			}
		} else {
			if (this->_image) {
				this->_image->draw(hdc, Point((clientRc.right - imageSize.w)/2 , (clientRc.bottom - imageSize.h)/2));
			}
		}
	}

	void ButtonX::enable(bool enable) {
		EnableWindow(this->_hwnd, enable);
	}
	bool ButtonX::isEnabled() {
		return IsWindowEnabled(_hwnd)!=0;
	}

	void ButtonX::setText(const CStdString& text) {
		this->_text = text;
		repaintWindow(_hwnd);
	}

	void ButtonX::press() {
		SendMessage(this->_hwnd, BM_CLICK, 0, 0);
	}


	int CALLBACK ButtonX::windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		switch(message) {

		case WM_SETTEXT:
			ButtonX::fromHWND(hwnd)->_text = (char*) lParam;
			//break;
			return 0;
		case WM_GETTEXT:
			if (ButtonX::fromHWND(hwnd)->_duringPaint) {
				((char*)lParam)[0] = 0;
				return 0;
			}
			//break;
			//			strncpy((char*) lParam,ButtonX::fromHWND(hwnd)->_text , wParam);
			strncpy((char*) lParam,ButtonX::fromHWND(hwnd)->_text , wParam);
			return min(ButtonX::fromHWND(hwnd)->_text.length(), wParam-1);
		case WM_GETTEXTLENGTH:
			if (ButtonX::fromHWND(hwnd)->_duringPaint) return 0;
			//break;
			return ButtonX::fromHWND(hwnd)->_text.length();

		case WM_PAINT: {
			ButtonX * bt = ButtonX::fromHWND(hwnd);
			//if (!bt->_icon) break; // standardowa obs³uga...
			//int r = 1;
			HRGN hrgn=CreateRectRgn(0, 0, 0, 0);
			bt->_duringPaint=true;
			GetUpdateRgn(hwnd, hrgn, false);
			int r = 1;
			if (!bt->isFlat()) {
				r = CallWindowProc(ButtonX::buttonClassProc, hwnd, message, wParam, lParam);
			}
			InvalidateRgn(hwnd, hrgn, false);

			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			if (bt->isFlat()) {
				FillRgn(hdc, hrgn, GetSysColorBrush(COLOR_BTNFACE));
			}
			RECT rc = {0,0,0,0};
			bt->drawCaption(hdc, rc);
			EndPaint(hwnd, &ps);
			bt->_duringPaint=false;
			DeleteObject(hrgn);
			return r;}
		case WM_ENABLE: case WM_UPDATEUISTATE: {
			int r = CallWindowProc(ButtonX::buttonClassProc, hwnd, message, wParam, lParam);
			repaintWindow(hwnd);
			return r;}
		case WM_ERASEBKGND:
			return 0;

		case WM_SETFOCUS:
		//case BM_SETSTYLE:
		case BM_SETSTATE:
			// specjalnie dla W98
			repaintWindow(hwnd);
			break;

		case WM_SETCURSOR:
			if (fromHWND(hwnd)->isFlat()) {
				SetCursor( LoadCursor(0, IDC_HAND) );
				return true;
			}
			break;

#ifndef STAMINA_KISS
		case WM_LBUTTONDOWN: {
			ButtonX* b = fromHWND(hwnd);
			S_ASSERT(b);
			b->_pressed = true;
			b->evtMouseDown(b, wParam);
			break;}
		case WM_LBUTTONUP: {
			ButtonX* b = fromHWND(hwnd);
			S_ASSERT(b);
			if (b->_pressed) {
				b->evtMouseUp(b, wParam);
				if (getClientRect(hwnd).contains(Point::fromLParam(lParam))) {
					b->evtClick(b);
				}
			}
			b->_pressed = false;
			break;}
		case WM_LBUTTONDBLCLK: {
			//SendMessage(hwnd, BM_CLICK, 0, 0);
			ButtonX* b = fromHWND(hwnd);
			S_ASSERT(b);
			b->evtMouseDown(b, wParam);
			b->evtMouseUp(b, wParam);
			b->evtClick(b);
			break;}
		case WM_MOUSELEAVE:
			//fromHWND(hwnd)->_pressed = false;
			break;
		case WM_MOUSEMOVE:
			_tipTarget->attachButton(fromHWND(hwnd));
			ToolTipX::mouseMovement(GetParent(hwnd));
			//if (wParam == MK_LBUTTON) {
			//	fromHWND(hwnd)->_pressed = true;
			//}
			break;

#endif

		case WM_DESTROY:
			delete ButtonX::fromHWND(hwnd);
			break;

		};

		return CallWindowProc(ButtonX::buttonClassProc, hwnd, message, wParam, lParam);
	}



};