/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include <stdstring.h>
#include <map>

using namespace std;
#include "ToolTipX.h"

namespace Stamina { namespace ToolTipX {

bool iTarget::operator==(const iTarget & b) const {
	return this == &b || this->getClass() == b.getClass();
}
int iTarget::getDelay(ToolTip * ttx) {
	return ttx->enterWait;
}

void iTarget::attach(ToolTip * ttx) {
	this->getTip(ttx)->attach(ttx->getTipWindow());
}
void iTarget::detach(ToolTip * ttx) {
	this->getTip(ttx)->detach(ttx->getTipWindow());
}



void Target::setTip(ToolTip * ttx, const oTip& tip) {
	bool majorChange = _tip.get() != tip.get();
	oTip oldTip = _tip;
	_tip = tip; 
	if (ttx && ttx->isCurrentTarget(this)) {
		//OutputDebugString("\nRefresh");
		if (majorChange && ttx->isAttached()) {
			if (oldTip)
				oldTip->detach(ttx->getTipWindow());
			if (tip)
				tip->attach(ttx->getTipWindow());
		}
		ttx->refresh();
	}
}


bool TargetWindow::mouseAtTarget(ToolTip * ttx,HWND window, const Point& screen, const Point& local) {
	return WindowFromPoint(screen) == this->_hwnd;
}

bool TargetId::mouseAtTarget(ToolTip * ttx,HWND window, const Point& screen, const Point& local) {
	HWND item = WindowFromPoint(screen);
	return item ? ( GetWindowLong(item, GWL_ID)==this->_id) : false;
}
bool TargetRect::mouseAtTarget(ToolTip * ttx,HWND window, const Point& screen, const Point& local) {
	return this->_rect.contains(local) != 0;
}

bool TargetRect::operator==(const iTarget& b) const {
	return (b.castObject(this) && this->_rect == b.castObject(this)->_rect);
}



// --------------------------------

void iTipCallBack::adjustSize(HDC hdc, Rect& rc) {
	if (checkCache()) _cachedTip->adjustSize(hdc, rc);
}
void iTipCallBack::paint(HDC hdc, const Rect& rc) {
	if (checkCache()) _cachedTip->paint(hdc, rc);
}
void iTipCallBack::attach(HWND hwnd) {
	if (checkCache()) _cachedTip->attach(hwnd);
}
void iTipCallBack::detach(HWND hwnd) {
	if (_cachedTip) _cachedTip->detach(hwnd);
	this->resetCache();
}
bool iTipCallBack::checkCache() {
	if (_cachedTip)
		return true;
	fillCache();
	return _cachedTip;
}

int iTipCallBack::parentMessageProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam, bool & returned) {
	if (_cachedTip) return _cachedTip->parentMessageProc(hwnd, message, wParam, lParam, returned);
	return 0;
}

// --------------------------------


TipText::TipText(const CStdString & txt, HFONT font, int width) {
	this->_text = txt;
	this->_width = width;
	if (font) {
		this->_font = font;
	} else {
		this->_font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
}
void TipText::adjustSize(HDC hdc, Rect& rc) {
	HFONT oldFont = (HFONT) SelectObject(hdc, (HGDIOBJ) this->_font);
	rc.bottom = 0;
	if (this->_width) rc.right = rc.left + this->_width;
	DrawText(hdc, this->_text, -1, rc.ref(), DT_LEFT | DT_NOPREFIX | DT_CALCRECT | DT_WORDBREAK);
	SelectObject(hdc, (HGDIOBJ)oldFont);
	rc.right+=2*margin;
	rc.bottom+=2*margin;
}
void TipText::paint(HDC hdc, const Rect& paintRc) {
	HFONT oldFont = (HFONT) SelectObject(hdc, (HGDIOBJ)this->_font);
	Rect rc(paintRc);
	rc.left+=margin;
	rc.top+=margin;
	SetBkMode(hdc , TRANSPARENT);
	DrawText(hdc, this->_text, -1, rc.ref(), DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);
	SelectObject(hdc, (HGDIOBJ)oldFont);
}

// --------------------------------------------------------------------

TipTextRich::TipTextRich(const CStdString& txt, HFONT font, RichEditFormat::fSetStyleCB styleCB, int width) {
	this->_text = txt;
	this->_rich = 0;
	this->_styleCB = styleCB;
	this->_width = width ? width : 200;
	this->_positioned = false;
	if (font) {
		this->_font = font;
	} else {
		this->_font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}

}
TipTextRich::~TipTextRich() {
	if (this->_rich)
		DestroyWindow(this->_rich);
}
void TipTextRich::adjustSize(HDC hdc, Rect& rc) {
	this->_positioned = false;
	RichEditHtml::setHTML(this->_rich , this->_text , this->_styleCB);
	SendMessage(this->_rich, EM_REQUESTRESIZE , 0, 0);
	rc = this->_rect;
	//OutputDebugString("\nsetSize");
}
void TipTextRich::paint(HDC hdc, const Rect& rc) {
	if (!this->_positioned) {
		this->_positioned = true;
		SetWindowPos(this->_rich, 0, rc.left, rc.top, rc.width(), rc.height(), SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
void TipTextRich::setText(const CStdString& text) {
	this->_text = text;
}
void TipTextRich::attach(HWND hwnd) {
	this->_rich = CreateWindowEx(0 , RICHEDIT_CLASS  , "" , WS_CHILD|ES_MULTILINE |ES_READONLY
		,0,0,this->_width,10,hwnd,0 , 0 , 0);
	RichEditHtml::init (this->_rich, true, GetSysColor(COLOR_INFOBK));
	SendMessage(this->_rich , WM_SETFONT , (WPARAM)this->_font , 1);
	SendMessage(this->_rich , EM_SETEVENTMASK, 0, SendMessage(this->_rich , EM_GETEVENTMASK, 0,0) | ENM_REQUESTRESIZE);
	SetProp(this->_rich, "TipTextRich*", this);
	//OutputDebugString("\nattach");
}
void TipTextRich::detach(HWND hwnd) {
	DestroyWindow(this->_rich);
	this->_rich = 0;
	//OutputDebugString("\ndetach");
}
TipTextRich * TipTextRich::fromHWND(HWND window) {
	return (TipTextRich*) GetProp(window, "TipTextRich*");
}

int TipTextRich::parentMessageProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam, bool & returned) {
	switch (message) {
		case WM_NOTIFY: {
			NMHDR * nmhdr = (NMHDR*) lParam;
			switch (nmhdr->code) {
				case EN_REQUESTRESIZE : {
					REQRESIZE * req = (REQRESIZE*)lParam;
					fromHWND(req->nmhdr.hwndFrom)->_rect = req->rc;
					break;}
			}
			break;}
	}
	return RichEditHtml::parentWindowProc(hwnd, message, wParam, lParam);
}
CStdString TipTextRich::getText() {
	return this->_text;
}

// --------------------------------------------------------------------

TipOnlyTitle::TipOnlyTitle(const CStdString & title, const oImage& icon, HFONT font):TipOnlyImage(icon) {
	this->_title = title;
	setFont(font);
}
void TipOnlyTitle::setFont(HFONT font) {
	if (font) {
		this->_font = font;
	} else {
		this->_font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
}
TipOnlyTitle::~TipOnlyTitle() {
}

void TipOnlyTitle::adjustSize(HDC hdc, Rect& rc) {
	HFONT oldFont = (HFONT) SelectObject(hdc, (HGDIOBJ) this->_font);
	rc.bottom = 0;
	if (!this->_title.empty()) {
		DrawText(hdc, this->_title, -1, rc.ref(), DT_LEFT | DT_NOPREFIX | DT_CALCRECT | DT_SINGLELINE);
	} else {
		rc.right = rc.bottom = 0;
	}
	SelectObject(hdc, (HGDIOBJ)oldFont);
	Size iconSize = this->_image ? this->_image->getSize() : Size();
	rc.bottom = max(rc.bottom, iconSize.h);
	rc.right += iconSize.w ? iconSize.w + (this->_title.empty() ? 0 : 2*margin) : 0; 
	rc.right+=2*margin;
	rc.bottom+=2*margin;

}
void TipOnlyTitle::paint(HDC hdc, const Rect& paintRc) {
	HFONT oldFont = (HFONT) SelectObject(hdc, (HGDIOBJ)this->_font);
	Rect rc(paintRc);
	rc.left += margin;
	rc.top += margin;
	//rc.right -= margin;
	rc.bottom -= margin;
	SetBkMode(hdc , TRANSPARENT);

	if (this->_image) {
		rc.right = rc.left + this->_image->getSize().w;
		this->_image->drawCentered(hdc, rc);
		rc.left = rc.right + 2*margin;
		rc.right = paintRc.right;
	}

	DrawText(hdc, this->_title, -1, rc.ref(), DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);
	SelectObject(hdc, (HGDIOBJ)oldFont);
}


// -------------------------------------------------------


TipOnlyImage::TipOnlyImage(const oImage& image) {
	setImage(image);
}
TipOnlyImage::~TipOnlyImage() {
}
void TipOnlyImage::adjustSize(HDC hdc, Rect& rc) {
	if (!this->_image) return;
	rc.setSize(this->_image->getSize());
}
void TipOnlyImage::paint(HDC hdc, const Rect& rc) {
	if (!this->_image) return;
	this->_image->draw(hdc, rc.getLT());

}
void TipOnlyImage::setImage(const oImage& image) {
	this->_image = image;
}
void TipOnlyImage::resetImage() {
	this->_image = oImage();
}

// --------------------------------------------------------------

oTip Tip::titleAndText(const oImage& image, const CStdString & title, const CStdString & text, bool textRich) {
	Tip * tip = new Tip();
	tip->add(oTip(new TipOnlyTitle(title, image)));
	if (textRich) {
		tip->add(oTip(new TipTextRich(text)));
	} else {
		tip->add(oTip(new TipText(text)));
	}
	return oTip(tip);
}
oTip Tip::titleTextAndImage(const oImage& icon, const CStdString & title, const CStdString & text, bool textRich, const oImage& image) {
	Tip * tip = new Tip();
	tip->add(oTip(new TipOnlyTitle(title, icon)));
	if (textRich) {
		tip->add(oTip(new TipTextRich(text)));
	} else {
		tip->add(oTip(new TipText(text)));
	}
	tip->add(oTip(new TipOnlyImage(image)));
	return oTip(tip);
}
oTip Tip::textAndImage(const CStdString & text, bool textRich, const oImage& image) {
	Tip * tip = new Tip();
	if (textRich) {
		tip->add(oTip(new TipTextRich(text)));
	} else {
		tip->add(oTip(new TipText(text)));
	}
	tip->add(oTip(new TipOnlyImage(image)));
	return oTip(tip);
}
void Tip::add(oTip tip, enAlign align) {
	this->_elements.push_back(Element(tip, Rect(), align));
}
void Tip::adjustSize(HDC hdc, Rect& rc) {
	int x = margin;
	int y = margin;
	int startX = x;
	rc.right = rc.bottom = 0;
	enAlign previousAlign = alignAuto;
	for (tElements::iterator it = this->_elements.begin(); it != this->_elements.end(); ++it) {
		Rect elRc (0,0,250,18);
		(*it)->adjustSize(hdc, elRc);
		int w = elRc.width();
		int h = elRc.height();
		/* Wrapujemy gdy:
		nowa szerokosc przekroczy wrapWidth
		lub
		szerokosc > wysokosc
		lub poprzedni ma alignLeft
		lub aktualny ma alignRight
		*/
		if (x != startX && (x+w > wrapWidth || (w > h))
			&& !(it->align & alignRight)
			&& !(previousAlign & alignLeft)
			|| (it->align & alignWrap)
			) 
		{
			x = startX; // wrapujemy...
			y = rc.bottom + 2*margin;
		}
		elRc.offset(Point(x,y));
		x += 2*margin + w;
		rc.right = max(rc.right, elRc.right);
		rc.bottom = max(rc.bottom, elRc.bottom);
		it->rc = elRc;
		previousAlign = it->align;
	}
	rc.right+=2*margin;
	rc.bottom+=2*margin;
}
void Tip::paint(HDC hdc, const Rect& rc) {
	for (tElements::iterator it = this->_elements.begin(); it != this->_elements.end(); ++it) {
		Rect elRc = it->rc;
		elRc.offset(rc.getLT());
		(*it)->paint(hdc, elRc);
		//FrameRect(hdc, &elRc, (HBRUSH)GetStockObject(BLACK_BRUSH));
	}
}
void Tip::attach(HWND hwnd) {
	for (tElements::iterator it = this->_elements.begin(); it != this->_elements.end(); ++it) {
		(*it)->attach(hwnd);
	}
}
void Tip::detach(HWND hwnd) {
	for (tElements::iterator it = this->_elements.begin(); it != this->_elements.end(); ++it) {
		(*it)->detach(hwnd);
	}
}
int Tip::parentMessageProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam, bool & returned) {
	for (tElements::iterator it = this->_elements.begin(); it != this->_elements.end(); ++it) {
		int r = (*it)->parentMessageProc(hwnd, message, wParam, lParam, returned);
		if (returned) return r;
	}
	return 0;
}
 
} }