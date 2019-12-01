// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "ZoneUtils/Utils/BinaryDumper.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		AddonMapEnts* IAddonMapEnts::parse(std::string name, ZoneMemory* mem)
		{
			// check if we can open a filepointer
			if (!FileSystem::FileExists(name + ".addon.ents"))
			{
				return nullptr;
			}

			auto file = FileSystem::FileOpen(name + ".addon.ents", "rb");

			// let them know that we're parsing a custom mapents file
			ZONETOOL_INFO("Parsing addon mapents \"%s\"...", name.c_str());

			// alloc mapents
			auto ents = mem->Alloc<AddonMapEnts>();

			ents->name = mem->StrDup(name);
			ents->numEntityChars = FileSystem::FileSize(file);

			ents->entityString = mem->Alloc<char>(ents->numEntityChars + 1);
			memset((char*)ents->entityString, 0, ents->numEntityChars);

			fread((char*)ents->entityString, ents->numEntityChars, 1, file);
			((char*)ents->entityString)[ents->numEntityChars] = '\0';

			// convert the mapents!
			IMapEnts::ConvertEnts(reinterpret_cast<MapEnts*>(ents), mem);

			// close filepointer
			FileSystem::FileClose(file);

			AssetReader triggerReader(mem);
			AssetReader stageReader(mem);

			if (triggerReader.open(name + ".addon.ents.triggers"))
			{
				ents->trigger.modelCount = triggerReader.read_int();
				ents->trigger.models = triggerReader.read_array<TriggerModel>();

				ents->trigger.hullCount = triggerReader.read_int();
				ents->trigger.hulls = triggerReader.read_array<TriggerHull>();

				ents->trigger.slabCount = triggerReader.read_int();
				ents->trigger.slabs = triggerReader.read_array<TriggerSlab>();
			}

			triggerReader.close();

			// return mapents
			return ents;
		}

		void IAddonMapEnts::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data()).addon_map_ents;
			}
		}

		void IAddonMapEnts::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IAddonMapEnts::load_depending(IZone* zone)
		{
			
		}

		std::string IAddonMapEnts::name()
		{
			return this->name_;
		}

		std::int32_t IAddonMapEnts::type()
		{
			return addon_map_ents;
		}

		void IAddonMapEnts::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->entityString)
			{
				buf->align(0);
				buf->write(data->entityString, data->numEntityChars);
				ZoneBuffer::ClearPointer(&dest->entityString);
			}

			IMapEnts::write_triggers(zone, buf, &dest->trigger);

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IAddonMapEnts::dump(AddonMapEnts* asset)
		{
			auto file = FileSystem::FileOpen(asset->name + ".addon.ents"s, "wb");

			if (file)
			{
				fwrite(asset->entityString, asset->numEntityChars, 1, file);
				FileSystem::FileClose(file);
			}

			AssetDumper triggerDumper;
			if (triggerDumper.open(asset->name + ".addon.ents.triggers"s))
			{
				triggerDumper.dump_int(asset->trigger.modelCount);
				triggerDumper.dump_array<TriggerModel>(asset->trigger.models, asset->trigger.modelCount);

				triggerDumper.dump_int(asset->trigger.hullCount);
				triggerDumper.dump_array<TriggerHull>(asset->trigger.hulls, asset->trigger.hullCount);

				triggerDumper.dump_int(asset->trigger.slabCount);
				triggerDumper.dump_array<TriggerSlab>(asset->trigger.slabs, asset->trigger.slabCount);

				triggerDumper.close();
			}
		}
	}
}
