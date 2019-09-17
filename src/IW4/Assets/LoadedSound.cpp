#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		ILoadedSound::ILoadedSound()
		{
		}

		ILoadedSound::~ILoadedSound()
		{
		}

		LoadedSound* ILoadedSound::parse(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
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
				ZONETOOL_ERROR("%s: Invalid RIFF Header.", name.c_str());
				fclose(fp);
				return nullptr;
			}

			fread(&chunkSize, 4, 1, fp);
			fread(&chunkIDBuffer, 4, 1, fp);

			if (chunkIDBuffer != 0x45564157) // WAVE
			{
				ZONETOOL_ERROR("%s: Invalid WAVE Header.", name.c_str());
				fclose(fp);
				return nullptr;
			}

			char* data;

			while (!result->sound.soundData && !feof(fp))
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
							ZONETOOL_ERROR("%s: Invalid wave format %i.", name.c_str(), format);
							fclose(fp);
							return nullptr;
						}
						result->sound.waveFormat = format;

						short numChannels;
						fread(&numChannels, 2, 1, fp);
						result->sound.channelCount = numChannels;

						int sampleRate;
						fread(&sampleRate, 4, 1, fp);
						result->sound.sampleRate = sampleRate;

						int byteRate;
						fread(&byteRate, 4, 1, fp);

						short blockAlign;
						fread(&blockAlign, 2, 1, fp);
						result->sound.blockAlign = blockAlign;

						short bitPerSample;
						fread(&bitPerSample, 2, 1, fp);
						result->sound.bitPerChannel = bitPerSample;

						if (chunkSize > 16)
						{
							fseek(fp, chunkSize - 16, SEEK_CUR);
						}
					}
					break;

				case 0x61746164: // data
					result->sound.dataLength = chunkSize;
					data = (char*)malloc(result->sound.dataLength);
					fread(data, 1, result->sound.dataLength, fp);
					result->sound.soundData = data;
					break;

				default:
					if (chunkSize > 0)
					{
						fseek(fp, chunkSize, SEEK_CUR);
					}
					break;
				}
			}

			if (!result->sound.soundData)
			{
				ZONETOOL_ERROR("%s: Could not read sounddata.", name.c_str());
				fclose(fp);
				return nullptr;
			}

			result->name = mem->StrDup(name);

			FileSystem::FileClose(fp);
			return result;
		}

		void ILoadedSound::init(const std::string& name, std::shared_ptr<ZoneMemory>& mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				ZONETOOL_WARNING("Sound %s not found, it will probably sound like a motorboat ingame!", name.data());
				this->m_asset = DB_FindXAssetHeader(this->type(), this->name().data()).loadedsound;
			}
		}

		void ILoadedSound::prepare(std::shared_ptr<ZoneBuffer>& buf, std::shared_ptr<ZoneMemory>& mem)
		{
		}

		void ILoadedSound::load_depending(IZone* zone)
		{
		}

		std::string ILoadedSound::name()
		{
			return this->m_name;
		}

		std::int32_t ILoadedSound::type()
		{
			return loaded_sound;
		}

		void ILoadedSound::write(IZone* zone, std::shared_ptr<ZoneBuffer>& buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			buf->push_stream(0);

			if (data->sound.soundData)
			{
				buf->align(0);
				buf->write(data->sound.soundData, data->sound.dataLength);
				ZoneBuffer::ClearPointer(&dest->sound.soundData);
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
				int subchunk2Size = sound->sound.dataLength;
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
				short audioFormat = sound->sound.waveFormat;
				fwrite(&audioFormat, 2, 1, fp);

				// NumChannels
				short numChannels = sound->sound.channelCount;
				fwrite(&numChannels, 2, 1, fp);

				// SampleRate
				int sampleRate = sound->sound.sampleRate;
				fwrite(&sampleRate, 4, 1, fp);

				// ByteRate
				int byteRate = sound->sound.sampleRate * sound->sound.channelCount * sound->sound.bitPerChannel / 8;
				fwrite(&byteRate, 4, 1, fp);

				// BlockAlign
				short blockAlign = sound->sound.blockAlign;
				fwrite(&blockAlign, 2, 1, fp);

				// BitsPerSample
				short bitsPerSample = sound->sound.bitPerChannel;
				fwrite(&bitsPerSample, 2, 1, fp);


				// --- DATA SUBCHUNK
				// Subchunk2ID
				char subchunk2ID[] = {'d', 'a', 't', 'a'};
				fwrite(subchunk2ID, 4, 1, fp);

				// Subchunk2Size
				fwrite(&subchunk2Size, 4, 1, fp);

				// Data
				fwrite(sound->sound.soundData, sound->sound.dataLength, 1, fp);
			}

			FileSystem::FileClose(fp);
		}
	}
}
