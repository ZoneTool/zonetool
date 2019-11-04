// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW5/Assets/LightDef.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		ILightDef::ILightDef()
		{
		}

		ILightDef::~ILightDef()
		{
		}

		GfxLightDef* ILightDef::parse(const std::string& name, ZoneMemory* mem)
		{
			// parse iw5 lightdef
			auto iw5_asset = IW5::ILightDef::parse(name, mem);

			if (!iw5_asset)
			{
				return nullptr;
			}

			// fixup iw4 structure
			auto asset = mem->Alloc<GfxLightDef>();
			memcpy(asset, iw5_asset, sizeof GfxLightDef);
			asset->lmapLookupStart, iw5_asset->lmapLookupStart;

			// return converted asset
			return asset;
		}

		void ILightDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).lightdef;
			}
		}

		void ILightDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ILightDef::load_depending(IZone* zone)
		{
			auto asset = this->asset_;

			if (asset->attenuation.image)
			{
				zone->add_asset_of_type(image, asset->attenuation.image->name);
			}
		}

		std::string ILightDef::name()
		{
			return this->name_;
		}

		std::int32_t ILightDef::type()
		{
			return lightdef;
		}

		void ILightDef::write(IZone* zone, ZoneBuffer* buf)
		{
			assert(sizeof(GfxLightDef), 16);

			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->attenuation.image)
			{
				dest->attenuation.image = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
					image, data->attenuation.image->name));
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ILightDef::dump(GfxLightDef* asset)
		{
			// allocate memory_ for the asset
			auto iw5_asset = new IW5::GfxLightDef;

			// copy data
			memcpy(iw5_asset, asset, sizeof GfxLightDef);
			memset(&iw5_asset->cucoloris, 0, sizeof GfxLightImage);
			iw5_asset->lmapLookupStart = asset->lmapLookupStart;

			// dump data
			IW5::ILightDef::dump(iw5_asset);

			// free memory_
			delete iw5_asset;
		}
	}
}
