// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		ILightDef::ILightDef()
		{
		}

		ILightDef::~ILightDef()
		{
		}

		void ILightDef::parseLightImage(GfxLightImage* image, nlohmann::json& data, ZoneMemory* mem)
		{
			if (data["image"].is_string())
			{
				auto imageName = data["image"].get<std::string>();

				if (imageName.size())
				{
					image->image = mem->Alloc<GfxImage>();
					image->image->name = mem->StrDup(imageName);
				}
			}

			image->samplerState = data["samplerState"].get<char>();
		}

		GfxLightDef* ILightDef::parse(const std::string& name, ZoneMemory* mem)
		{
			auto path = "lights/"s + name + ".lightdef"s;
			if (FileSystem::FileExists(path))
			{
				ZONETOOL_INFO("Parsing lightdef \"%s\"...", name.c_str());

				auto asset = mem->Alloc<GfxLightDef>();

				auto file = FileSystem::FileOpen(path, "rb");
				auto size = FileSystem::FileSize(file);
				auto bytes = FileSystem::ReadBytes(file, size);
				FileSystem::FileClose(file);

				nlohmann::json data = nlohmann::json::parse(bytes);

				asset->name = mem->StrDup(data["name"].get<std::string>());

				// parse light images
				parseLightImage(&asset->attenuation, data["attenuation"], mem);
				parseLightImage(&asset->cucoloris, data["cucoloris"], mem);

				asset->lmapLookupStart = data["lmapLookupStart"].get<int>();

				return asset;
			}

			return nullptr;
		}

		void ILightDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).lightdef;
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
			if (asset->cucoloris.image)
			{
				zone->add_asset_of_type(image, asset->cucoloris.image->name);
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
			assert(sizeof(GfxLightDef), 24);

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
			if (data->cucoloris.image)
			{
				dest->cucoloris.image = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
					image, data->cucoloris.image->name));
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ILightDef::dump(GfxLightDef* asset)
		{
			Json& data = asset->ToJson();

			std::string path = "lights/"s + asset->name + ".lightdef"s;
			std::string json = data.dump(4);

			auto file = FileSystem::FileOpen(path, "w"s);
			fwrite(json.data(), json.size(), 1, file);
			FileSystem::FileClose(file);
		}
	}
}
