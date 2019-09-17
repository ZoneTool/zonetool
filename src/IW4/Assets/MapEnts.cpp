#include "stdafx.hpp"
#include "ZoneUtils/Utils/BinaryDumper.hpp"
#include "../IW5/Assets/MapEnts.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		int freadint(FILE* file)
		{
			int data = 0;

			if (file)
			{
				fread(&data, sizeof(int), 1, file);
			}

			return data;
		}

		std::unordered_map<std::string, std::string> keyConversion =
		{
			{"1", "pl#"},
			{"1668", "classname"},
			{"1669", "origin"},
			{"1670", "model"},
			{"1671", "spawnflags"},
			{"1672", "target"},
			{"1673", "targetname"},
			{"1676", "dmg"},
			{"1677", "angles"},
			{"1679", "script_linkname"},
			{"1705", "intensity"},
			{"1774", "script_noteworthy"},
			{"1775", "speed"},
			{"1776", "lookahead"},
			{"1782", "radius"},
			{"1783", "height"},
			{"1788", "script_speed"},
			{"1983", "animscript"},
			{"1987", "ambient"},
			{"1989", "sunlight"},
			{"1990", "suncolor"},
			{"1991", "sundirection"},
			{"2009", "script_exploder"},
			{"2328", "script_linkto"},
			{"2369", "destructible_type"},
			{"2810", "sunRadiosity"},
			{"2811", "skycolor"},
			{"2812", "skylight"},
			{"2813", "_color"},
			{"2814", "ltOrigin"},
			{"2815", "gndLt"},
			{"2816", "sound_csv_include"},
			{"2817", "csv_include"},
			{"2818", "precache_script"},
			{"2820", "maxbounces"},
			{"2821", "radiosityScale"},
			{"2823", "def"},
			{"2824", "exponent"},
			{"2825", "fov_inner"},
			{"2826", "fov_outer"},
			{"2827", "__smorigin"},
			{"2828", "__smangles"},
			{"2829", "__smname"},
			{"2830", "__smid"},
			{"3717", "script_destruct_collision"},
			{"4630", "script_bombmode_original"},
			{"7712", "modelscale"},
			{"7876", "script_accel"},
			{"10338", "script_targetoffset_z"},
			{"10396", "script_airspeed"},
			// we don't need this one for MP maps
			// { "11039", "animation" },
			{"11848", "script_gameobjectname"},
			{"11996", "script_label"},
		};
		std::unordered_map<std::string, std::string> keyConversionReversed;

		void IMapEnts::ConvertEnts(MapEnts* ents, std::shared_ptr<ZoneMemory>& mem)
		{
			ZONETOOL_INFO("Converting mapents!");

			std::string newEntsString;
			std::string currentEntity;
			std::string lastKey;
			bool isValidEntity;
			bool isParsingKey;

			// make sure keyConversionReversed is prepared
			keyConversionReversed.clear();

			// prepare keyConversionReversed
			for (auto& key : keyConversion)
			{
				keyConversionReversed[key.second] = key.first;
			}

			// parse expressions
			ExpressionParser parser(ents->entityString);

			// 
			std::string expression = parser.Parse(true);
			while (!expression.empty())
			{
				// convert token to lower
				std::transform(expression.begin(), expression.end(), expression.begin(), tolower);

				// start parsing new entity
				if (expression == "{"s)
				{
					// clear data from previous entity
					currentEntity.clear();
					isValidEntity = true;
					isParsingKey = true;

					// add expression to current expression buffer
					currentEntity += expression + "\n";
				}
					// finalize current entity
				else if (expression == "}"s)
				{
					// add expression to current expression buffer
					currentEntity += expression + "\n";

					// check if the entity we're about to add to the mapents is valid
					if (isValidEntity)
					{
						// add current expression to entity buffer
						newEntsString += currentEntity;
					}
					else
					{
						ZONETOOL_INFO("Not adding entity because it contains invalid keys! Data was %s", &currentEntity[
0]);
					}
				}
					// check key values
				else
				{
					if (isParsingKey)
					{
						auto itr1 = keyConversion.find(expression);
						auto itr2 = keyConversionReversed.find(expression);

						// checks if the key is unknown to both iw4 and iw5
						if (itr1 == keyConversion.end() && itr2 == keyConversionReversed.end())
						{
							ZONETOOL_INFO("Unknown mapents key \"%s\". Removing entity from mapents...", &expression[0]
);

							// remove current entity from mapents file
							isValidEntity = false;
						}
							// when converting the key
						else if (itr1 != keyConversion.end())
						{
							currentEntity += "\"" + itr1->second + "\"";
							lastKey = itr1->second;
						}
							// when the key is already in iw4 format
						else
						{
							currentEntity += "\"" + expression + "\"";
							lastKey = expression;
						}
					}
					else
					{
						// check if we actually need the current key/value combo
						if (
							(lastKey == "classname"s && expression.length() >= 5 && expression.substr(0, 5) == "node_"s)
							||
							(lastKey == "targetname"s && expression == "delete_on_load"s)
						)
						{
							// remove current entity from mapents file
							isValidEntity = false;
						}

						// add expression to current expression buffer
						currentEntity += " \"" + expression + "\"\n";
					}

					// invert parsing key state
					isParsingKey = !isParsingKey;
				}

				// parse next expression
				expression = parser.Parse(true);
			}

			// ZONETOOL_INFO("Entity string is %s", newEntsString.data());

			// replace mapents if needed
			if (!newEntsString.empty())
			{
				ents->numEntityChars = newEntsString.size();
				ents->entityString = mem->Alloc<char>(ents->numEntityChars + 1);
				memcpy((void*)ents->entityString, &newEntsString[0], ents->numEntityChars);
			}

			//auto fp = fopen("output.d3dbsp.ents", "wb");
			//fwrite(ents->entityString, ents->numEntityChars, 1, fp);
			//fclose(fp);
		}

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

			// convert the mapents!
			this->ConvertEnts(ents, mem);

			// close filepointer
			FileSystem::FileClose(file);

			AssetReader triggerReader(mem);
			AssetReader stageReader(mem);

			if (triggerReader.Open(name + ".ents.triggers"))
			{
				ents->trigger.modelCount = triggerReader.Int();
				ents->trigger.models = triggerReader.Array<TriggerModel>();

				ents->trigger.hullCount = triggerReader.Int();
				ents->trigger.hulls = triggerReader.Array<TriggerHull>();

				ents->trigger.slabCount = triggerReader.Int();
				ents->trigger.slabs = triggerReader.Array<TriggerSlab>();
			}

			if (stageReader.Open(name + ".ents.stages"))
			{
				ZONETOOL_INFO("Parsing entity stages...");

				ents->stageCount = stageReader.Int();
				if (ents->stageCount)
				{
					ents->stageNames = stageReader.Array<Stage>();

					for (int i = 0; i < ents->stageCount; i++)
					{
						ents->stageNames[i].stageName = stageReader.String();
					}
				}
			}

			stageReader.Close();
			triggerReader.Close();

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
				this->m_asset = DB_FindXAssetHeader(this->type(), name.data()).mapents;
			}
		}

		void IMapEnts::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void IMapEnts::load_depending(IZone* zone)
		{
			// IW5::IMapEnts::load_depending_internal(zone, this->m_asset->entityString);
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
				dest->models = buf->write_s(3, dest->models, dest->modelCount);
			}
			if (dest->hulls)
			{
				dest->hulls = buf->write_s(3, dest->hulls, dest->hullCount);
			}
			if (dest->slabs)
			{
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

			if (data->stageNames)
			{
				buf->align(3);
				auto stage = buf->write(data->stageNames, data->stageCount);

				for (int i = 0; i < data->stageCount; i++)
				{
					if (data->stageNames[i].stageName)
					{
						buf->write_str(data->stageNames[i].stageName);
						ZoneBuffer::ClearPointer(&stage[i].stageName);
					}
				}

				ZoneBuffer::ClearPointer(&dest->stageNames);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IMapEnts::dump(MapEnts* asset)
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

			AssetDumper stageDumper;
			if (stageDumper.Open(asset->name + ".ents.stages"s))
			{
				stageDumper.Int(asset->stageCount);
				if (asset->stageCount)
				{
					stageDumper.Array(asset->stageNames, asset->stageCount);

					for (int i = 0; i < asset->stageCount; i++)
					{
						stageDumper.String(asset->stageNames[i].stageName);
					}
				}
			}
			stageDumper.Close();
		}
	}
}
