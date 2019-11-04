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
		ILoadedSound::ILoadedSound()
		{
		}

		ILoadedSound::~ILoadedSound()
		{
		}

		LoadedSound* ILoadedSound::parse(const std::string& name, ZoneMemory* mem)
		{
			auto path = "loaded_sound/" + name;

			if (!FileSystem::FileExists(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing loaded_sound \"%s\"...", name.data());

			auto fp = FileSystem::FileOpen(path, "rb");
			auto result = mem->Alloc<LoadedSound>();

			unsigned int chunkIDBuffer;
			unsigned int chunkSize;

			fread(&chunkIDBuffer, 4, 1, fp);
			if (chunkIDBuffer != 0x46464952) // RIFF
			{
				ZONETOOL_FATAL("%s: Invalid RIFF Header.", name.c_str());
				/*fclose(fp_);
				return nullptr;*/
			}

			fread(&chunkSize, 4, 1, fp);
			fread(&chunkIDBuffer, 4, 1, fp);

			if (chunkIDBuffer != 0x45564157) // WAVE
			{
				ZONETOOL_FATAL("%s: Invalid WAVE Header.", name.c_str());
				/*fclose(fp_);
				return nullptr;*/
			}

			char* data;

			while (!result->sound.data && !feof(fp))
			{
				fread(&chunkIDBuffer, 4, 1, fp);
				fread(&chunkSize, 4, 1, fp);
				switch (chunkIDBuffer)
				{
				case 0x20746D66: // fmt
					if (chunkSize >= 16)
					{
						short format;
						fread(&format, 2, 1, fp);
						if (format != 1 && format != 17)
						{
							ZONETOOL_FATAL("%s: Invalid wave format %i.", name.c_str(), format);
							/*fclose(fp_);
							return nullptr;*/
						}
						result->sound.info.format = format;

						short numChannels;
						fread(&numChannels, 2, 1, fp);
						result->sound.info.channels = numChannels;

						int sampleRate;
						fread(&sampleRate, 4, 1, fp);
						result->sound.info.rate = sampleRate;

						int byteRate;
						fread(&byteRate, 4, 1, fp);

						short blockAlign;
						fread(&blockAlign, 2, 1, fp);
						result->sound.info.block_size = blockAlign;

						short bitPerSample;
						fread(&bitPerSample, 2, 1, fp);
						result->sound.info.bits = bitPerSample;

						if (chunkSize > 16)
						{
							fseek(fp, chunkSize - 16, SEEK_CUR);
						}
					}
					break;

				case 0x61746164: // data
					result->sound.info.data_len = chunkSize;
					data = (char*)malloc(result->sound.info.data_len);
					fread(data, 1, result->sound.info.data_len, fp);
					result->sound.data = data;
					break;

				default:
					if (chunkSize > 0)
					{
						fseek(fp, chunkSize, SEEK_CUR);
					}
					break;
				}
			}

			if (!result->sound.data)
			{
				ZONETOOL_FATAL("%s: Could not read sounddata.", name.c_str());
				//fclose(fp_);
				//return nullptr;
			}

			result->name = mem->StrDup(name);

			FileSystem::FileClose(fp);
			return result;
		}

		void ILoadedSound::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				ZONETOOL_WARNING("Sound %s not found, it will probably sound like a motorboat ingame!", name.data());
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).loadedsound;
			}
		}

		void ILoadedSound::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ILoadedSound::load_depending(IZone* zone)
		{
		}

		std::string ILoadedSound::name()
		{
			return this->name_;
		}

		std::int32_t ILoadedSound::type()
		{
			return loaded_sound;
		}

		void ILoadedSound::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			buf->push_stream(0);

			if (data->sound.data)
			{
				buf->align(0);
				buf->write(data->sound.data, data->sound.info.data_len);
				ZoneBuffer::ClearPointer(&dest->sound.data);
			}

			buf->pop_stream();

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ILoadedSound::dump(LoadedSound* asset)
		{
			auto sound = asset;
			auto fp = FileSystem::FileOpen("loaded_sound/"s + asset->name, "wb");

			if (fp)
			{
				char chunkID[] = {'R', 'I', 'F', 'F'};
				fwrite(chunkID, 4, 1, fp);

				// ChunkSize
				int subchunk1Size = 16;
				int subchunk2Size = sound->sound.info.data_len;
				int chunkSize = 4 + (8 + subchunk1Size) + (8 + subchunk2Size);
				fwrite(&chunkSize, 4, 1, fp);

				// Format
				char format[] = {'W', 'A', 'V', 'E'};
				fwrite(format, 4, 1, fp);


				// --- FMT SUBCHUNK
				// Subchunk1ID
				char subchunk1ID[] = {'f', 'm', 't', ' '};
				fwrite(subchunk1ID, 4, 1, fp);

				// Subchunk1Size
				fwrite(&subchunk1Size, 4, 1, fp);

				// AudioFormat
				short audioFormat = sound->sound.info.format;
				fwrite(&audioFormat, 2, 1, fp);

				// NumChannels
				short numChannels = sound->sound.info.channels;
				fwrite(&numChannels, 2, 1, fp);

				// SampleRate
				int sampleRate = sound->sound.info.rate;
				fwrite(&sampleRate, 4, 1, fp);

				// ByteRate
				int byteRate = sound->sound.info.rate * sound->sound.info.channels * sound->sound.info.bits / 8;
				fwrite(&byteRate, 4, 1, fp);

				// BlockAlign
				short blockAlign = sound->sound.info.block_size;
				fwrite(&blockAlign, 2, 1, fp);

				// BitsPerSample
				short bitsPerSample = sound->sound.info.bits;
				fwrite(&bitsPerSample, 2, 1, fp);


				// --- DATA SUBCHUNK
				// Subchunk2ID
				char subchunk2ID[] = {'d', 'a', 't', 'a'};
				fwrite(subchunk2ID, 4, 1, fp);

				// Subchunk2Size
				fwrite(&subchunk2Size, 4, 1, fp);

				// Data
				fwrite(sound->sound.data, sound->sound.info.data_len, 1, fp);
			}

			FileSystem::FileClose(fp);
		}
	}
}
