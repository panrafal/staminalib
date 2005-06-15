/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */

#pragma once


#include "Image.h"

namespace Stamina {


	struct loadImageParams {
		loadImageParams() : size(0), bits(0), transparent(0) {}
		loadImageParams(short size, char bits) : size(size), bits(bits), transparent(false) {}
		short size;
		char bits;
		bool transparent;
	};

	oImage loadImageFromFile(const char* filenameString, loadImageParams loadParams = loadImageParams());
	oImage loadImageFromResource(HINSTANCE inst, const char* resType, const char* resId, int imageType, loadImageParams loadParams = loadImageParams());
	oImage loadImageByUrl(const char* urlString, HINSTANCE dll, loadImageParams loadParams = loadImageParams());

	class ImageIL: public Bitmap32 {
	public:
		//Bitmap32(const char* iconfile, int size);
		ImageIL(const char* filename);
		ImageIL(HINSTANCE inst, const char* resType, const char* resId);
		ImageIL(unsigned int ilid) {setImage(ilid);}
		ImageIL(void* pixels, unsigned int dataSize) {
			setImage(pixels, dataSize);
		}
	protected:
		void setImage(unsigned int ilid);
		void setImage(void* pixels, unsigned int dataSize);

	};


};
