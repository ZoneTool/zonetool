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
#include "../IW5/Assets/MapEnts.hpp"

namespace ZoneTool
{
	namespace IW4
	{
#ifdef CONVERT_IW5_MAPENTS
		std::unordered_map<std::string, std::string> key_conversion =
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
		std::unordered_map<std::string, std::string> key_conversion_reversed;

		void IMapEnts::convert_ents(MapEnts* ents, ZoneMemory* mem)
		{
			ZONETOOL_INFO("Converting mapents!");

			std::string new_ents_string;
			std::string current_entity;
			std::string last_key;
			bool is_valid_entity;
			bool is_parsing_key;

			// make sure key_conversion_reversed is prepared
			key_conversion_reversed.clear();

			// prepare key_conversion_reversed
			for (auto& key : key_conversion)
			{
				key_conversion_reversed[key.second] = key.first;
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
					current_entity.clear();
					is_valid_entity = true;
					is_parsing_key = true;

					// add expression to current expression buffer
					current_entity += expression + "\n";
				}
					// finalize current entity
				else if (expression == "}"s)
				{
					// add expression to current expression buffer
					current_entity += expression + "\n";

					// check if the entity we're about to add to the mapents is valid
					if (is_valid_entity)
					{
						// add current expression to entity buffer
						new_ents_string += current_entity;
					}
					else
					{
						ZONETOOL_INFO("Not adding entity because it contains invalid keys! Data was %s", &current_entity[0]);
					}
				}
					// check key values
				else
				{
					if (is_parsing_key)
					{
						auto itr1 = key_conversion.find(expression);
						auto itr2 = key_conversion_reversed.find(expression);

						// checks if the key is unknown to both iw4 and iw5
						if (itr1 == key_conversion.end() && itr2 == key_conversion_reversed.end())
						{
							ZONETOOL_INFO("Unknown mapents key \"%s\". Removing entity from mapents...", &expression[0]
);

							// remove current entity from mapents file
							is_valid_entity = false;
						}
							// when converting the key
						else if (itr1 != key_conversion.end())
						{
							current_entity += "\"" + itr1->second + "\"";
							last_key = itr1->second;
						}
							// when the key is already in iw4 format
						else
						{
							current_entity += "\"" + expression + "\"";
							last_key = expression;
						}
					}
					else
					{
						// check if we actually need the current key/value combo
						if (
							(last_key == "classname"s && expression.length() >= 5 && expression.substr(0, 5) == "node_"s)
							||
							(last_key == "targetname"s && expression == "delete_on_load"s)
						)
						{
							// remove current entity from mapents file
							is_valid_entity = false;
						}

						// add expression to current expression buffer
						current_entity += " \"" + expression + "\"\n";
					}

					// invert parsing key state
					is_parsing_key = !is_parsing_key;
				}

				// parse next expression
				expression = parser.Parse(true);
			}

			// ZONETOOL_INFO("Entity string is %s", newEntsString.data());

			// replace mapents if needed
			if (!new_ents_string.empty())
			{
				ents->numEntityChars = new_ents_string.size() + 1;
				ents->entityString = mem->Alloc<char>(ents->numEntityChars);
				memset((void*)ents->entityString, 0, ents->numEntityChars);
				memcpy((void*)ents->entityString, &new_ents_string[0], ents->numEntityChars - 1);
			}
		}
#endif

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
			auto* ents = mem->Alloc<MapEnts>();

			ents->name = mem->StrDup(name);
			ents->numEntityChars = FileSystem::FileSize(file);

			ents->entityString = mem->Alloc<char>(ents->numEntityChars + 1);
			memset((char*)ents->entityString, 0, ents->numEntityChars);

			fread((char*)ents->entityString, ents->numEntityChars, 1, file);
			((char*)ents->entityString)[ents->numEntityChars] = '\0';

#ifdef CONVERT_IW5_MAPENTS
			// convert the mapents!
			this->convert_ents(ents, mem);
#endif
			
			// close filepointer
			FileSystem::FileClose(file);

			AssetReader trigger_reader(mem);
			AssetReader stage_reader(mem);

			if (trigger_reader.open(name + ".ents.triggers"))
			{
				ents->trigger.modelCount = trigger_reader.read_int();
				ents->trigger.models = trigger_reader.read_array<TriggerModel>();

				ents->trigger.hullCount = trigger_reader.read_int();
				ents->trigger.hulls = trigger_reader.read_array<TriggerHull>();

				ents->trigger.slabCount = trigger_reader.read_int();
				ents->trigger.slabs = trigger_reader.read_array<TriggerSlab>();
			}

			if (stage_reader.open(name + ".ents.stages"))
			{
				ZONETOOL_INFO("Parsing entity stages...");

				ents->stageCount = stage_reader.read_int();
				if (ents->stageCount)
				{
					ents->stageNames = stage_reader.read_array<Stage>();

					for (auto i = 0; i < ents->stageCount; i++)
					{
						ents->stageNames[i].stageName = stage_reader.read_string();
					}
				}
			}

			stage_reader.close();
			trigger_reader.close();

			// return mapents
			return ents;
		}

		void IMapEnts::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/"s + (currentzone.substr(0, 3) == "mp_" ? "mp/" : "") + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data()).mapents;
			}
		}

		void IMapEnts::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IMapEnts::load_depending(IZone* zone)
		{
			// IW5::IMapEnts::load_depending_internal(zone, this->asset_->entityString);
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

			if (data->stageNames)
			{
				buf->align(3);
				auto* stage = buf->write(data->stageNames, data->stageCount);

				for (auto i = 0; i < data->stageCount; i++)
				{
					if (data->stageNames[i].stageName)
					{
						buf->write_str(data->stageNames[i].stageName);
						ZoneBuffer::clear_pointer(&stage[i].stageName);
					}
				}

				ZoneBuffer::clear_pointer(&dest->stageNames);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IMapEnts::dump(MapEnts* asset)
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

			AssetDumper stage_dumper;
			if (stage_dumper.open(asset->name + ".ents.stages"s))
			{
				stage_dumper.dump_int(asset->stageCount);
				if (asset->stageCount)
				{
					stage_dumper.dump_array(asset->stageNames, asset->stageCount);

					for (auto i = 0; i < asset->stageCount; i++)
					{
						stage_dumper.dump_string(asset->stageNames[i].stageName);
					}
				}
			}
			stage_dumper.close();
		}
	}
}
