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
		IW4::GfxImage* IGfxImage::GenerateIW4Image(GfxImage* image, ZoneMemory* mem)
		{
			auto* iw4_image = mem->Alloc<IW4::GfxImage>();

			// copy image data
			iw4_image->mapType = image->mapType;
			iw4_image->semantic = image->semantic;
			iw4_image->category = image->category;
			iw4_image->dataLen1 = image->cardMemory.platform[0];
			iw4_image->dataLen2 = image->cardMemory.platform[1];
			iw4_image->width = image->width;
			iw4_image->height = image->height;
			iw4_image->depth = image->depth;
			iw4_image->pad = image->delayLoadPixels;
			iw4_image->name = (char*)image->name;

			// alloc texture
			iw4_image->texture = (IW4::GfxImageLoadDef*)image->texture.loadDef;

			return iw4_image;
		}
		
		void IGfxImage::dump(GfxImage* asset, ZoneMemory* mem)
		{
			if (!asset)
			{
				return;	
			}

			ZONETOOL_INFO("dumping map image %s", asset->name);
			
			// alloc IW4 image
			auto* iw4_image = IGfxImage::GenerateIW4Image(asset, mem);
			
			// dump IW4 image
			IW4::IGfxImage::dump(iw4_image);
		}
	}
}
