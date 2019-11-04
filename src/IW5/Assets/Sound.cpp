// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

#define SOUND_DUMP_STRING(entry) \
	if (asset->entry) sound[#entry] = std::string(asset->entry); \
	else sound[#entry] = nullptr;
#define SOUND_DUMP_INT(entry) \
	sound[#entry] = (int)asset->entry;
#define SOUND_DUMP_FLOAT(entry) \
	sound[#entry] = (float)asset->entry;

#define SOUND_STRING(entry) \
	if (!snddata[#entry].is_null()) { \
		asset->entry = _strdup(snddata[#entry].get<std::string>().c_str()); \
	} else { asset->entry = nullptr; }
#define SOUND_INT(entry) \
	asset->entry = snddata[#entry].get<int>()
#define SOUND_FLOAT(entry) \
	asset->entry = snddata[#entry].get<float>()

namespace ZoneTool
{
	namespace IW5
	{
		ISound::ISound()
		{
		}

		ISound::~ISound()
		{
		}

		void SndAlias_Parse(snd_alias_t* asset, nlohmann::json snddata, ZoneMemory* mem)
		{
			SOUND_STRING(aliasName);
			SOUND_STRING(subtitle);
			SOUND_STRING(secondaryAliasName);
			SOUND_STRING(chainAliasName);
			SOUND_STRING(mixerGroup);

			if (!snddata["type"].is_null())
			{
				asset->soundFile = mem->Alloc<SoundFile>();
				asset->soundFile->type = snddata["type"].get<int>();
				asset->soundFile->exists = true;

				if (asset->soundFile->type == 1)
				{
					asset->soundFile->sound.loadSnd = mem->Alloc<LoadedSound>();
					// (LoadedSound*)malloc(sizeof(LoadedSound*));
					asset->soundFile->sound.loadSnd->name = _strdup(snddata["soundfile"].get<std::string>().c_str());
				}
				else
				{
					char* fullpath = _strdup(snddata["soundfile"].get<std::string>().c_str());
					asset->soundFile->sound.streamSnd.dir = fullpath;
					for (int i = strnlen(fullpath, 255) - 1; i >= 0; i--)
					{
						if (fullpath[i] == '/')
						{
							fullpath[i] = '\0';
							asset->soundFile->sound.streamSnd.name = &fullpath[i + 1];
							break;
						}
					}
				}

				SOUND_INT(sequence);
				SOUND_INT(flags);
				SOUND_INT(startDelay);

				// floats
				SOUND_FLOAT(volMin);
				SOUND_FLOAT(volMax);
				SOUND_FLOAT(pitchMin);
				SOUND_FLOAT(pitchMax);
				SOUND_FLOAT(distMin);
				SOUND_FLOAT(distMax);

				SOUND_FLOAT(slavePercentage);
				SOUND_FLOAT(probability);
				SOUND_FLOAT(lfePercentage);
				SOUND_FLOAT(centerPercentage);
				SOUND_FLOAT(envelopMin);
				SOUND_FLOAT(envelopMax);
				SOUND_FLOAT(envelopPercentage);

				// game specific
				//		SOUND_INT(volModIndex);
				//		SOUND_INT(masterPriority);
				//		SOUND_FLOAT(velocityMin);
				//		SOUND_FLOAT(masterPercentage);

				if (!snddata["volumeFalloffCurve"].is_null())
				{
					asset->volumeFalloffCurve = mem->Alloc<SndCurve>(); // (SndCurve*)calloc(1, sizeof(SndCurve));
					asset->volumeFalloffCurve->filename = _strdup(
						snddata["volumeFalloffCurve"].get<std::string>().c_str());
				}

				if (!snddata["speakerMap"].is_null())
				{
					asset->speakerMap = mem->Alloc<SpeakerMap>(); // (SpeakerMap*)calloc(1, sizeof(SpeakerMap));
					nlohmann::json speakerMap = snddata["speakerMap"];

					asset->speakerMap->name = _strdup(speakerMap["name"].get<std::string>().c_str());
					asset->speakerMap->isDefault = speakerMap["isDefault"].get<bool>();

					if (!speakerMap["channelMaps"].is_null())
					{
						nlohmann::json channelMaps = speakerMap["channelMaps"];
						for (char x = 0; x < 2; x++)
						{
							for (char y = 0; y < 2; y++)
							{
								if (!channelMaps[(x & 0x01) << 1 | y & 0x01].is_null())
								{
									nlohmann::json channelMap = channelMaps[(x & 0x01) << 1 | y & 0x01];
									asset->speakerMap->channelMaps[x][y].entryCount = channelMap["entryCount"].get<int
									>();

									if (!channelMap["speakers"].is_null())
									{
										nlohmann::json speakers = channelMap["speakers"];

										for (int speaker = 0; speaker < asset->speakerMap->channelMaps[x][y].entryCount;
										     speaker++)
										{
											if (!speakers[speaker].is_null())
											{
												nlohmann::json jspeaker = speakers[speaker];
												asset->speakerMap->channelMaps[x][y].speakers[speaker].speaker =
													jspeaker["speaker"].get<int>();
												asset->speakerMap->channelMaps[x][y].speakers[speaker].numLevels =
													jspeaker["numLevels"].get<int>();
												asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[0] =
													jspeaker["levels0"].get<float>();
												asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[1] =
													jspeaker["levels1"].get<float>();
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		snd_alias_list_t* ISound::parse(const std::string& name, ZoneMemory* mem)
		{
			if (name.empty())
			{
				return nullptr;
			}

			std::string path = "sounds/" + name;

			if (FileSystem::FileExists(path))
			{
				ZONETOOL_INFO("Parsing sound %s...", name.c_str());

				auto file = FileSystem::FileOpen(path, "rb");
				auto size = FileSystem::FileSize(file);
				auto bytes = FileSystem::ReadBytes(file, size);
				FileSystem::FileClose(file);

				nlohmann::json snddata = nlohmann::json::parse(bytes);

				snd_alias_list_t* asset = mem->Alloc<snd_alias_list_t>();

				SOUND_STRING(aliasName);
				SOUND_INT(count);

				asset->head = mem->Alloc<snd_alias_t>(asset->count);

				nlohmann::json heads = snddata["head"];
				for (int i = 0; i < asset->count; i++)
				{
					SndAlias_Parse(&asset->head[i], heads[i], mem);
				}

				return asset;
			}

			return nullptr;
		}

		void ISound::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).sound;
			}
		}

		void ISound::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ISound::load_depending(IZone* zone)
		{
			auto data = this->asset_;

			for (auto i = 0u; i < data->count; i++)
			{
				auto head = &data->head[i];

				if (head->volumeFalloffCurve)
				{
					zone->add_asset_of_type(sndcurve, head->volumeFalloffCurve->name);
				}

				if (head->soundFile)
				{
					if (head->soundFile->type == SAT_LOADED)
					{
						zone->add_asset_of_type(loaded_sound, head->soundFile->sound.loadSnd->name);
					}
				}
			}
		}

		std::string ISound::name()
		{
			return this->name_;
		}

		std::int32_t ISound::type()
		{
			return sound;
		}

		void ISound::write_soundfile(IZone* zone, ZoneBuffer* buf, SoundFile* data)
		{
			auto dest = buf->write(data);

			if (data->type == SAT_STREAMED || data->type == SAT_PRIMED)
			{
				if (data->sound.streamSnd.dir)
				{
					dest->sound.streamSnd.dir = buf->write_str(data->sound.streamSnd.dir);
				}

				if (data->sound.streamSnd.name)
				{
					dest->sound.streamSnd.name = buf->write_str(data->sound.streamSnd.name);
				}
			}
			else if (data->type == SAT_LOADED)
			{
				if (data->sound.loadSnd)
				{
					dest->sound.loadSnd = reinterpret_cast<LoadedSound*>(zone->get_asset_pointer(
						loaded_sound, data->sound.loadSnd->name));
				}
			}
		}

		void ISound::write_head(IZone* zone, ZoneBuffer* buf, snd_alias_t* dest)
		{
			auto data = dest;

			if (data->aliasName)
			{
				dest->aliasName = buf->write_str(data->aliasName);
			}

			if (data->subtitle)
			{
				dest->subtitle = buf->write_str(data->subtitle);
			}

			if (data->secondaryAliasName)
			{
				dest->secondaryAliasName = buf->write_str(data->secondaryAliasName);
			}

			if (data->chainAliasName)
			{
				dest->chainAliasName = buf->write_str(data->chainAliasName);
			}

			if (data->mixerGroup)
			{
				dest->mixerGroup = buf->write_str(data->mixerGroup);
			}

			if (data->soundFile)
			{
				buf->align(3);
				write_soundfile(zone, buf, data->soundFile);
				ZoneBuffer::ClearPointer(&dest->soundFile);
			}

			if (data->volumeFalloffCurve)
			{
				dest->volumeFalloffCurve = reinterpret_cast<SndCurve*>(zone->get_asset_pointer(
					sndcurve, data->volumeFalloffCurve->name));
			}

			if (data->speakerMap)
			{
				buf->align(3);
				auto speakerMap = buf->write(data->speakerMap);

				if (speakerMap->name)
				{
					speakerMap->name = buf->write_str(speakerMap->name);
				}

				ZoneBuffer::ClearPointer(&dest->speakerMap);
			}
		}

		void ISound::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (dest->head)
			{
				buf->align(3);
				auto destSound = buf->write(data->head, data->count);

				for (std::int32_t i = 0; i < data->count; i++)
				{
					write_head(zone, buf, &destSound[i]);
				}

				ZoneBuffer::ClearPointer(&dest->head);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void Dump_SndAlias(nlohmann::json& sound, snd_alias_t* asset/*, std::vector<nlohmann::json*>* toFree*/)
		{
			// strings
			SOUND_DUMP_STRING(aliasName);
			SOUND_DUMP_STRING(subtitle);
			SOUND_DUMP_STRING(secondaryAliasName);
			SOUND_DUMP_STRING(chainAliasName);
			SOUND_DUMP_STRING(mixerGroup);

			// soundfile shit
			if (asset->soundFile)
			{
				sound["type"] = (int)asset->soundFile->type;

				if (asset->soundFile->type == 1)
					sound["soundfile"] = asset->soundFile->sound.loadSnd->name;
				else
					sound["soundfile"] = std::string(asset->soundFile->sound.streamSnd.dir) + "/" + std::string(
						asset->soundFile->sound.streamSnd.name);
			}

			// ints
			SOUND_DUMP_INT(sequence);
			SOUND_DUMP_INT(flags);
			SOUND_DUMP_INT(startDelay);

			// floats
			SOUND_DUMP_FLOAT(volMin);
			SOUND_DUMP_FLOAT(volMax);
			SOUND_DUMP_FLOAT(pitchMin);
			SOUND_DUMP_FLOAT(pitchMax);
			SOUND_DUMP_FLOAT(distMin);
			SOUND_DUMP_FLOAT(distMax);

			SOUND_DUMP_FLOAT(slavePercentage);
			SOUND_DUMP_FLOAT(probability);
			SOUND_DUMP_FLOAT(lfePercentage);
			SOUND_DUMP_FLOAT(centerPercentage);
			SOUND_DUMP_FLOAT(envelopMin);
			SOUND_DUMP_FLOAT(envelopMax);
			SOUND_DUMP_FLOAT(envelopPercentage);

			// game specific
#ifdef IW5
			//		SOUND_DUMP_INT(volModIndex);
			//		SOUND_DUMP_INT(masterPriority);
			//		SOUND_DUMP_FLOAT(velocityMin);
			//		SOUND_DUMP_FLOAT(masterPercentage);
#endif

			if (asset->volumeFalloffCurve && asset->volumeFalloffCurve->filename)
				sound["volumeFalloffCurve"] = asset->volumeFalloffCurve->filename;

			if (asset->speakerMap)
			{
				nlohmann::json speakerMap;
				speakerMap["name"] = asset->speakerMap->name;
				speakerMap["isDefault"] = asset->speakerMap->isDefault;

				nlohmann::json channelMaps;
				for (char x = 0; x < 2; x++)
				{
					for (char y = 0; y < 2; y++)
					{
						nlohmann::json channelMap;
						channelMap["entryCount"] = asset->speakerMap->channelMaps[x][y].entryCount;

						nlohmann::json speakers;

						for (int speaker = 0; speaker < asset->speakerMap->channelMaps[x][y].entryCount; speaker++)
						{
							nlohmann::json jspeaker;

							jspeaker["speaker"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].speaker;
							jspeaker["numLevels"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].numLevels;
							jspeaker["levels0"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[0];
							jspeaker["levels1"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[1];

							speakers[speaker] = jspeaker;
						}

						channelMap["speakers"] = speakers;

						channelMaps[(x & 0x01) << 1 | y & 0x01] = channelMap;
					}
				}

				speakerMap["channelMaps"] = channelMaps;

				sound["speakerMap"] = speakerMap;
			}
		}

		void Dump_SndAliasList(snd_alias_list_t* asset)
		{
			nlohmann::json sound;
			//std::vector<nlohmann::json*> toFree;
			//toFree.clear();

			SOUND_DUMP_STRING(aliasName);
			SOUND_DUMP_INT(count);

			nlohmann::json aliases;

			for (int i = 0; i < asset->count; i++)
			{
				nlohmann::json alias;
				Dump_SndAlias(alias, &asset->head[i]/*, &toFree*/);
				aliases[i] = alias;
			}

			sound["head"] = aliases;

			std::string assetstr = sound.dump(4);
			FILE* fp = FileSystem::FileOpen("sounds/"s + asset->name, "wb");

			if (fp)
			{
				fwrite(assetstr.c_str(), assetstr.size(), 1, fp);
				fclose(fp);
			}
		}

		void ISound::dump(snd_alias_list_t* asset)
		{
			Dump_SndAliasList(asset);
		}
	}
}
