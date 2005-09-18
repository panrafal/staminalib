/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#include "stdafx.h"
#include "Region.h"

namespace Stamina {

	Region::Region(const Rect& rc) {
		this->_rgn = createRectRgn(rc);
	}

	void Region::combine(HRGN rgn, int op) {
		CombineRgn(this->get(), this->get(), rgn, op);
	}
	void Region::set(HRGN rgn) {
		if (!rgn) {
			this->clear();
			return;
		}
		CombineRgn(this->get(), rgn, 0, RGN_COPY);
	}
	void Region::set(const Region& rgn) {
		this->set(rgn.get());
	}

	HRGN Region::getCopy() const {
		HRGN r = createEmptyRgn();
		CombineRgn(r, this->get(), 0, RGN_COPY);
		return r;
	}
	Rect Region::getBoundingBox() const {
		Rect r;
		GetRgnBox(this->get(), r.ref());
		return r;
	}

	bool Region::contains(const Rect& rc) {
		return RectInRegion(this->get(), rc.ref()) != 0;
	}
	bool Region::contains(const Point& pt) {
		return PtInRegion(this->get(), pt.x, pt.y) != 0;
	}

	bool Region::equal(const Region& rg) {
		return EqualRgn(this->get(), rg.get()) != 0;
	}

	void Region::or(const Region& rg) {
		this->combine(rg, RGN_OR);
	}
	void Region::diff(const Region& rg) {
		this->combine(rg, RGN_DIFF);
	} 
	void Region::xor(const Region& rg) {
		this->combine(rg, RGN_XOR);
	}
	void Region::and(const Region& rg) {
		this->combine(rg, RGN_AND);
	}

	void Region::clear() {
		if (this->_rgn)
			DeleteObject(this->_rgn);
		this->_rgn = 0;
	}

	void Region::offset(const Point& pt) {
		OffsetRgn(this->get(), pt.x, pt.y);
	}

	bool Region::isEmpty() {
		return this->_rgn == 0;
	}

	void Region::dcFill(HDC dc, HBRUSH brush) {
		if (!this->isEmpty())
			FillRgn(dc, this->get(), brush);
	}
	void Region::dcFrame(HDC dc, HBRUSH brush, int bw, int bh) {
		if (!this->isEmpty())
			FrameRgn(dc, this->get(), brush, bw, bh);
	}
	void Region::dcInvert(HDC dc) {
		if (!this->isEmpty())
			InvertRgn(dc, this->get());
	}
	void Region::dcPaint(HDC dc) {
		if (!this->isEmpty())
			PaintRgn(dc, this->get());
	}

	HRGN Region::createEmptyRgn() {
		return CreateRectRgnIndirect(Rect().ref());
	}
	HRGN Region::createRectRgn(const Rect& r) {
		return CreateRectRgnIndirect(r.ref());
	}
	HRGN Region::createRoundRectRgn(const Rect& r, short roundX, short roundY) {
		if (roundY == -1) roundY = roundX;
		return CreateRoundRectRgn(r.left, r.top, r.right, r.bottom, roundX, roundY);
	}

};
