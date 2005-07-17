/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id: Lib.h 13 2005-06-17 18:02:57Z milka $
 */
#pragma once
#ifndef __STAMINA_LIBINSTANCE__
#define __STAMINA_LIBINSTANCE__


namespace Stamina { 

	class LibInstance {
	public:

		inline static LibInstance& get() {
			return _instance;
		}

	public:

		bool operator == (const LibInstance& b) {
			return this == &b; // porównujemy tylko wskaŸniki...
		}

		virtual void free(void* buff) {
			::free(buff);
		}

	private:
		LibInstance() {}
		static LibInstance _instance;
	};

}

#endif