#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		FxWorld* IFxWorld::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
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
			if (!read.Open(path))
			{
				return nullptr;
			}

			asset = read.Array<FxWorld>();
			asset->name = read.String();

			asset->glassSys.defs = read.Array<FxGlassDef>();
			for (int i = 0; i < asset->glassSys.defCount; i++)
			{
				asset->glassSys.defs[i].material = read.Asset<Material>();
				asset->glassSys.defs[i].materialShattered = read.Asset<Material>();
				asset->glassSys.defs[i].physPreset = read.Asset<PhysPreset>();
			}

			asset->glassSys.piecePlaces = read.Array<FxGlassPiecePlace>();
			asset->glassSys.pieceStates = read.Array<FxGlassPieceState>();
			asset->glassSys.pieceDynamics = read.Array<FxGlassPieceDynamics>();
			asset->glassSys.geoData = read.Array<FxGlassGeometryData>();
			asset->glassSys.isInUse = read.Array<unsigned int>();
			asset->glassSys.cellBits = read.Array<unsigned int>();
			asset->glassSys.visData = read.Array<char>();
			asset->glassSys.linkOrg = read.Array<VecInternal<3>>();
			asset->glassSys.halfThickness = read.Array<float>();
			asset->glassSys.lightingHandles = read.Array<unsigned __int16>();
			asset->glassSys.initPieceStates = read.Array<FxGlassInitPieceState>();
			asset->glassSys.initGeoData = read.Array<FxGlassGeometryData>();
			read.Close();

			return asset;
		}

		IFxWorld::IFxWorld()
		{
		}

		IFxWorld::~IFxWorld()
		{
		}

		void IFxWorld::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), name.data(), 1).fxworld;
			}
		}

		void IFxWorld::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IFxWorld::load_depending(IZone* zone)
		{
			auto data = this->m_asset;
			if (data->glassSys.defs)
			{
				for (unsigned int i = 0; i < data->glassSys.defCount; i++)
				{
					if (data->glassSys.defs[i].physPreset)
					{
						zone->AddAssetOfType(physpreset, data->glassSys.defs[i].physPreset->name);
					}
					if (data->glassSys.defs[i].material)
					{
						zone->AddAssetOfType(material, data->glassSys.defs[i].material->name);
					}
					if (data->glassSys.defs[i].materialShattered)
					{
						zone->AddAssetOfType(material, data->glassSys.defs[i].materialShattered->name);
					}
				}
			}
		}

		std::string IFxWorld::name()
		{
			return this->m_name;
		}

		std::int32_t IFxWorld::type()
		{
			return fx_map;
		}

		void IFxWorld::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
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
						glass_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->GetAssetPointer(
							physpreset, data->glassSys.defs[i].physPreset->name));
					}
					if (data->glassSys.defs[i].material)
					{
						glass_def[i].material = reinterpret_cast<Material*>(zone->GetAssetPointer(
							material, data->glassSys.defs[i].material->name));
					}
					if (data->glassSys.defs[i].materialShattered)
					{
						glass_def[i].materialShattered = reinterpret_cast<Material*>(zone->GetAssetPointer(
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

			if (!write.Open(asset->name + ".fxmap"s))
			{
				return;
			}

			write.Array(asset, 1);

			write.String(asset->name);

			write.Array(asset->glassSys.defs, asset->glassSys.defCount);
			for (unsigned int i = 0; i < asset->glassSys.defCount; i++)
			{
				write.Asset(asset->glassSys.defs[i].material);
				write.Asset(asset->glassSys.defs[i].materialShattered);
				write.Asset(asset->glassSys.defs[i].physPreset);
			}

			write.Array(asset->glassSys.piecePlaces, asset->glassSys.pieceLimit);

			write.Array(asset->glassSys.pieceStates, asset->glassSys.pieceLimit);

			write.Array(asset->glassSys.pieceDynamics, asset->glassSys.pieceLimit);

			write.Array(asset->glassSys.geoData, asset->glassSys.geoDataLimit);

			write.Array(asset->glassSys.isInUse, asset->glassSys.pieceWordCount);

			write.Array(asset->glassSys.cellBits, asset->glassSys.pieceWordCount * asset->glassSys.cellCount);

			write.Array(asset->glassSys.visData, (asset->glassSys.pieceLimit + 15) & 0xFFFFFFF0);

			write.Array(asset->glassSys.linkOrg, asset->glassSys.pieceLimit);

			write.Array(asset->glassSys.halfThickness, (asset->glassSys.pieceLimit + 3) & 0xFFFFFFFC);

			write.Array(asset->glassSys.lightingHandles, asset->glassSys.initPieceCount);

			write.Array(asset->glassSys.initPieceStates, asset->glassSys.initPieceCount);

			write.Array(asset->glassSys.initGeoData, asset->glassSys.initGeoDataCount);

			write.Close();
		}
	}
}
