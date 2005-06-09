/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

/**@file

Obs³uga typów prostok¹ta, punktu i rozmiaru...
*/
#pragma once

#ifndef __STAMINA_RECT__
#define __STAMINA_RECT__

namespace Stamina {

	/**Punkt*/
	class Point {
	public:
		int x;
		int y;
		Point() {
			this->x = 0;
			this->y = 0;
		}
		Point(int x, int y) {
			this->x = x;
			this->y = y;
		}
		/*Point(const Point & b) {
			*this = b;
		}*/
		bool operator == (const Point & b) {
            return eq(b);
		}
		bool operator != (const Point & b) {
            return !eq(b);
		}

		bool eq(const Point &b) {
            return x == b.x && y == b.y;
		}

		void add(const Point & b) {
			x += b.x;
			y += b.y;
		}
		void sub(const Point & b) {
			x -= b.x;
			y -= b.y;
		}
		void negate() {
			x = -x;
			y = -y;
		}

		Point operator + (const Point & b) const {
			Point p(*this);
			p.add(b);
			return p;
		}
		Point& operator += (const Point & b) {
			this->add(b);
			return *this;
		}
		Point operator - (const Point & b) const {
			Point p(*this);
			p.sub(b);
			return p;
		}
		Point& operator -= (const Point & b) {
			this->sub(b);
			return *this;
		}
		Point operator - () const {
			Point p(*this);
			p.negate();
			return p;	
		}
		/*Point& operator = (const Point & b) {
			x = b.x;
			y = b.y;
			return *this;
		}*/

#ifdef _WINDEF_
		operator POINT() const {
			POINT pt = {x,y};
			return pt;
		}
		operator POINT&() {
			return *(reinterpret_cast<POINT*>(this));
		}
		POINT * ref() {
			return reinterpret_cast<POINT*>(this);
		}
#endif


		static Point fromLParam(int lP) {
			return Point(lP & 0xFFFF, lP >> 16 );
		}
	};

	/**Rozmiar*/
	class Size {
	public:
		union {
			int cx;
			int w;
		};
		union {
			int cy;	
			int h;
		};
		Size() {
			this->w = 0;
			this->h = 0;
		}
		Size(int w, int h) {
			this->w = w;
			this->h = h;
		}
		bool operator == (const Size & b) {
            return w == b.w && h == b.h;
		}
		operator bool() {
			return w != 0 || h != 0;
		}

	};


	/**Prostok¹t*/
	class Rect {
	public:
		int left;
		int top;
		int right;
		int bottom;

		Rect() {
			left = top = right = bottom = 0;
		}

		Rect(int left, int top, int right, int bottom) {
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
		}
		Rect(const Point & pt, const Size & sz) {
			this->setLT(pt);
			this->setSize(sz);
		}
		Rect(const Point & lt, const Point & rb) {
			this->setLT(lt);
			this->setRB(rb);
		}

		inline Point getPos() const {
			return Point(left, top);
		}
		inline Size getSize() const {
			return Size(right - left, bottom - top);
		}
		void setPos(const Point & pt) {
			this->right = pt.x + this->getWidth();
			this->bottom = pt.y + this->getHeight();
			this->left = pt.x;
			this->top = pt.y;
		}
		inline void setSize(const Size & sz) {
			this->right = this->left + sz.cx;
			this->bottom = this->top + sz.cy;
		}

		inline void setLT(const Point & pt) {
			this->left = pt.x;
			this->top = pt.y;
		}
		inline void setRB(const Point & pt) {
			this->right = pt.x;
			this->bottom = pt.y;
		}
		inline Point getLT() const {
			return getPos();
		}
		inline Point getRB() const {
			return Point(right, bottom);
		}
		inline int getWidth() const {
			return abs(right - left);
		}
		inline int getHeight() const {
			return abs(bottom - top);
		}
		inline int width() const {
			return getWidth();
		}
		inline int height() const {
			return getHeight();
		}
		inline Point getCenter() const {
			return Point(this->left + this->width() / 2, this->top + this->height() / 2);
		}

