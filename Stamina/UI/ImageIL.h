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

namespace Stamina { namespace UI {


	struct loadImageParams {
		loadImageParams() : size(0), bits(0), transparent(0) {}
		loadImageParams(Size size, char bits) : size(size), bits(bits), transparent(false) {}
		Size size;
		char bits;
		bool transparent;
	};

	oImage loadImageFromFile(const char* filenameString, loadImageParams loadParams = loadImageParams());
	oImage loadImageFromResource(HINSTANCE inst, const char* resType, const char* resId, int imageType, loadImageParams loadParams = loadImageParams());
	oImage loadImageByUrl(const char* urlString, HINSTANCE dll, loadImageParams loadParams = loadImageParams());

	class ImageIL: public Bitmap32 {
	public:
		//Bitmap32(const char* iconfile, int size);
		ImageIL(const char* filename, loadImageParams params);
		ImageIL(HINSTANCE inst, const char* resType, const char* resId, loadImageParams params);
		ImageIL(unsigned int ilid, loadImageParams params) {setImage(ilid, params);}
		ImageIL(void* pixels, unsigned int dataSize, loadImageParams params) {
			setImage(pixels, dataSize, params);
		}
	protected:
		void setImage(unsigned int ilid, loadImageParams params);
		void setImage(void* pixels, unsigned int dataSize, loadImageParams params);

	};


} };
