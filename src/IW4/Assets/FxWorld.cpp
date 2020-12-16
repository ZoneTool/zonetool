// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW5/Assets/FxWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		FxWorld* IFxWorld::parse(const std::string& name, ZoneMemory* mem)
		{
			auto* iw5_fxworld = IW5::IFxWorld::parse(name, mem);

			if (!iw5_fxworld)
			{
				return nullptr;
			}

			// asset is actually the same so just cast
			return reinterpret_cast<FxWorld*>(iw5_fxworld);
		}

		void IFxWorld::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/"s + (currentzone.substr(0, 3) == "mp_" ? "mp/" : "") + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data()).fxworld;
			}
		}

		void IFxWorld::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IFxWorld::load_depending(IZone* zone)
		{
			auto* data = this->asset_;
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
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->glassSys.defs)
			{
				buf->align(3);
				auto* glass_def = buf->write(data->glassSys.defs, data->glassSys.defCount);

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

				ZoneBuffer::clear_pointer(&dest->glassSys.defs);
			}

			buf->push_stream(2);
			if (data->glassSys.piecePlaces)
			{
				buf->align(3);
				buf->write(data->glassSys.piecePlaces, data->glassSys.pieceLimit);
				ZoneBuffer::clear_pointer(&dest->glassSys.piecePlaces);
			}

			if (data->glassSys.pieceStates)
			{
				buf->align(3);
				buf->write(data->glassSys.pieceStates, data->glassSys.pieceLimit);
				ZoneBuffer::clear_pointer(&dest->glassSys.pieceStates);
			}

			if (data->glassSys.pieceDynamics)
			{
				buf->align(3);
				buf->write(data->glassSys.pieceDynamics, data->glassSys.pieceLimit);
				ZoneBuffer::clear_pointer(&dest->glassSys.pieceDynamics);
			}

			if (data->glassSys.geoData)
			{
				buf->align(3);
				buf->write(data->glassSys.geoData, data->glassSys.geoDataLimit);
				ZoneBuffer::clear_pointer(&dest->glassSys.geoData);
			}

			if (data->glassSys.isInUse)
			{
				buf->align(3);
				buf->write(data->glassSys.isInUse, data->glassSys.pieceWordCount);
				ZoneBuffer::clear_pointer(&dest->glassSys.isInUse);
			}

			if (data->glassSys.cellBits)
			{
				buf->align(3);
				buf->write(data->glassSys.cellBits, data->glassSys.pieceWordCount * data->glassSys.cellCount);
				ZoneBuffer::clear_pointer(&dest->glassSys.cellBits);
			}

			if (data->glassSys.visData)
			{
				buf->align(15);
				buf->write(data->glassSys.visData, (data->glassSys.pieceLimit + 15) & 0xFFFFFFF0);
				ZoneBuffer::clear_pointer(&dest->glassSys.visData);
			}

			if (data->glassSys.linkOrg)
			{
				buf->align(3);
				buf->write(data->glassSys.linkOrg, data->glassSys.pieceLimit);
				ZoneBuffer::clear_pointer(&dest->glassSys.linkOrg);
			}

			if (data->glassSys.halfThickness)
			{
				buf->align(15);
				buf->write(data->glassSys.halfThickness, (data->glassSys.pieceLimit + 3) & 0xFFFFFFFC);
				ZoneBuffer::clear_pointer(&dest->glassSys.halfThickness);
			}
			buf->pop_stream();

			if (data->glassSys.lightingHandles)
			{
				buf->align(1);
				buf->write(data->glassSys.lightingHandles, data->glassSys.initPieceCount);
				ZoneBuffer::clear_pointer(&dest->glassSys.lightingHandles);
			}

			if (data->glassSys.initPieceStates)
			{
				buf->align(3);
				buf->write(data->glassSys.initPieceStates, data->glassSys.initPieceCount);
				ZoneBuffer::clear_pointer(&dest->glassSys.initPieceStates);
			}

			if (data->glassSys.initGeoData)
			{
				buf->align(3);
				buf->write(data->glassSys.initGeoData, data->glassSys.initGeoDataCount);
				ZoneBuffer::clear_pointer(&dest->glassSys.initGeoData);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IFxWorld::dump(FxWorld* asset)
		{
			IW5::IFxWorld::dump((IW5::FxWorld*)asset);
		}
	}
}
