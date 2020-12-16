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
		MapEnts* IMapEnts::parse(std::string name, ZoneMemory* mem)
		{
			// check if we can open a filepointer
			if (!FileSystem::FileExists(name + ".ents"))
			{
				return nullptr;
			}

			auto* file = FileSystem::FileOpen(name + ".ents", "rb");

			// let them know that we're parsing a custom mapents file
			ZONETOOL_INFO("Parsing mapents \"%s\"...", name.c_str());

			// alloc mapents
			auto ents = mem->Alloc<MapEnts>();

			ents->name = mem->StrDup(name);
			ents->numEntityChars = FileSystem::FileSize(file);

			ents->entityString = mem->Alloc<char>(ents->numEntityChars + 1);
			memset((char*)ents->entityString, 0, ents->numEntityChars);

			fread((char*)ents->entityString, ents->numEntityChars, 1, file);
			((char*)ents->entityString)[ents->numEntityChars] = '\0';

			// close filepointer
			FileSystem::FileClose(file);

			/*if (!FileSystem::FileExists(name + ".ents.triggers"))
			{
				return ents;
			}*/

			AssetReader trigger_reader(mem);
			if (trigger_reader.open(name + ".ents.triggers"))
			{
				ents->trigger.modelCount = trigger_reader.read_int();
				ents->trigger.models = trigger_reader.read_array<TriggerModel>();

				ents->trigger.hullCount = trigger_reader.read_int();
				ents->trigger.hulls = trigger_reader.read_array<TriggerHull>();

				ents->trigger.slabCount = trigger_reader.read_int();
				ents->trigger.slabs = trigger_reader.read_array<TriggerSlab>();

				trigger_reader.close();
			}

			// return mapents
			return ents;
		}

		void IMapEnts::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/"s + (currentzone.substr(0, 3) == "mp_" ? "mp/" : "") + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data(), 1).mapents;
			}
		}

		void IMapEnts::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IMapEnts::load_depending(IZone* zone)
		{
			load_depending_internal(zone, this->asset_->entityString);
		}

		void IMapEnts::load_depending_internal(IZone* zone, const char* entityString)
		{
			//auto expTreeOrg = Expression::ParseExpression(entityString, false);
			//auto expTree = expTreeOrg.get();

			//while (expTree)
			//{
			//	if (expTree->ToString() == "model" || expTree->ToString() == "1670")
			//	{
			//		// go to next expression
			//		expTree = expTree->Next();

			//		// model name
			//		auto modelName = expTree->ToString();

			//		if (modelName[0] != '*' && modelName[0] != '?')
			//		{
			//			ZONETOOL_INFO("Adding model \"%s\" because its referenced in the entities file.", modelName.data());
			//			zone->AddAssetOfType(XAssetType::xmodel, modelName);
			//		}
			//	}
			//	else if (expTree->ToString() == "weaponinfo" || expTree->ToString() == "1803")
			//	{
			//		// go to next expression
			//		expTree = expTree->Next();

			//		// model name
			//		auto weaponName = expTree->ToString();

			//		ZONETOOL_INFO("Adding weapon \"%s\" because its referenced in the entities file.", weaponName.data());
			//		zone->AddAssetOfType(XAssetType::weapon, weaponName);
			//	}
			//	else if (expTree->ToString() == "csv_include" || expTree->ToString() == "2817")
			//	{
			//		// go to next expression
			//		expTree = expTree->Next();

			//		// csv name
			//		auto csvName = expTree->ToString();

			//		ZONETOOL_INFO("Parsing sub-csv file \"%s\" because its referenced in the entities file.", csvName.data());

			//		// parse CSV...
			//		auto csv = std::make_unique<CSV>("radiant\\" + csvName + ".csv");
			//		for (auto i = 0; i < csv->Rows(); i++)
			//		{
			//			if (csv->Columns(i) >= 2)
			//			{
			//				auto assetType = csv->Get(i, 0);
			//				auto assetName = csv->Get(i, 1);

			//				ZONETOOL_INFO("[%s]: Adding sub-asset \"%s\"...", csvName.data(), assetName.data());

			//				auto type = zone->GetTypeByName(assetType);
			//				zone->AddAssetOfType(type, assetName);
			//			}
			//		}
			//	}

			//	expTree = expTree->Next();
			//}
		}

		std::string IMapEnts::name()
		{
			return this->name_;
		}

		std::int32_t IMapEnts::type()
		{
			return map_ents;
		}

		void IMapEnts::write_triggers(IZone* zone, ZoneBuffer* buf, MapTriggers* dest)
		{
			if (dest->models)
			{
				/*buf->align(3);
				buf->write(dest->models, dest->modelCount);
				ZoneBuffer::clear_pointer(&dest->models);*/
				dest->models = buf->write_s(3, dest->models, dest->modelCount);
			}
			
			if (dest->hulls)
			{
				/*buf->align(3);
				buf->write(dest->hulls, dest->hullCount);
				ZoneBuffer::clear_pointer(&dest->hulls);*/
				dest->hulls = buf->write_s(3, dest->hulls, dest->hullCount);
			}
			
			if (dest->slabs)
			{
				/*buf->align(3);
				buf->write(dest->slabs, dest->slabCount);
				ZoneBuffer::clear_pointer(&dest->slabs);*/
				dest->slabs = buf->write_s(3, dest->slabs, dest->slabCount);
			}
		}

		void IMapEnts::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->entityString)
			{
				buf->align(0);
				buf->write(data->entityString, data->numEntityChars);
				ZoneBuffer::clear_pointer(&dest->entityString);
			}

			write_triggers(zone, buf, &dest->trigger);
			write_triggers(zone, buf, &dest->clientTrigger.trigger);

			if (data->clientTrigger.clientTriggerAabbTree)
			{
				buf->align(3);
				buf->write(data->clientTrigger.clientTriggerAabbTree, data->clientTrigger.numClientTriggerNodes);
				ZoneBuffer::clear_pointer(&dest->clientTrigger.clientTriggerAabbTree);
			}
			if (data->clientTrigger.triggerString)
			{
				buf->align(0);
				buf->write(data->clientTrigger.triggerString, data->clientTrigger.triggerStringLength);
				ZoneBuffer::clear_pointer(&dest->clientTrigger.triggerString);
			}
			if (data->clientTrigger.visionSetTriggers)
			{
				buf->align(1);
				buf->write(data->clientTrigger.visionSetTriggers, data->clientTrigger.trigger.modelCount);
				ZoneBuffer::clear_pointer(&dest->clientTrigger.visionSetTriggers);
			}
			if (data->clientTrigger.triggerType)
			{
				buf->align(0);
				buf->write(data->clientTrigger.triggerType, data->clientTrigger.trigger.modelCount);
				ZoneBuffer::clear_pointer(&dest->clientTrigger.triggerType);
			}
			if (data->clientTrigger.origins)
			{
				buf->align(3);
				buf->write(data->clientTrigger.origins, 3 * data->clientTrigger.trigger.modelCount);
				ZoneBuffer::clear_pointer(&dest->clientTrigger.origins);
			}
			if (data->clientTrigger.scriptDelay)
			{
				buf->align(3);
				buf->write(data->clientTrigger.scriptDelay, data->clientTrigger.trigger.modelCount);
				ZoneBuffer::clear_pointer(&dest->clientTrigger.scriptDelay);
			}
			if (data->clientTrigger.audioTriggers)
			{
				buf->align(1);
				buf->write(data->clientTrigger.audioTriggers, data->clientTrigger.trigger.modelCount);
				ZoneBuffer::clear_pointer(&dest->clientTrigger.audioTriggers);
			}

			END_LOG_STREAM;
			buf->pop_stream();

			encrypt_data(dest, sizeof MapEnts);
		}

		void IMapEnts::dump(MapEnts* asset)
		{
			if (asset)
			{
				auto* file = FileSystem::FileOpen(asset->name + ".ents"s, "wb");
				if (file)
				{
					fwrite(asset->entityString, asset->numEntityChars, 1, file);
					FileSystem::FileClose(file);
				}

				AssetDumper trigger_dumper;
				if (trigger_dumper.open(asset->name + ".ents.triggers"s))
				{
					trigger_dumper.dump_int(asset->trigger.modelCount);
					trigger_dumper.dump_array<TriggerModel>(asset->trigger.models, asset->trigger.modelCount);

					trigger_dumper.dump_int(asset->trigger.hullCount);
					trigger_dumper.dump_array<TriggerHull>(asset->trigger.hulls, asset->trigger.hullCount);

					trigger_dumper.dump_int(asset->trigger.slabCount);
					trigger_dumper.dump_array<TriggerSlab>(asset->trigger.slabs, asset->trigger.slabCount);

					trigger_dumper.close();
				}
			}
		}
	}
}
