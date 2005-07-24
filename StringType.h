/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: String.h 51 2005-07-18 10:13:35Z hao $
 */

#ifndef __STAMINA_STRINGTYPE__
#define __STAMINA_STRINGTYPE__

#pragma once


namespace Stamina {

	template<typename CHAR, typename CP = CP_ACP>
	class StringType {
	public:

		class const_iterator {
		public:
			iterator(const CHAR* ptr):_p(ptr) {
			}
			const_iterator& operator ++ () {
				_p++;
				return *this;
			}
			const_iterator& operator -- () {
				_p--;
				return *this;
			}
			const_iterator& operator + (unsigned int offset) {
				_p+=offset;
				return *this;
			}
			const_iterator& operator - (unsigned int offset) {
				_p-=offset;
				return *this;
			}
			const_iterator& operator += (unsigned int offset) {
				_p+=offset;
				return *this;
			}
			const_iterator& operator -= (unsigned int offset) {
				_p-=offset;
				return *this;
			}
			CHAR operator * () {
				return *_p;
			}

		private:
			const CHAR* _p;
		};


	};

}