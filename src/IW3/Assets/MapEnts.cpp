// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: momo5502 (https://github.com/momo5502)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW4/Assets/MapEnts.hpp"
#include "MapEnts.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		bool StartsWith(const std::string& haystack, const std::string& needle)
		{
			return (haystack.size() >= needle.size() && !strncmp(needle.data(), haystack.data(), needle.size()));
		}

		std::string StrToLower(std::string input)
		{
			std::transform(input.begin(), input.end(), input.begin(), ::tolower);
			return input;
		}

		class Entities
		{
		public:
			Entities()
			{
			};

			Entities(const char* string, size_t lenPlusOne) : Entities(std::string(string, lenPlusOne - 1))
			{
			}

			Entities(std::string buffer) : Entities() { this->parse(buffer); };

			Entities(const Entities& obj) : entities(obj.entities)
			{
			};

			std::string build()
			{
				std::string entityString;

				for (auto& entity : this->entities)
				{
					entityString.append("{\n");

					for (auto& property : entity)
					{
						entityString.push_back('"');
						entityString.append(property.first);
						entityString.append("\" \"");
						entityString.append(property.second);
						entityString.append("\"\n");
					}

					entityString.append("}\n");
				}

				return entityString;
			}

			std::vector<std::string> getModels()
			{
				std::vector<std::string> models;

				for (auto& entity : this->entities)
				{
					if (entity.find("model") != entity.end())
					{
						std::string model = entity["model"];

						if (!model.empty() && model[0] != '*' && model[0] != '?') // Skip brushmodels
						{
							if (std::find(models.begin(), models.end(), model) == models.end())
							{
								models.push_back(model);
							}
						}
					}
				}

				return models;
			}

			void deleteTriggers()
			{
				for (auto i = this->entities.begin(); i != this->entities.end();)
				{
					if (i->find("classname") != i->end())
					{
						std::string classname = (*i)["classname"];
						if (StartsWith(classname, "trigger_"))
						{
							i = this->entities.erase(i);
							continue;
						}
					}

					++i;
				}
			}

			void deleteWeapons(bool keepTurrets)
			{
				for (auto i = this->entities.begin(); i != this->entities.end();)
				{
					if (i->find("weaponinfo") != i->end() || (i->find("targetname") != i->end() && (*i)["targetname"] ==
						"oldschool_pickup"s))
					{
						if (!keepTurrets || i->find("classname") == i->end() || (*i)["classname"] != "misc_turret"s)
						{
							i = this->entities.erase(i);
							continue;
						}
					}

					++i;
				}
			}

			void convertTurrets()
			{
				for (auto& entity : this->entities)
				{
					if (entity.find("classname") != entity.end())
					{
						if (entity["classname"] == "misc_turret"s)
						{
							entity["weaponinfo"] = "turret_minigun_mp";
							entity["model"] = "weapon_minigun";
						}
					}
				}
			}

		private:
			enum
			{
				PARSE_AWAIT_KEY,
				PARSE_READ_KEY,
				PARSE_AWAIT_VALUE,
				PARSE_READ_VALUE,
			};

			std::vector<std::unordered_map<std::string, std::string>> entities;

			void parse(std::string buffer)
			{
				int parseState = 0;
				std::string key;
				std::string value;
				std::unordered_map<std::string, std::string> entity;

				for (unsigned int i = 0; i < buffer.size(); ++i)
				{
					const char character = buffer[i];
					if (character == '{')
					{
						entity.clear();
					}

					switch (character)
					{
					case '{':
						{
							entity.clear();
							break;
						}

					case '}':
						{
							this->entities.push_back(entity);
							entity.clear();
							break;
						}

					case '"':
						{
							if (parseState == PARSE_AWAIT_KEY)
							{
								key.clear();
								parseState = PARSE_READ_KEY;
							}
							else if (parseState == PARSE_READ_KEY)
							{
								parseState = PARSE_AWAIT_VALUE;
							}
							else if (parseState == PARSE_AWAIT_VALUE)
							{
								value.clear();
								parseState = PARSE_READ_VALUE;
							}
							else if (parseState == PARSE_READ_VALUE)
							{
								entity[StrToLower(key)] = value;
								parseState = PARSE_AWAIT_KEY;
							}
							else
							{
								throw std::runtime_error("Parsing error!");
							}
							break;
						}

					default:
						{
							if (parseState == PARSE_READ_KEY) key.push_back(character);
							else if (parseState == PARSE_READ_VALUE) value.push_back(character);

							break;
						}
					}
				}
			}
		};

		void AdaptEntities(IW4::MapEnts* ents, ZoneMemory* mem)
		{
			std::string entString(ents->entityString, ents->numEntityChars - 1);

			Entities mapEnts(entString);
			mapEnts.deleteTriggers();
			mapEnts.deleteWeapons(/*true*/false); // For now delete turrets, as we can't write weapons
			mapEnts.convertTurrets();
			entString = mapEnts.build();

			ents->numEntityChars = entString.size() + 1;
			ents->entityString = mem->Alloc<char>(ents->numEntityChars);
			std::memcpy((char*)ents->entityString, entString.data(), ents->numEntityChars);
		}

		void IMapEnts::dump(MapEnts* asset)
		{
			ZoneMemory mem(20 * 1024 * 1024);

			if (!asset) return;

			auto* mapents = mem.Alloc<IW4::MapEnts>();
			memset(mapents, 0, sizeof IW4::MapEnts);

			mapents->name = asset->name;
			mapents->entityString = asset->entityString;
			mapents->numEntityChars = asset->numEntityChars;

			mapents->stageCount = 1;
			mapents->stageNames = mem.Alloc<IW4::Stage>();
			mapents->stageNames[0].stageName = mem.StrDup("stage 0");
			mapents->stageNames[0].triggerIndex = 0x400;
			mapents->stageNames[0].sunPrimaryLightIndex = 0x1;

			// Remove unsupported stuff
			AdaptEntities(mapents, &mem);

			IW4::IMapEnts::dump(mapents);
		}
	}
}
