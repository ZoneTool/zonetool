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
		IFontDef::IFontDef()
		{
		}

		IFontDef::~IFontDef()
		{
		}

		void IFontDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name_.data(), 1).font;
			}
		}

		void IFontDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IFontDef::load_depending(IZone* zone)
		{
			auto* data = this->asset_;

			if (data->material)
			{
				zone->add_asset_of_type(material, data->material->name);
			}

			if (data->glowMaterial)
			{
				zone->add_asset_of_type(material, data->glowMaterial->name);
			}
		}

		std::string IFontDef::name()
		{
			return this->name_;
		}

		std::int32_t IFontDef::type()
		{
			return font;
		}

		void IFontDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->material)
			{
				dest->material = reinterpret_cast<Material*>(
					zone->get_asset_pointer(material, data->material->name)
				);
			}

			if (data->glowMaterial)
			{
				dest->glowMaterial = reinterpret_cast<Material*>(
					zone->get_asset_pointer(material, data->glowMaterial->name)
				);
			}

			if (data->glyphs)
			{
				buf->align(3);
				buf->write(data->glyphs, data->glyphCount);
				ZoneBuffer::clear_pointer(&dest->glyphs);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		Font_s* IFontDef::parse(const std::string& name, ZoneMemory* mem)
		{
			auto path = name;
			auto file = FileSystem::FileOpen(path, "rb"s);
			if (!file)
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing font \"%s\"...", name.c_str());

			auto size = FileSystem::FileSize(file);
			auto bytes = FileSystem::ReadBytes(file, size);
			FileSystem::FileClose(file);

			nlohmann::json fontdata = nlohmann::json::parse(bytes);

			auto font = mem->Alloc<Font_s>();
			font->name = strdup(fontdata["fontName"].get<std::string>().c_str());
			font->glyphCount = fontdata["glyphCount"].get<int>();
			font->pixelHeight = fontdata["pixelHeight"].get<int>();

			font->material = mem->Alloc<Material>();
			font->material->name = strdup(fontdata["material"].get<std::string>().c_str());
			font->glowMaterial = mem->Alloc<Material>();
			font->glowMaterial->name = strdup(fontdata["glowMaterial"].get<std::string>().c_str());

			font->glyphs = mem->Alloc<Glyph>(font->glyphCount);

			for (int i = 0; i < font->glyphCount; i++)
			{
				font->glyphs[i].letter = fontdata["glyphs"][i].get<unsigned __int16>();
				font->glyphs[i].x0 = fontdata["glyphs"][i].get<char>();
				font->glyphs[i].y0 = fontdata["glyphs"][i].get<char>();
				font->glyphs[i].dx = fontdata["glyphs"][i].get<char>();
				font->glyphs[i].pixelWidth = fontdata["glyphs"][i].get<char>();
				font->glyphs[i].pixelHeight = fontdata["glyphs"][i].get<char>();
				font->glyphs[i].s0, fontdata["glyphs"][i].get<float>();
				font->glyphs[i].t0, fontdata["glyphs"][i].get<float>();
				font->glyphs[i].s1, fontdata["glyphs"][i].get<float>();
				font->glyphs[i].t1, fontdata["glyphs"][i].get<float>();
			}

			return font;
		}

		void IFontDef::dump(Font_s* asset)
		{
			nlohmann::json fontdata;

			ZONETOOL_INFO("Dumping font %s", asset->name);

			fontdata["fontName"] = std::string(asset->name);
			fontdata["glyphCount"] = asset->glyphCount;
			fontdata["material"] = std::string(asset->material->name);
			fontdata["glowMaterial"] = std::string(asset->glowMaterial->name);
			fontdata["pixelHeight"] = asset->pixelHeight;

			nlohmann::json glyphs;
			for (int i = 0; i < asset->glyphCount; i++)
			{
				nlohmann::json glyph;

				glyph["letter"] = asset->glyphs[i].letter;
				glyph["x0"] = asset->glyphs[i].x0;
				glyph["y0"] = asset->glyphs[i].y0;
				glyph["dx"] = asset->glyphs[i].dx;
				glyph["pixelWidth"] = asset->glyphs[i].pixelWidth;
				glyph["pixelHeight"] = asset->glyphs[i].pixelHeight;
				glyph["s0"] = asset->glyphs[i].s0;
				glyph["t0"] = asset->glyphs[i].t0;
				glyph["s1"] = asset->glyphs[i].s1;
				glyph["t1"] = asset->glyphs[i].t1;

				glyphs.push_back(glyph);
			}

			fontdata["glyphs"] = glyphs;

			std::string assetstr = fontdata.dump(4);

			auto assetPath = asset->name;

			auto fileAsset = FileSystem::FileOpen(assetPath, "wb");

			if (fileAsset)
			{
				fwrite(assetstr.c_str(), assetstr.size(), 1, fileAsset);
				FileSystem::FileClose(fileAsset);
			}
		}
	}
}
