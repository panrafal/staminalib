#pragma once

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: WinHelper.h 17 2005-06-18 12:52:05Z hao $
 */

#include <iostream>

namespace Stamina {

	class ConsoleProgress {
	public:
		ConsoleProgress(unsigned int from, unsigned int to, unsigned int width = 50) {
			//		_from = from;
			_to = to - from;
			_pos = 0;
			_width = width;
			_bars = 0;
		}
		void drawBar() {
			for (int i=0; i < _width; i++) {
				std::cout << ".";
			}
		}
		void doStep(unsigned int offset = 1) {
			_pos += offset;
			unsigned int newBars = ((float)_pos / _to) * _width;
			while (_bars < newBars) {
				std::cout << "#";
				_bars++;
			}
		}
	private:
		unsigned int _from, _to;
		unsigned int _pos;
		unsigned int _width;
		unsigned int _bars;

	};

};