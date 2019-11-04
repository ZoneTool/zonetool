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
		/*legacy zonetool code, refactor me!*/
#define PARSE_STRING(entry) \
	if (!commapdata[#entry].is_null()) { \
		asset->entry = _strdup(commapdata[#entry].get<std::string>().c_str()); \
	} else { asset->entry = nullptr; }
#define PARSE_INT(entry) \
	asset->entry = commapdata[#entry].get<int>()
#define PARSE_FLOAT(entry) \
	asset->entry = commapdata[#entry].get<float>()
#define PARSE_FLOAT_ARRAY(entry,size) \
	for (int i = 0; i < size; i++) \
		asset->entry[i] = commapdata[#entry][i].get<float>();

		void PrimaryLight_Parse(ComPrimaryLight* asset, nlohmann::json commapdata)
		{
			PARSE_STRING(defName);

			PARSE_INT(type);
			PARSE_INT(canUseShadowMap);
			PARSE_INT(canUseShadowMap);

			PARSE_FLOAT_ARRAY(color, 3);
			PARSE_FLOAT_ARRAY(dir, 3);
			PARSE_FLOAT_ARRAY(origin, 3);

			PARSE_FLOAT(radius);
			PARSE_FLOAT(cosHalfFovOuter);
			PARSE_FLOAT(cosHalfFovInner);
			PARSE_FLOAT(cosHalfFovExpanded);
			PARSE_FLOAT(rotationLimit);
			PARSE_FLOAT(translationLimit);
		}

		ComWorld* IComWorld::parse(const std::string& name, ZoneMemory* mem)
		{
			auto path = name + ".comworld";
			if (FileSystem::FileExists(path))
			{
				ZONETOOL_INFO("Parsing commap \"%s\"...", name.c_str());

				auto asset = mem->Alloc<ComWorld>();

				auto file = FileSystem::FileOpen(path, "rb");
				auto size = FileSystem::FileSize(file);
				auto bytes = FileSystem::ReadBytes(file, size);
				FileSystem::FileClose(file);

				nlohmann::json commapdata = nlohmann::json::parse(bytes);

				PARSE_STRING(name);
				PARSE_INT(isInUse);
				PARSE_INT(primaryLightCount);

				asset->primaryLights = mem->Alloc<ComPrimaryLight>(asset->primaryLightCount);

				nlohmann::json primaryLights = commapdata["primaryLights"];
				for (int i = 0; i < asset->primaryLightCount; i++)
				{
					PrimaryLight_Parse(&asset->primaryLights[i], primaryLights[i]);
				}

				return asset;
			}

			return nullptr;
		}

		IComWorld::IComWorld()
		{
		}

		IComWorld::~IComWorld()
		{
		}

		void IComWorld::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data(), 1).comworld;
			}
		}

		void IComWorld::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IComWorld::load_depending(IZone* zone)
		{
			auto asset = this->asset_;

			for (int i = 0; i < asset->primaryLightCount; i++)
			{
				if (asset->primaryLights[i].defName)
				{
					zone->add_asset_of_type(lightdef, asset->primaryLights[i].defName);
				}
			}
		}

		std::string IComWorld::name()
		{
			return this->name_;
		}

		std::int32_t IComWorld::type()
		{
			return com_map;
		}

		void IComWorld::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);

			dest->name = buf->write_str(this->name());

			if (data->primaryLights)
			{
				buf->align(3);
				auto destlight = buf->write(data->primaryLights, data->primaryLightCount);

				for (std::uint32_t i = 0; i < data->primaryLightCount; i++)
				{
					if (data->primaryLights[i].defName)
					{
						destlight[i].defName = buf->write_str(data->primaryLights[i].defName);
					}
				}
			}

			buf->pop_stream();

			encrypt_data(dest, sizeof ComWorld);
		}

		void IComWorld::dump(ComWorld* asset, bool fromIW5)
		{
			Json& data = asset->ToJson(fromIW5);

			std::string path = asset->name + ".comworld"s;
			std::string json = data.dump(4);

			auto file = FileSystem::FileOpen(path, "w"s);
			fwrite(json.data(), json.size(), 1, file);
			FileSystem::FileClose(file);
		}
	}
}
