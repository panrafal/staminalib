/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
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
	/** ButtonX implements easy to use button class.

	Each one button can be drawn with icon, image or text only.
	*/
	class ButtonX: public LockableObject<iLockableObject> {
		public:

			STAMINA_OBJECT_CLASS(Stamina::ButtonX, iLockableObject);

			/** Constructor
			@param title String that contains text to display on the button.
			@param style Additional styles of button. (Windows Button Style, i.e. BS_LEFT )
			@param x X-coord of upper left button edge.
			@param y Y-coord of upper left button edge.
			@param w Button's width.
			@param h Button's height.
			@param parent HWND of parent window.
			@param id Handle of menu.
			@param param User defined data, which should be passed.
			*/
			ButtonX(const char * title , int style, int x , int y , int w , int h , HWND parent , HMENU id , LPARAM param);

			/** Constructor
			@param hwnd HWND of parent window.
			*/
			ButtonX(HWND hwnd);

			/** Destructor
			*/
			~ButtonX();

			/** Static metod to get pointer to ButtonX class from parent window
			@param wnd HWND of parent window
			@return Pointer to ButtonX class.
			*/
			static ButtonX * fromHWND(HWND wnd);

			/** Sets buttons icon.
			@param icon Handle to icon.
			@param shared If icon is shared with other windows.
			*/
			void setIcon(HICON icon, bool shared=false);

			/** Sets button's icon.
			@param list Handle to an image list.
			@param pos Position of icon in the image list.
			*/
			void setIcon(HIMAGELIST list, int pos);

			/** Sets button's image.
			@param image Pointer to Image class, that represents the image.
			*/
			void setImage(const oImage& image) {
				this->_image = image;
			}

			/** Sets button's text
			*/
			void setText(const CStdString& text);
			CStdString getText() {
				return _text;
			}

			/** Return handle of button.
			*/
			HWND getHWND() {return _hwnd;}

			/** Enables or disables mouse and keyboard input.
			*/
			void enable(bool enable);
			bool isEnabled();

			bool isFlat() {
				return (GetWindowLong(this->_hwnd, GWL_STYLE) & BS_TYPEMASK) == BS_OWNERDRAW;
			}

			/** Sets button's rectangle.
			@param rc Stamina::Rect object.
			*/
			virtual void setRect(const Rect& rc) {
				SetWindowPos(_hwnd, 0, rc.left, rc.top, rc.getWidth(), rc.getHeight(), SWP_NOZORDER);
			}

			/** Sets button's position.
			@param pt Stamina::Point object.
			*/
			virtual void setPos(const Point& pt) {
				SetWindowPos(_hwnd, 0, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}

			/** Sets button's size.
			@param sz Stamina::Size object.
			*/
			virtual void setSize(const Size& sz) {
				SetWindowPos(_hwnd, 0, 0, 0, sz.w, sz.h, SWP_NOZORDER | SWP_NOMOVE);
			}

			Rect getRect() {
				Rect rc;
				GetClientRect(_hwnd, rc.ref());
				return rc;
			}


			/** Presses the button.
			*/
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
 