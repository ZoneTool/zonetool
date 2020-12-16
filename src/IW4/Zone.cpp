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
	namespace IW4
	{
		IAsset* Zone::find_asset(std::int32_t type, const std::string& name)
		{
			for (auto idx = 0u; idx < m_assets.size(); idx++)
			{
				if (m_assets[idx]->type() == type && m_assets[idx]->name() == name)
				{
					return m_assets[idx].get();
				}
			}

			return nullptr;
		}

		void* Zone::get_asset_pointer(std::int32_t type, const std::string& name)
		{
			if (name.empty())
			{
				return nullptr;
			}
			
			for (auto idx = 0u; idx < m_assets.size(); idx++)
			{
				if (m_assets[idx]->type() == type && m_assets[idx]->name() == name)
				{
					auto ptr = reinterpret_cast<void*>((3 << 28) | ((this->m_assetbase + (8 * idx) + 4) & 0x0FFFFFFF) + 1);
					return ptr;
				}
			}

			return nullptr;
		}

		void Zone::add_asset_of_type_by_pointer(std::int32_t type, void* pointer)
		{
			if (!pointer)
			{
				return;
			}
			
			// don't add asset if it already exists
			for (std::size_t idx = 0; idx < m_assets.size(); idx++)
			{
				if (m_assets[idx]->type() == type && m_assets[idx]->pointer() == pointer)
				{
					return;
				}
			}

#define DECLARE_ASSET(__type__, __interface__) \
			if (type == __type__) \
			{ \
				auto asset = std::make_shared < __interface__ >(); \
				asset->init(pointer, this->m_zonemem.get()); \
				asset->load_depending(this); \
				m_assets.push_back(asset); \
			}

			// declare asset interfaces
			DECLARE_ASSET(xmodelsurfs, IXSurface);
			DECLARE_ASSET(image, IGfxImage);
			DECLARE_ASSET(localize, ILocalizeEntry);
		}

		void Zone::add_asset_of_type(std::int32_t type, const std::string& name)
		{
			if (name.empty())
			{
				return;
			}
			
			// don't add asset if it already exists
			if (get_asset_pointer(type, name))
			{
				return;
			}

#define DECLARE_ASSET(__type__, __interface__) \
			if (type == __type__) \
			{ \
				auto asset = std::make_shared < __interface__ >(); \
				asset->init(name, this->m_zonemem.get()); \
				asset->load_depending(this); \
				m_assets.push_back(asset); \
			}

			// declare asset interfaces
			DECLARE_ASSET(xanim, IXAnimParts);
			DECLARE_ASSET(pixelshader, IPixelShader);
			DECLARE_ASSET(vertexdecl, IVertexDecl);
			DECLARE_ASSET(vertexshader, IVertexShader);
			DECLARE_ASSET(techset, ITechset);
			DECLARE_ASSET(image, IGfxImage);
			DECLARE_ASSET(material, IMaterial)
			DECLARE_ASSET(xmodelsurfs, IXSurface);
			DECLARE_ASSET(xmodel, IXModel);
			DECLARE_ASSET(map_ents, IMapEnts);
			DECLARE_ASSET(rawfile, IRawFile);
			DECLARE_ASSET(com_map, IComWorld);
			DECLARE_ASSET(font, IFontDef);
			DECLARE_ASSET(localize, ILocalizeEntry);
			DECLARE_ASSET(physpreset, IPhysPreset);
			DECLARE_ASSET(phys_collmap, IPhysCollmap);
			DECLARE_ASSET(stringtable, IStringTable);
			DECLARE_ASSET(sound, ISound);
			DECLARE_ASSET(loaded_sound, ILoadedSound);
			DECLARE_ASSET(sndcurve, ISoundCurve);
			DECLARE_ASSET(game_map_mp, IGameWorldMp);
			DECLARE_ASSET(game_map_sp, IGameWorldSp);
			DECLARE_ASSET(fx_map, IFxWorld);
			DECLARE_ASSET(tracer, ITracerDef);
			DECLARE_ASSET(gfx_map, IGfxWorld);
			DECLARE_ASSET(col_map_mp, IClipMap);
			DECLARE_ASSET(fx, IFxEffectDef);
			DECLARE_ASSET(lightdef, ILightDef);
			DECLARE_ASSET(structureddatadef, IStructuredDataDef);
			DECLARE_ASSET(addon_map_ents, IAddonMapEnts);
		}

		void Zone::add_asset_of_type(const std::string& type, const std::string& name)
		{
			std::int32_t itype = m_linker->type_to_int(type);
			this->add_asset_of_type(itype, name);
		}

		std::int32_t Zone::get_type_by_name(const std::string& type)
		{
			return m_linker->type_to_int(type);
		}

		void Zone::build(ZoneBuffer* buf)
		{
			// init streams properly
			buf->init_streams(/*target_ == zone_target::pc ? 8 : 6*/8);							// use 8 because it always uses a shiftsize of 28, even though console is missing 1-2 streams.
			
			if (target_ != zone_target::pc)
			{
				auto pak_version = 269;
				if (target_version_ == zone_target_version::iw4_alpha_482)
				{
					pak_version = 253;
				}
				
				buf->alloc_image_pak(pak_version);
			}
			
			const auto start_time = GetTickCount64();

			// make a folder in main, for the map images
			std::filesystem::create_directories("main\\" + this->name_ + "\\images");

			ZONETOOL_INFO("Compiling fastfile \"%s\"...", this->name_.data());

			constexpr std::size_t num_streams = 8;
			XZoneMemory<num_streams> mem = {};

			const auto header_size = sizeof XZoneMemory<num_streams>;
			memset(&mem, 0, header_size);

			auto zone = buf->at<XZoneMemory<num_streams>>();

			// write zone header
			auto mem_ptr = buf->at<XZoneMemory<num_streams>>();

			buf->write_stream(&mem, 4, target_ == zone_target::pc ? 10 : target_ == zone_target::ps3 ? 9 : 8);			// write correct amount of streams, skip last 2 streams for console
			
			std::uintptr_t pad = 0xFFFFFFFF;
			std::uintptr_t zero = 0;

			// write asset types to header
			for (auto i = 0u; i < m_assets.size(); i++)
			{
				m_assets[i]->prepare(buf, this->m_zonemem.get());
			}

			// write scriptstring count
			auto stringcount = buf->scriptstring_count();
			const auto stringcount_ptr = buf->write<std::uint32_t>(&stringcount);
			buf->write<std::uintptr_t>(stringcount > 0 ? (&pad) : (&zero));

			// write asset count
			auto asset_count = m_assets.size();
			const auto asset_count_ptr = buf->write<std::uint32_t>(&asset_count);
			buf->write<std::uintptr_t>(asset_count > 0 ? (&pad) : (&zero));

			if (target_ != zone_target::pc)
			{
				endian_convert(asset_count_ptr);
				endian_convert(stringcount_ptr);
			}
			
			// push stream
			buf->push_stream(3);
			START_LOG_STREAM;

			if (stringcount)
			{
				// write pointer for every scriptstring
				for (std::size_t idx = 0; idx < stringcount; idx++)
				{
					buf->write<std::uintptr_t>(&pad);
				}

				// write scriptstrings
				buf->align(3);
				for (std::size_t idx = 0; idx < stringcount; idx++)
				{
					buf->write_str(buf->get_scriptstring(idx));
				}
			}

			buf->pop_stream();
			buf->push_stream(3);

			// align buffer
			buf->align(3);

			// set asset ptr base
			this->m_assetbase = buf->stream_offset(3);

			// write asset types to header
			for (auto i = 0u; i < asset_count; i++)
			{
				// write asset data to zone
				auto type = m_assets[i]->type();
				const auto type_ptr = buf->write(&type);
				buf->write(&pad);

				if (target_ != zone_target::pc)
				{
					if (*type_ptr >= 7 && target_ == zone_target::xbox360)
					{
						*type_ptr -= 2;
					}
					else if (*type_ptr >= 8 && target_ == zone_target::ps3)
					{
						*type_ptr -= 1;
					}
					
					endian_convert(type_ptr);
				}
			}

			// write assets
			for (auto& asset : m_assets)
			{
				// push stream
				buf->push_stream(0);
				buf->align(3);

				// write asset
				asset->write(this, buf);
				
				// pop stream
				buf->pop_stream();
			}

			// pop stream
			END_LOG_STREAM;
			buf->pop_stream();

			// update zone header
			zone->size = buf->size() - (target_ == zone_target::pc ? 0x28 : target_ == zone_target::ps3 ? 0x24 : 0x20);
			zone->externalsize = 0;

			// Update stream data
			for (int i = 0; i < (target_ == zone_target::pc ? 8 : target_ == zone_target::ps3 ? 7 : 6); i++)
			{
				zone->streams[i] = buf->stream_offset(i);
			}

			auto streamed_files = buf->stream_files();
			
			if (target_ != zone_target::pc)
			{
				// mem_ptr->streams[5] = 1352;
				
				endian_convert(&mem_ptr->size);
				endian_convert(&mem_ptr->externalsize);
				for (auto& stream : mem_ptr->streams)
				{
					endian_convert(&stream);
				}
			}

			if (this->get_target() == zone_target::ps3)
			{
				auto zoneSize = ((buf->size() + (0x10000 - 1)) & ~(0x10000 - 1));
				std::vector<uint8_t> pad(zoneSize - buf->size(), 0);
				buf->write(pad.data(), zoneSize - buf->size());
			}

			// Dump zone to disk (DEBUGGING PURPOSES)
			buf->save("debug\\" + this->name_ + ".zone");

			// Compress buffer
			auto buf_compressed = buf->compress_zlib(this->get_target() == zone_target::ps3 ? true : false);

			// Generate FF header
			auto header = this->m_zonemem->Alloc<XFileHeader>();
			strcpy(header->header, "IWffu100");
			header->version = 276;
			header->allowOnlineUpdate = 0;

			if (target_ == zone_target::xbox360 || target_ == zone_target::ps3)
			{
				auto pak_version = 269;
				if (target_version_ == zone_target_version::iw4_alpha_482)
				{
					pak_version = 253;
				}

				header->version = pak_version;
				endian_convert(&header->version);
			}

			// fix entry count
			if (streamed_files.size())
			{
				XAssetStreamFile meme = {};
				memset(&meme, 0, sizeof XAssetStreamFile);

				const auto needed_entries = 4 - (streamed_files.size() % 4);
				for (auto i = 0u; i < needed_entries; i++)
				{
					streamed_files.push_back(meme);
				}
			}
			
			// Save fastfile
			ZoneBuffer fastfile(buf_compressed.size() + ((target_ == zone_target::xbox360 || target_ == zone_target::ps3) ? 37 : 21) + (streamed_files.size() * 12));
			fastfile.init_streams(1);
			fastfile.write_stream(header, 21);

			if (target_ == zone_target::xbox360 || target_ == zone_target::ps3)
			{				
				auto language = 1;
				auto entry_count = streamed_files.size();
				
				endian_convert(&language);
				endian_convert(&entry_count);
				
				fastfile.write_stream(&language, 4);
				fastfile.write_stream(&entry_count, 4);

				// write streamed files
				for (auto& stream_file : streamed_files)
				{
					auto file = fastfile.at<XAssetStreamFile>();
					fastfile.write_stream(&stream_file, sizeof XAssetStreamFile, 1);

					endian_convert(&file->fileIndex);
					endian_convert(&file->offset);
					endian_convert(&file->offsetEnd);
				}
				
				fastfile.write_stream(&mem_ptr->size, 4);
				fastfile.write_stream(&mem_ptr->size, 4);
			}

			fastfile.write(buf_compressed.data(), buf_compressed.size());

			if (target_version_ == zone_target_version::iw4_alpha_482)
			{
				fastfile.save("zone\\english\\" + this->name_ + ".ffm");
				buf->save_image_pak("imagefile5.pakm");
			}
			else
			{
				fastfile.save("zone\\english\\" + this->name_ + ".ff");
			}

			ZONETOOL_INFO("Successfully compiled fastfile \"%s\"!", this->name_.data());
			ZONETOOL_INFO("Compiling took %u msec.", GetTickCount64() - start_time);

			// this->m_linker->UnloadZones();
		}

		Zone::Zone(std::string name, ILinker* linker)
		{
			currentzone = name;

			this->m_linker = linker;
			this->name_ = name;

			this->m_zonemem = std::make_shared<ZoneMemory>(MAX_ZONE_SIZE);
		}

		Zone::~Zone()
		{
			// wipe all assets
			m_assets.clear();
		}
	}
}
