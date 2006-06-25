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

#include "Image.h"
#include "WinHelper.h"

namespace Stamina {

//	const double pi = 3.141592653;

	void drawSimpleGradient(HDC dc, int color1, int color2, const Rect& rect, short angle, short midpoint) {
		Rect rc = rect;
		rc.validate();
/*		float x = rc.left;
		float y = rc.top;
		float ox = cos((angle % 90) / 180 * pi);
		float oy = sin((angle % 90) / 180 * pi);
		while (x <= rc.right && y <= rc.bottom) {
            x+=ox;
			y+=oy;
		}*/
		/*Na razie tylko 0 i 90 stopni*/
		int length = (angle>45) ? rc.width() : rc.height();
		float mid = (float)midpoint/0xFF;
		HPEN oldPen = 0;
		for (int pos = 0; pos <= length; pos++) {
			float part = (float)pos/length;
			unsigned short blend = ((part > mid) ? 0.5 + ((part-mid)/(1-mid)/2) : part / mid / 2) * 0xFF;
			COLORREF color = blendRGB(color1, color2
				, 0xFF - blend);
			HPEN pen = CreatePen(PS_SOLID, 1, color);
			oldPen = (HPEN)SelectObject(dc, pen);
			if (angle > 45) {
				MoveToEx(dc, rc.left + pos, rc.top, 0);
				LineTo(dc, rc.left + pos, rc.bottom);
			} else {
				MoveToEx(dc, rc.left, rc.top + pos, 0);
				LineTo(dc, rc.right, rc.top + pos);
			}
			SelectObject(dc, oldPen);
			DeleteObject(pen);
		}

	}


	void drawSimpleGradient(HBITMAP bmp, int color1, int color2, const Rect& rect,  short angle, short midpoint) {
		HDC dc = CreateCompatibleDC(0);
		SelectObject(dc, bmp);
		drawSimpleGradient(dc, color1, color2, rect, angle, midpoint);
		DeleteDC(dc);
	}
	HBRUSH createSimpleGradientBrush(int color1, int color2, const Size& size, short angle, short midpoint) {
		HDC dc = GetDC(0);
		HBITMAP bmp = CreateCompatibleBitmap(dc, size.w, size.h);
		ReleaseDC(0, dc);
		drawSimpleGradient(bmp, color1, color2, Rect(Point(0,0), size), angle, midpoint);
		HBRUSH brush = CreatePatternBrush(bmp);
		DeleteObject(bmp);
		return brush;
	}
	oImage createSimpleGradient(int color1, int color2, const Size& size, short angle, short midpoint) {
		HDC dc = GetDC(0);
		HBITMAP bmp = CreateCompatibleBitmap(dc, size.w, size.h);
		ReleaseDC(0, dc);
		drawSimpleGradient(bmp, color1, color2, Rect(Point(0,0), size), angle, midpoint);
		return new Bitmap32(bmp, false, false);
	}



	// ------------------------------------------------------

	void Image::drawCentered(HDC dc, const Point& pt, const DrawOpts* opt) {
		Size sz = this->getSize();
		this->draw(dc, Point(pt.x - sz.w / 2, pt.y - sz.h / 2), opt);
	}


	void Image::drawStretched(HDC dc, const Rect& rc, const DrawOpts* opt) {
		DrawOpts _opt (opt);
		_opt.set(dmSize);
		_opt.size = rc.getSize();
		this->draw(dc, rc.getPos(), &_opt);
	}

	void Image::drawPattern(HDC dc, const Region& rgn, const Point& origin, const DrawOpts* opts) {
        Region old;
		int hadOne = GetClipRgn(dc, old);
		SelectClipRgn(dc, rgn);
		drawPattern(dc, rgn.getBoundingBox(), origin, opts);
		if (hadOne)
			SelectClipRgn(dc, old);
		else
			SelectClipRgn(dc, 0);
	}
	void Image::drawPattern(HDC dc, const Rect& rc, const Point& origin, const DrawOpts* _opt) {
		DrawOpts opt (_opt);
		Point pt = origin;
		Size sz = this->getSize();
		// sprowadzamy origin w pobli¿e punktu startowego
		pt.x = pt.x % sz.w;
		pt.y = pt.y % sz.h;
		// "odwracamy" znak
		if (pt.x < 0) pt.x += sz.w;
		if (pt.y < 0) pt.y += sz.h;
		// ustawiamy na start uwzglêdniaj¹c przed chwil¹ policzony origin
		pt = rc.getLT() - pt;
		int startX = pt.x;
		opt.set(dmSource);
		opt.set(dmSize);
		while (pt.y < rc.bottom) {
			opt.source = Rect(Point(), sz);
			Point drawPt = pt;
			// przycinamy w miarê potrzeb...
			if (pt.x < rc.left) {
				opt.source.left += rc.left - pt.x;
				drawPt.x = rc.left;
			}
			if (pt.x + sz.w > rc.right)
				opt.source.right -= pt.x + sz.w - rc.right;
			if (pt.y < rc.top) {
				opt.source.top += rc.top - pt.y;
				drawPt.y = rc.top;
			}
			if (pt.y + sz.h > rc.bottom)
				opt.source.bottom -= pt.y + sz.h - rc.bottom;
			opt.size = opt.source.getSize();

			this->draw(dc, drawPt, &opt);

			pt.x += sz.w;
			if (pt.x >= rc.right) {
				pt.x = startX;
				pt.y += sz.h;
			}
		}

	}

