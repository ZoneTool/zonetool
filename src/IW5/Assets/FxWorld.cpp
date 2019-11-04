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
		FxWorld* IFxWorld::parse(const std::string& name, ZoneMemory* mem)
		{
			auto path = name + ".fxmap";
			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			// alloc FxWorld
			FxWorld* asset;

			ZONETOOL_INFO("Parsing FxMap \"%s\" ...", name.data());

			AssetReader read(mem);
			if (!read.open(path))
			{
				return nullptr;
			}

			asset = read.read_array<FxWorld>();
			asset->name = read.read_string();

			asset->glassSys.defs = read.read_array<FxGlassDef>();
			for (int i = 0; i < asset->glassSys.defCount; i++)
			{
				asset->glassSys.defs[i].material = read.read_asset<Material>();
				asset->glassSys.defs[i].materialShattered = read.read_asset<Material>();
				asset->glassSys.defs[i].physPreset = read.read_asset<PhysPreset>();
			}

			asset->glassSys.piecePlaces = read.read_array<FxGlassPiecePlace>();
			asset->glassSys.pieceStates = read.read_array<FxGlassPieceState>();
			asset->glassSys.pieceDynamics = read.read_array<FxGlassPieceDynamics>();
			asset->glassSys.geoData = read.read_array<FxGlassGeometryData>();
			asset->glassSys.isInUse = read.read_array<unsigned int>();
			asset->glassSys.cellBits = read.read_array<unsigned int>();
			asset->glassSys.visData = read.read_array<char>();
			asset->glassSys.linkOrg = read.read_array<VecInternal<3>>();
			asset->glassSys.halfThickness = read.read_array<float>();
			asset->glassSys.lightingHandles = read.read_array<unsigned __int16>();
			asset->glassSys.initPieceStates = read.read_array<FxGlassInitPieceState>();
			asset->glassSys.initGeoData = read.read_array<FxGlassGeometryData>();
			read.close();

			return asset;
		}

		IFxWorld::IFxWorld()
		{
		}

		IFxWorld::~IFxWorld()
		{
		}

		void IFxWorld::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data(), 1).fxworld;
			}
		}

		void IFxWorld::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IFxWorld::load_depending(IZone* zone)
		{
			auto data = this->asset_;
			if (data->glassSys.defs)
			{
				for (unsigned int i = 0; i < data->glassSys.defCount; i++)
				{
					if (data->glassSys.defs[i].physPreset)
					{
						zone->add_asset_of_type(physpreset, data->glassSys.defs[i].physPreset->name);
					}
					if (data->glassSys.defs[i].material)
					{
						zone->add_asset_of_type(material, data->glassSys.defs[i].material->name);
					}
					if (data->glassSys.defs[i].materialShattered)
					{
						zone->add_asset_of_type(material, data->glassSys.defs[i].materialShattered->name);
					}
				}
			}
		}

		std::string IFxWorld::name()
		{
			return this->name_;
		}

		std::int32_t IFxWorld::type()
		{
			return fx_map;
		}

		void IFxWorld::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->glassSys.defs)
			{
				buf->align(3);
				auto glass_def = buf->write(data->glassSys.defs, data->glassSys.defCount);

				for (std::uint32_t i = 0; i < data->glassSys.defCount; i++)
				{
					if (data->glassSys.defs[i].physPreset)
					{
						glass_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
							physpreset, data->glassSys.defs[i].physPreset->name));
					}
					if (data->glassSys.defs[i].material)
					{
						glass_def[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
							material, data->glassSys.defs[i].material->name));
					}
					if (data->glassSys.defs[i].materialShattered)
					{
						glass_def[i].materialShattered = reinterpret_cast<Material*>(zone->get_asset_pointer(
							material, data->glassSys.defs[i].materialShattered->name));
					}
				}

				ZoneBuffer::ClearPointer(&dest->glassSys.defs);
			}

			buf->push_stream(2);
			if (data->glassSys.piecePlaces)
			{
				buf->align(3);
				buf->write(data->glassSys.piecePlaces, data->glassSys.pieceLimit);
				ZoneBuffer::ClearPointer(&dest->glassSys.piecePlaces);
			}

			if (data->glassSys.pieceStates)
			{
				buf->align(3);
				buf->write(data->glassSys.pieceStates, data->glassSys.pieceLimit);
				ZoneBuffer::ClearPointer(&dest->glassSys.pieceStates);
			}

			if (data->glassSys.pieceDynamics)
			{
				buf->align(3);
				buf->write(data->glassSys.pieceDynamics, data->glassSys.pieceLimit);
				ZoneBuffer::ClearPointer(&dest->glassSys.pieceDynamics);
			}

			if (data->glassSys.geoData)
			{
				buf->align(3);
				buf->write(data->glassSys.geoData, data->glassSys.geoDataLimit);
				ZoneBuffer::ClearPointer(&dest->glassSys.geoData);
			}

			if (data->glassSys.isInUse)
			{
				buf->align(3);
				buf->write(data->glassSys.isInUse, data->glassSys.pieceWordCount);
				ZoneBuffer::ClearPointer(&dest->glassSys.isInUse);
			}

			if (data->glassSys.cellBits)
			{
				buf->align(3);
				buf->write(data->glassSys.cellBits, data->glassSys.pieceWordCount * data->glassSys.cellCount);
				ZoneBuffer::ClearPointer(&dest->glassSys.cellBits);
			}

			if (data->glassSys.visData)
			{
				buf->align(15);
				buf->write(data->glassSys.visData, (data->glassSys.pieceLimit + 15) & 0xFFFFFFF0);
				ZoneBuffer::ClearPointer(&dest->glassSys.visData);
			}

			if (data->glassSys.linkOrg)
			{
				buf->align(3);
				buf->write(data->glassSys.linkOrg, data->glassSys.pieceLimit);
				ZoneBuffer::ClearPointer(&dest->glassSys.linkOrg);
			}

			if (data->glassSys.halfThickness)
			{
				buf->align(15);
				buf->write(data->glassSys.halfThickness, (data->glassSys.pieceLimit + 3) & 0xFFFFFFFC);
				ZoneBuffer::ClearPointer(&dest->glassSys.halfThickness);
			}
			buf->pop_stream();

			if (data->glassSys.lightingHandles)
			{
				buf->align(1);
				buf->write(data->glassSys.lightingHandles, data->glassSys.initPieceCount);
				ZoneBuffer::ClearPointer(&dest->glassSys.lightingHandles);
			}

			if (data->glassSys.initPieceStates)
			{
				buf->align(3);
				buf->write(data->glassSys.initPieceStates, data->glassSys.initPieceCount);
				ZoneBuffer::ClearPointer(&dest->glassSys.initPieceStates);
			}

			if (data->glassSys.initGeoData)
			{
				buf->align(3);
				buf->write(data->glassSys.initGeoData, data->glassSys.initGeoDataCount);
				ZoneBuffer::ClearPointer(&dest->glassSys.initGeoData);
			}

			END_LOG_STREAM;
			buf->pop_stream();

			encrypt_data(dest, sizeof FxWorld);
		}

		void IFxWorld::dump(FxWorld* asset)
		{
			AssetDumper write;

			if (!write.open(asset->name + ".fxmap"s))
			{
				return;
			}

			write.dump_array(asset, 1);

			write.dump_string(asset->name);

			write.dump_array(asset->glassSys.defs, asset->glassSys.defCount);
			for (unsigned int i = 0; i < asset->glassSys.defCount; i++)
			{
				write.dump_asset(asset->glassSys.defs[i].material);
				write.dump_asset(asset->glassSys.defs[i].materialShattered);
				write.dump_asset(asset->glassSys.defs[i].physPreset);
			}

			write.dump_array(asset->glassSys.piecePlaces, asset->glassSys.pieceLimit);

			write.dump_array(asset->glassSys.pieceStates, asset->glassSys.pieceLimit);

			write.dump_array(asset->glassSys.pieceDynamics, asset->glassSys.pieceLimit);

			write.dump_array(asset->glassSys.geoData, asset->glassSys.geoDataLimit);

			write.dump_array(asset->glassSys.isInUse, asset->glassSys.pieceWordCount);

			write.dump_array(asset->glassSys.cellBits, asset->glassSys.pieceWordCount * asset->glassSys.cellCount);

			write.dump_array(asset->glassSys.visData, (asset->glassSys.pieceLimit + 15) & 0xFFFFFFF0);

			write.dump_array(asset->glassSys.linkOrg, asset->glassSys.pieceLimit);

			write.dump_array(asset->glassSys.halfThickness, (asset->glassSys.pieceLimit + 3) & 0xFFFFFFFC);

			write.dump_array(asset->glassSys.lightingHandles, asset->glassSys.initPieceCount);

			write.dump_array(asset->glassSys.initPieceStates, asset->glassSys.initPieceCount);

			write.dump_array(asset->glassSys.initGeoData, asset->glassSys.initGeoDataCount);

			write.close();
		}
	}
}