		inline void offset(const Point& pt) {
			this->setPos(this->getPos() + pt);
		}

		void expand(int x, int y) {
			this->left -=x;
			this->top -= y;
			this->right += x;
			this->bottom +=y;
		}

		void fitIn(const Rect& fitIn) {
			Point off;
			if (this->left < fitIn.left) off.x = fitIn.left - this->left;
			if (this->top < fitIn.top) off.y = fitIn.top - this->top;
			if (this->right > fitIn.right) off.x += fitIn.right - this->right;
			if (this->bottom > fitIn.bottom) off.y += fitIn.bottom - this->bottom;
			this->offset(off);
		}


		void validate() {
			if (this->top > this->bottom) {
				int a = this->top;
				this->top = this->bottom;
				this->bottom = a;
			}
			if (this->left > this->right) {
				int a = this->left;
				this->left = this->right;
				this->right = a;
			}
		}

		void include(const Rect& b) {
			if (b.left < left) left = b.left;
			if (b.top < top) top = b.top;
			if (b.right > right) right = b.right;
			if (b.bottom > bottom) bottom = b.bottom;
		}

		bool contains(const Point& pt) const {
			return this->isSet() && this->left <= pt.x && this->right >= pt.x && this->top <= pt.y && this->bottom >= pt.y;
		}
		bool contains(const Rect& rc) const {
			return this->isSet() && rc.isSet()
				&& this->containsW(rc)
				&& this->containsH(rc)
			;
		}
		bool intersects(const Rect& rc) const {
			return intersectsW(rc)
				&& intersectsH(rc);

		}
		inline bool containsW(const Rect& rc) const {
			return this->left   <= rc.left
				&& this->right  >= rc.right;
		}
		inline bool containsH(const Rect& rc) const {
			return this->top    <= rc.top
				&& this->bottom >= rc.bottom;
		}
		inline bool intersectsW(const Rect& rc) const {
			return ((rc.left > this->left ? (this->right - rc.left) : (rc.right - this->left)) >= 0);
		}
		inline bool intersectsH(const Rect& rc) const {
			return ((rc.top > this->top ? (this->bottom - rc.top) : (rc.bottom - this->top)) >= 0);
		}

		bool eq(const Rect& b) const {
			return 
				   this->top    == b.top
				&& this->left   == b.left
				&& this->right  == b.right
				&& this->bottom == b.bottom
			;
		}

		bool operator == (const Rect& b) const {
			return eq(b);
		}
		bool operator != (const Rect& b) const {
			return !eq(b);
		}

		operator bool() const {
			return isSet;
		}

		Rect operator - (const Point& pt) const {
			Rect r(*this);
			return r-=pt;
		}
		Rect& operator -= (const Point& pt) {
			this->offset(-pt);
			return *this;
		}
		Rect& operator += (const Point& pt) {
			this->offset(pt);
			return *this;
		}

		bool isSet() const {
			return this->top || this->bottom || this->left || this->right;
		}

#ifdef _WINDEF_
		Rect(const RECT& rc) {
			*this = rc;
		}
		Rect& operator = (const RECT& rc) {
			this->top = rc.top;
			this->left = rc.left;
			this->bottom = rc.bottom;
			this->right = rc.right;
			return *this;
		}

		operator RECT() {
			return *(reinterpret_cast<RECT*>(this));
		}
		operator RECT&() {
			return *(reinterpret_cast<RECT*>(this));
		}
		operator const RECT&() const {
			return *(reinterpret_cast<const RECT*>(this));
		}
		RECT * ref() {
			return reinterpret_cast<RECT*>(this);
		}
		const RECT * ref() const {
			return reinterpret_cast<const RECT*>(this);
		}
#endif
	};

	

};

#endif