	oImage Image::loadIcon32(HINSTANCE inst, const char* id, int size, int bits) {
		HGDIOBJ obj = 0;
		oImage image;
		int r = Stamina::loadIcon32(obj, inst, id, size, bits, false);
		S_ASSERT(obj);
		if (obj) {
			if (r==IMAGE_ICON) {
				image = new Icon((HICON)obj, false);
			} else if (r==IMAGE_BITMAP) {
				image = new Bitmap32((HBITMAP)obj, true, false);
			}
		}
		if (image)
			image->setSize(Size(size, size));
		return image;
	}



// -----------------------------------------------------------------


	Icon::Icon(const char* filename, int size, int bits) {
		_icon = loadIconEx(0, filename, size, bits);
		_shared = false;
		S_ASSERT(_icon);
	}
	Icon::Icon(HICON icon, bool shared):_icon(icon), _shared(shared) {
		S_ASSERT(_icon);	
	}
	Icon::Icon(HINSTANCE hinst, const char* resId, int size, int bits) {
		_icon = loadIconEx(hinst ? hinst : Stamina::getInstance(), resId, size, bits);
		_shared = false;
		S_ASSERT(_icon);
	}

	Icon::~Icon() {
		if (!_shared && _icon)
			DestroyIcon(_icon);
	}
	Size Icon::getImageSize() {
		if (!_size) {
			ICONINFO ii;
			bool result = GetIconInfo(_icon , &ii) != 0;
			S_ASSERT(result);
			BITMAP b;
			GetObject(ii.hbmColor , sizeof(BITMAP),&b);
			_size.w = b.bmWidth;
			_size.h = b.bmHeight;
			DeleteObject(ii.hbmColor);
			DeleteObject(ii.hbmMask);
		}
		return _size;
	}
	void Icon::draw(HDC dc, const Point& pt, const DrawOpts* opts) {
		Size sz = Image::getOptSize(opts, this->getSize());
		DrawIconEx(dc, pt.x, pt.y, _icon, sz.w, sz.h, 0, 0, DI_NORMAL);

	}


	Size ImageListIndex::getImageSize() {
		Size sz;
		ImageList_GetIconSize(_iml , &sz.w , &sz.h);
		return sz;
	}

	void ImageListIndex::draw(HDC dc, const Point& pt, const DrawOpts* opts) {
		Size sz = Image::getOptSize(opts, this->getSize());
        S_ASSERT(_idx >= 0);
        IMAGELISTDRAWPARAMS ildp;
        ildp.cbSize = sizeof(IMAGELISTDRAWPARAMS);
        ildp.himl = _iml;
        ildp.i = _idx;
        ildp.hdcDst = dc;
        ildp.x = pt.x;
        ildp.y = pt.y;
        ildp.cx = sz.w;
		ildp.cy = sz.h;
		ildp.xBitmap = ildp.yBitmap = 0;
        ildp.rgbBk = CLR_NONE;
        ildp.rgbFg = CLR_NONE;
        ildp.fStyle = ILD_TRANSPARENT;
        ildp.fState = 0;
        ildp.dwRop = 0;
        ildp.Frame = 0;
        ildp.crEffect = 0x00FF0000;
        if (!ImageList_DrawIndirect(&ildp))
            ImageList_Draw(_iml,_idx,dc , pt.x,pt.y,0);
	}

	// ---------------------------------------------------------

	Bitmap::Bitmap(HBITMAP bmp, bool shared) {
		_bitmap = bmp;
		_shared = shared;
	}
	Bitmap::Bitmap(const char* filename, bool trans) {
		_bitmap = (HBITMAP) LoadImage(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | (trans ? LR_LOADTRANSPARENT : 0));
		_shared = false;
	}
	Bitmap::Bitmap(HINSTANCE hinst, const char* resId, bool trans) {
		_bitmap = (HBITMAP) LoadImage(hinst ? hinst : Stamina::getInstance(), resId, IMAGE_BITMAP, 0, 0, (trans ? LR_LOADTRANSPARENT : 0));
		_shared = false;
	}

