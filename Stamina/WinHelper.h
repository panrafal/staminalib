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

#include "Rect.h"

namespace Stamina {
	
	void setInstance(HINSTANCE inst);
	HINSTANCE getInstance();


	int parseScrollRequest(HWND hwnd, short bar, short trackPos, short request, short line);
	
	bool getKeyState(int vkey);
	
	void repaintWindow(HWND hwnd);

	void getChildRect (HWND hwnd , Rect& r);

	inline Rect getChildRect (HWND hwnd) {
		Rect r;
		getChildRect(hwnd, r);
		return r;
	}

	inline void repaintWindow(HWND hwnd) {
			RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}

	inline Rect getClientRect(HWND hwnd) {
		Rect r;
		GetClientRect(hwnd, r.ref());
		return r;
	}

	inline Rect getWindowRect(HWND hwnd) {
		Rect r;
		GetWindowRect(hwnd, r.ref());
		return r;
	}



	void setParentsFont(HWND hwnd);
	void copyWindowFont(HWND from, HWND to);
	inline HFONT setWindowFont(HWND hwnd, HFONT font) {
		return (HFONT) SendMessage(hwnd, WM_SETFONT, (WPARAM)font, 0);
	}

	inline HFONT createFont(const char* face, int size, int weight=0, bool italic=false, bool underline=false) {
		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
		lf.lfCharSet = DEFAULT_CHARSET;
		strcpy(lf.lfFaceName, face);
		lf.lfWeight = weight == 1 ? FW_BOLD : weight;
		lf.lfHeight = size;
		lf.lfUnderline = underline;
		lf.lfItalic = italic;
		return CreateFontIndirect(&lf);
	}


	const char* vkeyToAscii(int vkey, int scan=-1);

	int removeDirTree(const char * path);


	void setRectSizeByEdge(int edge , RECT * rc , int w, int h);
	void clipSize(int edge , RECT * rc , int minW , int minH, int maxW, int maxH);

#ifdef _STRING_
	std::string regQueryString(HKEY hKey , const char * name, const char * def = 0);
	long regSetString(HKEY hKey , const char * name , const std::string& val);
#endif
	long regSetDWord(HKEY hKey , const char * name , int val);
	long regSetDWord(HKEY hKey , const char * name , int val);

	int getDllVersion(LPCTSTR lpszDllName);
	bool isComctl(unsigned int maj , unsigned int min);
	inline bool isWindowsXP() {
		return isComctl(6,0);
	}
	HICON loadIconEx(HINSTANCE hInst , LPCTSTR id , int size=32 , int bits=32);
	int loadIcon32(HGDIOBJ& object, HINSTANCE hInst, LPCTSTR id , int size , int bits, bool preferIcon);

	void bitmapPremultiply(void * pixels, unsigned int pixelsCount);
	void drawBitmapBlended (HDC hDC , HBITMAP hBmp , int x , int y, int alpha);

	int getFontHeight(HDC hdc);
	int getFontHeight(HWND hwnd, HFONT font);
	Size getTextSize(HDC hdc, const char* text, int width = 0);
	Size getTextSize(HWND hwnd, HFONT font, const char* text, int width = 0);

	Point getLocalPoint(HWND window, const Point& screen, int scrollX=-1, int scrollY=-1);
	Point getScreenPoint();

	bool windowBelongsTo(HWND window, HWND parent);

	void * loadResourceData(HMODULE inst, const char* name, const char* type, HGLOBAL& rsrc, int* size);

#ifdef _STRING_
	std::string expandEnvironmentStrings(const char * src, unsigned int size=300);
	inline std::string getFullPathName(LPCTSTR filename) {
		TCHAR buffer [MAX_PATH + 1];
		if (GetFullPathName(filename, MAX_PATH, buffer, 0))
			return buffer;
		else
			return "";
	}
#endif

	inline void runAPCqueue() {
		while (SleepEx(0, TRUE) == WAIT_IO_COMPLETION) {}
	}

};