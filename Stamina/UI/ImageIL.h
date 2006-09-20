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
