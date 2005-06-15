/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

/**@file

Obs³uga typów prostok¹ta, punktu i rozmiaru...
*/
#pragma once

#include <windows.h>
#include "Rect.h"


namespace Stamina {

	class Region {
	public:
		Region() {
			init();
		}
		Region(HRGN rgn) {
			init();
			set(rgn);
		}
		Region(const Region& rgn) {
			init();
			set(rgn);
		}
		Region(const Rect& rc);
		inline Region (const Rect& r, short roundX, short roundY=-1) {
			this->init();
			this->assign(createRoundRectRgn(r, roundX, roundY));
		}

		~Region() {
			this->clear();
		}

		Region& operator = (const Region& b) {
			this->set(b);
			return *this;
		}
		Region& operator = (HRGN b) {
			this->set(b);
			return *this;
		}
		operator bool() {
			return !this->isEmpty();
		}
		Region& operator += (const Point& pt) {
			this->offset(pt);
			return *this;
		}
		inline void assign(HRGN rgn) {
			this->clear();
			this->_rgn = rgn;
		}

		void set(HRGN rgn);
		void set(const Region& rgn);

		const HRGN get() const {
			return this->_rgn;
		}
		HRGN get() {
			if (!this->_rgn)
				this->_rgn = createEmptyRgn();
			return this->_rgn;
		}
		operator HRGN() {
			return get();
		}
		operator const HRGN() const {
			return get();
		}
		HRGN getCopy() const;
		Rect getBoundingBox() const;
		inline Rect getRect() const {
			return this->getBoundingBox();
		}

		bool contains(const Rect& rc);
		bool contains(const Point& pt);

		bool equal(const Region& rg);

		void or(const Rect& rc) {
			this->or(Region(rc));	
		}
		void or(const Region& rg);
		void diff(const Rect& rc) {
			this->diff(Region(rc));	
		}
		void diff(const Region& rg);
		void xor(const Rect& rc) {
			this->xor(Region(rc));	
		}
		void xor(const Region& rg);
		void and(const Rect& rc) {
			this->and(Region(rc));	
		}
		void and(const Region& rg);

		void clear();

		void offset(const Point& pt);

		bool isEmpty();

		void dcFill(HDC dc, HBRUSH brush);
		void dcFrame(HDC dc, HBRUSH brush, int bw, int bh);
		void dcInvert(HDC dc);
		void dcPaint(HDC dc);

		static HRGN createEmptyRgn();
		static HRGN createRectRgn(const Rect& r);
		static HRGN createRoundRectRgn(const Rect& r, short roundX, short roundY=-1);
	private:
		inline void init() {
			this->_rgn = 0;
		}
		void combine(HRGN rgn, int op);


		HRGN _rgn;
	};

};
