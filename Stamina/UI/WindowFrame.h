#pragma once

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */



#include "Window.h"

namespace Stamina { namespace UI {



	class WindowFrame: public Window {
	public:

		WindowFrame(const char* text, int style, int styleEx, const Rect& rect , HWND parent , int id);
		void create(const char* text, int style, int styleEx, const Rect& rect , HWND parent , int id);
		WindowFrame(HWND hwnd = 0);
		

		static void registerWindowClass();

		static WindowFrame* fromHWND(HWND wnd);



		virtual void onCreateWindow();
		virtual void onDestroyWindow();

		virtual int windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam, bool& handled);

		static int CALLBACK staticWindowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);


	protected:

	};


	/** Groups several controls */
	class WindowDrawableControl: public WindowFrame {
	public:

		WindowDrawableControl(const oDrawableControl& ctrl) : _ctrl(ctrl) {
			bufferedPaint = true;
			eraseBrush = GetSysColorBrush(COLOR_BTNFACE);
		}

		void createAuto(const Point& pos, HWND parent, int id = 0, int style = WS_CHILD | WS_VISIBLE , int styleEx = 0) {
			S_ASSERT(_ctrl);
			WindowFrame::create("", style, styleEx, Rect(pos, pos + _ctrl->getRect().getRB()), parent, id);
		}

		void setControl(const oDrawableControl& ctrl) {
			_ctrl = ctrl;
		}
		oDrawableControl getControl() {
			return _ctrl;
		}

		virtual void repaint(bool whole) {
			if (whole) {
				WindowFrame::repaint(whole);
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


} };