// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

#include <zstd.h>
#include <zlib.h>

namespace ZoneTool
{
	namespace IW5
	{
		struct db_zstd_context_s
		{
			// original context
			char pad[24];

			// zstd context
			ZSTD_DCtx* DCtx;
			ZSTD_DStream* DStream;
		};

		struct db_z_stream_s
		{
			char* next_in;
			unsigned int avail_in;
			unsigned int total_in;
			char* next_out;
			unsigned int avail_out;
			unsigned int total_out;
			char* msg;
			db_zstd_context_s* state;
			char*(__cdecl *zalloc)(char*, unsigned int, unsigned int);
			void (__cdecl *zfree)(char*, char*);
			char* opaque;
			int data_type;
			unsigned int adler;
		};

		class FFCompression : public IPatch
		{
		private:
			static std::int32_t z_inflateInit(const char* version, db_z_stream_s* strm, int stream_size);
			static void db_inflateInit();

			static std::int32_t z_inflate(db_z_stream_s* strm);
			static void db_inflate();

			static std::int32_t z_inflateEnd(db_z_stream_s* strm);
			static void db_inflateEnd();

			static void SetFastfileVersion(std::int32_t version);
			static void DB_ReadXFileVersion();

			static void XModelSizeStub();
			static void XModelSizeStub2();

			static void ReadEncryptedKey();

			static void MaterialSizeStub();
			static void MaterialSizeStub2();

			static void AlignmentHook();

			static void AlignmentHook2();

			static void DecryptStream(char* data, std::size_t);
			static void EncryptedLoadStream();

		public:
			static std::int32_t ff_version;

			FFCompression();
			~FFCompression();
		};
	}
}
