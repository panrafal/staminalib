/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#ifndef __STAMINA_BUTTONX__
#define __STAMINA_BUTTONX__

#include <windows.h>
#include <commctrl.h>
#include <stdstring.h>
#ifndef STAMINA_KISS
#include <boost\signals.hpp>
#endif

#include "iToolTipX.h"
#include "Object.h"
#include "Image.h"

namespace Stamina {
	class ButtonX: public LockableObject<iLockableObject> {
		public:

			STAMINA_OBJECT_CLASS(Stamina::ButtonX, iLockableObject);

			ButtonX(const char * title , int style, int x , int y , int w , int h , HWND parent , HMENU id , LPARAM param);
			ButtonX(HWND hwnd);
			~ButtonX();
			static ButtonX * fromHWND(HWND wnd);
			void setIcon(HICON icon, bool shared=false);
			void setIcon(HIMAGELIST list, int pos);
			void setImage(const oImage& image) {
				this->_image = image;
			}

			void setText(const CStdString& text);
			CStdString getText() {
				return _text;
			}

			HWND getHWND() {return _hwnd;}

			void enable(bool enable);
			bool isEnabled();

			bool isFlat() {
				return (GetWindowLong(this->_hwnd, GWL_STYLE) & BS_TYPEMASK) == BS_OWNERDRAW;
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

			Rect getRect() {
				Rect rc;
				GetClientRect(_hwnd, rc.ref());
				return rc;
			}


			void press();

			virtual ToolTipX::oTip getTipObject(ToolTipX::ToolTip* ttx) {
				return ToolTipX::oTip();
			}

			static ToolTipX::oTarget getTipTarget();

#ifndef STAMINA_KISS
			boost::signal<void (ButtonX* sender)> evtClick;
			boost::signal<void (ButtonX* sender, int mvkey)> evtMouseDown;
			boost::signal<void (ButtonX* sender, int mvkey)> evtMouseUp;
#endif

		private:
			void freeIcon();
			void initWindow();
			static int CALLBACK windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);
			void drawCaption(HDC hdc, RECT updRect);
			static WNDPROC buttonClassProc;

			oImage _image;
			static const int _iconMargin = 5;
			HWND _hwnd;
			CStdString _text;
			bool _duringPaint;
			bool _pressed;

	};


};

#endif
 