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
		void* Zone::get_asset_pointer(std::int32_t type, const std::string& name)
		{
			if (name.empty())
			{
				return nullptr;	
			}
			
			for (std::size_t idx = 0; idx < m_assets.size(); idx++)
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
			DECLARE_ASSET(glass_map, IGlassWorld);
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
			DECLARE_ASSET(col_map_mp, IClipMap);
			DECLARE_ASSET(gfx_map, IGfxWorld);
			DECLARE_ASSET(rawfile, IRawFile);
			DECLARE_ASSET(com_map, IComWorld);
			DECLARE_ASSET(font, IFontDef);
			DECLARE_ASSET(leaderboarddef, ILeaderBoardDef);
			DECLARE_ASSET(localize, ILocalizeEntry);
			DECLARE_ASSET(attachment, IAttachmentDef);
			DECLARE_ASSET(physpreset, IPhysPreset);
			DECLARE_ASSET(phys_collmap, IPhysCollmap);
			DECLARE_ASSET(fx, IFxEffectDef);
			DECLARE_ASSET(stringtable, IStringTable);
			DECLARE_ASSET(sound, ISound);
			DECLARE_ASSET(loaded_sound, ILoadedSound);
			DECLARE_ASSET(sndcurve, ISoundCurve);
			DECLARE_ASSET(glass_map, IGlassWorld);
			DECLARE_ASSET(fx_map, IFxWorld);
			DECLARE_ASSET(weapon, IWeaponDef);
			DECLARE_ASSET(structureddatadef, IStructuredDataDef);
			DECLARE_ASSET(menu, IMenuDef);
			DECLARE_ASSET(scriptfile, IScriptFile);
			DECLARE_ASSET(lightdef, ILightDef);
			DECLARE_ASSET(font, IFontDef);
		}

		std::int32_t Zone::get_type_by_name(const std::string& type)
		{
			return m_linker->type_to_int(type);
		}

		void Zone::add_asset_of_type(const std::string& type, const std::string& name)
		{
			std::int32_t itype = m_linker->type_to_int(type);
			this->add_asset_of_type(itype, name);
		}

		void Zone::build(ZoneBuffer* buf)
		{
#ifdef USE_VMPROTECT
			VMProtectBeginUltra("IW5::Zone::Build");
#endif

			auto startTime = GetTickCount64();

			// make a folder in main, for the map images
			std::filesystem::create_directories("main\\" + this->name_ + "\\images");

			ZONETOOL_INFO("Compiling fastfile \"%s\"...", this->name_.data());

			constexpr std::size_t num_streams = 9;
			XZoneMemory<num_streams> mem;

			std::size_t headersize = sizeof XZoneMemory<num_streams>;
			memset(&mem, 0, headersize);

			auto zone = buf->at<XZoneMemory<num_streams>>();

			// write zone header
			buf->write(&mem);

			std::uintptr_t pad = 0xFFFFFFFF;
			std::uintptr_t zero = 0;

			// write asset types to header
			for (auto i = 0u; i < m_assets.size(); i++)
			{
				m_assets[i]->prepare(buf, this->m_zonemem.get());
			}

			// write scriptstring count
			std::uint32_t stringcount = buf->scriptstring_count();
			buf->write<std::uint32_t>(&stringcount);
			buf->write<std::uintptr_t>(stringcount > 0 ? (&pad) : (&zero));

			// write asset count
			std::uint32_t asset_count = m_assets.size();
			buf->write<std::uint32_t>(&asset_count);
			buf->write<std::uintptr_t>(asset_count > 0 ? (&pad) : (&zero));

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
			ZONETOOL_INFO("m_assetbase: %u", this->m_assetbase);

			// write asset types to header
			for (auto i = 0u; i < asset_count; i++)
			{
				// write asset data to zone
				auto type = m_assets[i]->type();
				buf->write(&type);
				buf->write(&pad);
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
			zone->size = buf->size() - headersize;
			zone->externalsize = 0;

			// Update stream data
			for (int i = 0; i < num_streams; i++)
			{
				zone->streams[i] = buf->stream_offset(i);
			}

			// Dump zone to disk (for debugging)
			buf->save("debug\\" + this->name_ + ".zone");

			// Compress buffer
			auto buf_compressed = buf->compress_zstd();

			// Generate FF header
			auto header = this->m_zonemem->Alloc<XFileHeader>();
			strcpy(header->header, "IWffu100");
			header->version = 2000;
			header->allowOnlineUpdate = 0;

			// Save fastfile
			ZoneBuffer fastfile(buf_compressed.size() + 21);
			fastfile.init_streams(1);
			fastfile.write_stream(header, 21);

			fastfile.write(buf_compressed.data(), buf_compressed.size());

			std::string localappdata = getenv("LOCALAPPDATA");
			fastfile.save(localappdata + "\\Plutonium-staging\\storage\\iw5\\zone\\" + this->name_ + ".ff");

			// oxygen output paths
			// fastfile.save("C:\\Users\\RektInator\\AppData\\Local\\Plutonium\\storage\\iw5\\zone\\" + this->name_ + ".ff");
			fastfile.save("zone\\english\\" + this->name_ + ".ff");

			ZONETOOL_INFO("Successfully compiled fastfile \"%s\"!", this->name_.data());
			ZONETOOL_INFO("Compiling took %u msec.", GetTickCount64() - startTime);

			// this->m_linker->UnloadZones();

#ifdef USE_VMPROTECT
			VMProtectEnd();
#endif
		}

		Zone::Zone(std::string name, ILinker* linker)
		{
			currentzone = name;

			this->name_ = name;
			this->m_linker = linker;

			this->m_zonemem = std::make_shared<ZoneMemory>(MAX_ZONE_SIZE);
		}

		Zone::~Zone()
		{
			// wipe all assets
			m_assets.clear();
		}
	}
}
