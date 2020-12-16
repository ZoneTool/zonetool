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
		snd_alias_list_t* ISound::parse(const std::string& name, ZoneMemory* mem)
		{
			if (name.empty())
			{
				return nullptr;
			}

			const auto path = "sounds\\"s + name + ".xss";

			if (FileSystem::FileExists(path))
			{
				AssetReader reader(mem);
				if (!reader.open(path))
				{
					return nullptr;
				}

				ZONETOOL_INFO("Parsing sound %s...", name.c_str());

				auto* asset = reader.read_single<snd_alias_list_t>();
				asset->name = reader.read_string();
				asset->head = reader.read_array<snd_alias_t>();

				for (auto i = 0; i < asset->count; i++)
				{
					auto* current = &asset->head[i];

					if (current->aliasName)
					{
						current->aliasName = reader.read_string();
					}

					if (current->subtitle)
					{
						current->subtitle = reader.read_string();
					}

					if (current->secondaryAliasName)
					{
						current->secondaryAliasName = reader.read_string();
					}

					if (current->chainAliasName)
					{
						current->chainAliasName = reader.read_string();
					}

					if (current->mixerGroup)
					{
						current->mixerGroup = reader.read_string();
					}

					if (current->soundFile)
					{
						current->soundFile = reader.read_single<SoundFile>();

						if (current->soundFile->type == SAT_LOADED)
						{
							current->soundFile->sound.loadSnd = reader.read_asset<LoadedSound>();
						}
						else
						{
							current->soundFile->sound.streamSnd.name = reader.read_string();
							current->soundFile->sound.streamSnd.dir = reader.read_string();
						}
					}

					if (current->volumeFalloffCurve)
					{
						current->volumeFalloffCurve = reader.read_asset<SndCurve>();
					}

					if (current->speakerMap)
					{
						current->speakerMap = reader.read_single<SpeakerMap>();
						current->speakerMap->name = reader.read_string();
					}
				}

				reader.close();
				
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
			auto* data = this->asset_;

			for (auto i = 0u; i < data->count; i++)
			{
				auto* head = &data->head[i];

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
			auto* dest = buf->write(data);

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
					dest->sound.loadSnd = static_cast<LoadedSound*>(zone->get_asset_pointer(
						loaded_sound, data->sound.loadSnd->name));
				}
			}
		}

		void ISound::write_head(IZone* zone, ZoneBuffer* buf, snd_alias_t* dest)
		{
			auto* data = dest;

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
				ZoneBuffer::clear_pointer(&dest->soundFile);
			}

			if (data->volumeFalloffCurve)
			{
				dest->volumeFalloffCurve = static_cast<SndCurve*>(zone->get_asset_pointer(
					sndcurve, data->volumeFalloffCurve->name));
			}

			if (data->speakerMap)
			{
				buf->align(3);
				auto* speaker_map = buf->write(data->speakerMap);

				if (speaker_map->name)
				{
					speaker_map->name = buf->write_str(speaker_map->name);
				}

				ZoneBuffer::clear_pointer(&dest->speakerMap);
			}
		}

		void ISound::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (dest->head)
			{
				buf->align(3);
				auto* dest_sound = buf->write(data->head, data->count);

				for (std::int32_t i = 0; i < data->count; i++)
				{
					write_head(zone, buf, &dest_sound[i]);
				}

				ZoneBuffer::clear_pointer(&dest->head);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ISound::dump(snd_alias_list_t* asset)
		{
			const auto path = "sounds\\"s + asset->name + ".xss";

			AssetDumper dump;
			if (!dump.open(path))
			{
				return;
			}
			
			dump.dump_single(asset);
			dump.dump_string(asset->name);
			dump.dump_array(asset->head, asset->count);
			
			for (auto i = 0; i < asset->count; i++)
			{
				auto* current = &asset->head[i];

				if (current->aliasName)
				{
					dump.dump_string(current->aliasName);
				}

				if (current->subtitle)
				{
					dump.dump_string(current->subtitle);
				}

				if (current->secondaryAliasName)
				{
					dump.dump_string(current->secondaryAliasName);
				}

				if (current->chainAliasName)
				{
					dump.dump_string(current->chainAliasName);
				}

				if (current->mixerGroup)
				{
					dump.dump_string(current->mixerGroup);
				}

				if (current->soundFile)
				{
					dump.dump_single(current->soundFile);

					if (current->soundFile->type == SAT_LOADED)
					{
						dump.dump_asset(current->soundFile->sound.loadSnd);
					}
					else
					{
						dump.dump_string(current->soundFile->sound.streamSnd.name);
						dump.dump_string(current->soundFile->sound.streamSnd.dir);
					}
				}

				if (current->volumeFalloffCurve)
				{
					dump.dump_asset(current->volumeFalloffCurve);
				}

				if (current->speakerMap)
				{
					dump.dump_single(current->speakerMap);
					dump.dump_string(current->speakerMap->name);
				}
			}
			
			dump.close();
		}
	}
}
