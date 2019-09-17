// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW4/Assets/GfxImage.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		//IW4::GfxImage* IGfxImage::GenerateIW4Image(GfxImage* image)
		//{
		//	auto iw4_image = new IW4::GfxImage;
		//	ZeroMemory(iw4_image, sizeof IW4::GfxImage);

		//	// copy image data
		//	iw4_image->mapType = image->mapType;
		//	iw4_image->semantic = image->semantic;
		//	iw4_image->category = image->category;
		//	iw4_image->dataLen1 = image->cardMemory.platform[0];
		//	iw4_image->dataLen2 = image->cardMemory.platform[1];
		//	iw4_image->width = image->width;
		//	iw4_image->height = image->height;
		//	iw4_image->depth = image->depth;
		//	iw4_image->pad = image->delayLoadPixels;
		//	iw4_image->name = (char*)image->name;

		//	// alloc texture
		//	iw4_image->texture = (IW4::GfxImageLoadDef*)image->texture.loadDef;

		//	return iw4_image;
		//}

		void IGfxImage::createIWI(GfxImage* asset)
		{
			// create an IWI file based on the image asset
			// TODO
		}

		void IGfxImage::dump(GfxImage* asset)
		{
			//// alloc IW4 image
			//auto iw4_image = IGfxImage::GenerateIW4Image(asset);

			//// dump IW4 image
			//IW4::IGfxImage::dump(iw4_image);

			//// free IW4 image data
			//delete[] iw4_image;
		}
	}
}