	void Bitmap::setBitmap(HBITMAP bmp, bool shared) {
		if (_bitmap && !_shared)
			DeleteObject(_bitmap);
		_bitmap = bmp;
		_shared = shared;
	}


	Bitmap::~Bitmap() {
		this->setBitmap(0, false);
	}
	Size Bitmap::getImageSize() {
		BITMAP b;
		if (GetObject(this->_bitmap , sizeof(BITMAP),&b) > 0) {
			return Size(b.bmWidth, b.bmHeight);
		} else {
			return Size();
		}
	}

	void Bitmap::draw(HDC dc, const Point& pt, const Image::DrawOpts* opt) {
		Size sz = Image::getOptSize(opt, this->getImageSize());
		Rect src = Image::getOptSource(opt, Rect());
		HDC hdcmem = CreateCompatibleDC(dc);
		SelectObject(hdcmem , _bitmap);
		BitBlt(dc, pt.x, pt.y, sz.w, sz.h, hdcmem, src.left, src.top, SRCCOPY);
		DeleteDC (hdcmem);
	}


	// ---------------------------------------------------------

/*	Bitmap32::Bitmap32(const char* iconfile, int size) {
		HICON icon = loadIconEx(0, iconfile, size, 32);
		S_ASSERT(icon);
		ICONINFO ii;
		bool result = GetIconInfo(icon , &ii) != 0;
		S_ASSERT(result);
		DeleteObject(ii.hbmMask);
		_bitmap = ii.hbmColor;
		_shared = false;
		_transparent = true;
		this->prepareBitmap();
	}*/
	Bitmap32::Bitmap32(HBITMAP bmp, bool transparent, bool shared):Bitmap(bmp, shared) {
		S_ASSERT(bmp);
		_transparency = transparent;
		this->prepareBitmap();
	}
	void Bitmap32::setBitmap(HBITMAP bmp, bool transparent, bool shared) {
		S_ASSERT(bmp);
		Bitmap::setBitmap(bmp, shared);
		_transparency = transparent;
		this->prepareBitmap();
	}


	void Bitmap32::draw(HDC dc, const Point& pt, const Image::DrawOpts* opt) {
		//BITMAP bmp;
		//SetStretchBltMode(dc, COLORONCOLOR);
		Size sz = Image::getOptSize(opt, this->getSize());
		//GetObject(_bitmap , sizeof(BITMAP),&bmp);
		//bool hasAlpha = (bmp.bmBitsPixel == 32);
		HDC hdcmem = CreateCompatibleDC(dc);
		SelectObject(hdcmem , _bitmap);

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = Image::getOptAlpha(opt);
		bf.AlphaFormat = _transparency ? AC_SRC_ALPHA : 0;
		Rect src = Image::getOptSource(opt, Rect(Point(), this->getImageSize()));
		AlphaBlend(dc, pt.x, pt.y, sz.w, sz.h, hdcmem, src.left, src.top, src.width(), src.height(), bf);
		DeleteDC(hdcmem);
	}

	void Bitmap32::prepareBitmap() {
		if (_transparency) {
			if (!_bitmap) return;
			BITMAP bmp;
			if (GetObject(_bitmap , sizeof(BITMAP),&bmp) <= 0) return;
			BITMAPINFO bi;
			memset(&bi, 0, sizeof(bi));
			bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi.bmiHeader.biWidth = bmp.bmWidth;
			bi.bmiHeader.biHeight = bmp.bmHeight;
			bi.bmiHeader.biPlanes = bmp.bmPlanes;
			bi.bmiHeader.biBitCount= bmp.bmBitsPixel;
			bi.bmiHeader.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;
			bi.bmiHeader.biCompression = BI_RGB;
			HDC dc = GetDC(0);
			//HDC dc = CreateCompatibleDC(0);
			int scanLines;

//			scanLines = GetDIBits(dc, _bitmap, 0, 0, 0, &bi, DIB_RGB_COLORS);
//			S_ASSERT(scanLines);
			char* bits = new char [bi.bmiHeader.biSizeImage];
			scanLines = GetDIBits(dc, _bitmap, 0, bi.bmiHeader.biHeight, bits, &bi, DIB_RGB_COLORS);
			S_ASSERT(scanLines);
			bitmapPremultiply(bits, bi.bmiHeader.biSizeImage / 4);
			scanLines = SetDIBits(dc, _bitmap, 0, bi.bmiHeader.biHeight, bits, &bi, DIB_RGB_COLORS);
			S_ASSERT(scanLines);
			ReleaseDC(0, dc);
			//DeleteDC(dc);
			delete [] bits;
		}
	}


};