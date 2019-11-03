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
	namespace IW3
	{
		void ILoadedSound::dump(LoadedSound* asset, ZoneMemory* mem)
		{
			if (asset->struct1.waveFormat != 1) 
			{
				ZONETOOL_ERROR("Audio format other than PCM currently not supported. Sound: %s\n", asset->name);
				return;
			}

			const auto fp = FileSystem::FileOpen("loaded_sound\\"s + asset->name, "wb");
			if (!fp)
			{
				return;
			}

			// --- RIF HEADER
	// ChunkID
			char chunkID[] = { 'R', 'I', 'F', 'F' };
			fwrite(chunkID, 4, 1, fp);

			// ChunkSize
			int subchunk1Size = 16;
			int subchunk2Size = asset->struct1.dataLength;
			int chunkSize = 4 + (8 + subchunk1Size) + (8 + subchunk2Size);
			fwrite(&chunkSize, 4, 1, fp);

			// Format
			char format[] = { 'W', 'A', 'V', 'E' };
			fwrite(format, 4, 1, fp);


			// --- FMT SUBCHUNK
			// Subchunk1ID
			char subchunk1ID[] = { 'f', 'm', 't', ' ' };
			fwrite(subchunk1ID, 4, 1, fp);

			// Subchunk1Size
			fwrite(&subchunk1Size, 4, 1, fp);

			// AudioFormat
			short audioFormat = asset->struct1.waveFormat;
			fwrite(&audioFormat, 2, 1, fp);

			// NumChannels
			short numChannels = asset->struct1.channelCount;
			fwrite(&numChannels, 2, 1, fp);

			// SampleRate
			int sampleRate = asset->struct1.sampleRate;
			fwrite(&sampleRate, 4, 1, fp);

			// ByteRate
			int byteRate = asset->struct1.sampleRate * asset->struct1.channelCount * asset->struct1.bitPerChannel / 8;
			fwrite(&byteRate, 4, 1, fp);

			// BlockAlign
			short blockAlign = asset->struct1.blockAlign;
			fwrite(&blockAlign, 2, 1, fp);

			// BitsPerSample
			short bitsPerSample = asset->struct1.bitPerChannel;
			fwrite(&bitsPerSample, 2, 1, fp);


			// --- DATA SUBCHUNK
			// Subchunk2ID
			char subchunk2ID[] = { 'd', 'a', 't', 'a' };
			fwrite(subchunk2ID, 4, 1, fp);

			// Subchunk2Size
			fwrite(&subchunk2Size, 4, 1, fp);

			// Data
			fwrite(asset->struct1.soundData, asset->struct1.dataLength, 1, fp);

			FileSystem::FileClose(fp);
		}
	}
}
