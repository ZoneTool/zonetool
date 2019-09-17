#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		char* freadstr(FILE* file);
		int freadint(FILE* file);

		/*legacy zonetool code, refactor me!*/
		MapEnts* IMapEnts::parse(std::string name, std::shared_ptr<ZoneMemory>& mem)
		{
			// check if we can open a filepointer
			if (!FileSystem::FileExists(name + ".ents"))
			{
				return nullptr;
			}

			auto file = FileSystem::FileOpen(name + ".ents", "rb");

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

			AssetReader triggerReader(mem);
			if (triggerReader.Open(name + ".ents.triggers"))
			{
				ents->trigger.modelCount = triggerReader.Int();
				ents->trigger.models = triggerReader.Array<TriggerModel>();

				ents->trigger.hullCount = triggerReader.Int();
				ents->trigger.hulls = triggerReader.Array<TriggerHull>();

				ents->trigger.slabCount = triggerReader.Int();
				ents->trigger.slabs = triggerReader.Array<TriggerSlab>();

				triggerReader.Close();
			}

			//file = FileSystem::FileOpen(name + ".ents.triggers", "rb");

			//ents->trigger.hullCount = freadint(file);
			//ents->trigger.modelCount = freadint(file);
			//ents->trigger.slabCount = freadint(file);

			//// parse trigger data
			//if (ents->trigger.hullCount)
			//{
			//	ents->trigger.hulls = mem->Alloc<TriggerHull>(ents->trigger.hullCount);
			//	fread(ents->trigger.hulls, sizeof(TriggerHull), ents->trigger.hullCount, file);
			//}
			//if (ents->trigger.modelCount)
			//{
			//	ents->trigger.models = mem->Alloc<TriggerModel>(ents->trigger.modelCount);
			//	fread(ents->trigger.models, sizeof(TriggerModel), ents->trigger.modelCount, file);
			//}
			//if (ents->trigger.slabCount)
			//{
			//	ents->trigger.slabs = mem->Alloc<TriggerSlab>(ents->trigger.slabCount);
			//	fread(ents->trigger.slabs, sizeof(TriggerSlab), ents->trigger.slabCount, file);
			//}
			//FileSystem::FileClose(file);

			// return mapents
			return ents;
		}

		IMapEnts::IMapEnts()
		{
		}

		IMapEnts::~IMapEnts()
		{
		}

		void IMapEnts::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = "maps/mp/" + currentzone + ".d3dbsp"; // name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), name.data(), 1).mapents;
			}
		}

		void IMapEnts::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IMapEnts::load_depending(IZone* zone)
		{
			load_depending_internal(zone, this->m_asset->entityString);
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
			return this->m_name;
		}

		std::int32_t IMapEnts::type()
		{
			return map_ents;
		}

		void IMapEnts::write_triggers(IZone* zone, std::shared_ptr<ZoneBuffer>& buf, MapTriggers* dest)
		{
			if (dest->models)
			{
				/*buf->align(3);
				buf->write(dest->models, dest->modelCount);
				ZoneBuffer::ClearPointer(&dest->models);*/
				dest->models = buf->write_s(3, dest->models, dest->modelCount);
			}
			if (dest->hulls)
			{
				/*buf->align(3);
				buf->write(dest->hulls, dest->hullCount);
				ZoneBuffer::ClearPointer(&dest->hulls);*/
				dest->hulls = buf->write_s(3, dest->hulls, dest->hullCount);
			}
			if (dest->slabs)
			{
				/*buf->align(3);
				buf->write(dest->slabs, dest->slabCount);
				ZoneBuffer::ClearPointer(&dest->slabs);*/
				dest->slabs = buf->write_s(3, dest->slabs, dest->slabCount);
			}
		}

		void IMapEnts::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
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

			write_triggers(zone, buf, &dest->trigger);
			write_triggers(zone, buf, &dest->clientTrigger.trigger);

			if (data->clientTrigger.clientTriggerAabbTree)
			{
				buf->align(3);
				buf->write(data->clientTrigger.clientTriggerAabbTree, data->clientTrigger.numClientTriggerNodes);
				ZoneBuffer::ClearPointer(&dest->clientTrigger.clientTriggerAabbTree);
			}
			if (data->clientTrigger.triggerString)
			{
				buf->align(0);
				buf->write(data->clientTrigger.triggerString, data->clientTrigger.triggerStringLength);
				ZoneBuffer::ClearPointer(&dest->clientTrigger.triggerString);
			}
			if (data->clientTrigger.visionSetTriggers)
			{
				buf->align(1);
				buf->write(data->clientTrigger.visionSetTriggers, data->clientTrigger.trigger.modelCount);
				ZoneBuffer::ClearPointer(&dest->clientTrigger.visionSetTriggers);
			}
			if (data->clientTrigger.triggerType)
			{
				buf->align(0);
				buf->write(data->clientTrigger.triggerType, data->clientTrigger.trigger.modelCount);
				ZoneBuffer::ClearPointer(&dest->clientTrigger.triggerType);
			}
			if (data->clientTrigger.origins)
			{
				buf->align(3);
				buf->write(data->clientTrigger.origins, 3 * data->clientTrigger.trigger.modelCount);
				ZoneBuffer::ClearPointer(&dest->clientTrigger.origins);
			}
			if (data->clientTrigger.scriptDelay)
			{
				buf->align(3);
				buf->write(data->clientTrigger.scriptDelay, data->clientTrigger.trigger.modelCount);
				ZoneBuffer::ClearPointer(&dest->clientTrigger.scriptDelay);
			}
			if (data->clientTrigger.audioTriggers)
			{
				buf->align(1);
				buf->write(data->clientTrigger.audioTriggers, data->clientTrigger.trigger.modelCount);
				ZoneBuffer::ClearPointer(&dest->clientTrigger.audioTriggers);
			}

			END_LOG_STREAM;
			buf->pop_stream();

			encrypt_data(dest, sizeof MapEnts);
		}

		void IMapEnts::dump(MapEnts* asset)
		{
			if (asset)
			{
				auto file = FileSystem::FileOpen(asset->name + ".ents"s, "wb");
				if (file)
				{
					fwrite(asset->entityString, asset->numEntityChars, 1, file);
					FileSystem::FileClose(file);
				}

				AssetDumper triggerDumper;
				if (triggerDumper.Open(asset->name + ".ents.triggers"s))
				{
					triggerDumper.Int(asset->trigger.modelCount);
					triggerDumper.Array<TriggerModel>(asset->trigger.models, asset->trigger.modelCount);

					triggerDumper.Int(asset->trigger.hullCount);
					triggerDumper.Array<TriggerHull>(asset->trigger.hulls, asset->trigger.hullCount);

					triggerDumper.Int(asset->trigger.slabCount);
					triggerDumper.Array<TriggerSlab>(asset->trigger.slabs, asset->trigger.slabCount);

					triggerDumper.Close();
				}
			}
		}
	}
}
