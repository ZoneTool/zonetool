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
		bool is_dumping_complete = false;
		bool is_dumping = false;

		const char* Linker::version()
		{
			return "IW5";
		}

		bool Linker::is_used()
		{
			return !strncmp(reinterpret_cast<char*>(0x770C6C), this->version(), 3);
		}

		__declspec(naked) void Linker::MessageLoop()
		{
			while (true)
			{
			}
		}

		void Linker::startup()
		{
			// AssetHandler::SetDump(true);

			if (this->is_used())
			{
				// Kill original console window
				Memory(0x53CB60).set<std::uint8_t>(0xC3);
				Memory(0x53C850).set<std::uint8_t>(0xC3);

				// Message loop
				Memory(0x53A2E0).jump(MessageLoop);

				// Load patches
				RegisterPatch<FFCompression>();
				RegisterPatch<AssetHandler>();

				// Disable DW
				Memory(0x53821C).nop(5);

				// Do not read server config, we don't need it anyways.
				Memory(0x4DAEFE).nop(5);

				// Do not interact with the steamapi functions.
				Memory(0x599EAE).nop(5);

				// Sys_Error -> printf
				Memory(0x539A50).jump(printf);

				// Patch hunk size (Allows us to load more assets)
				Memory(0x5340D9).set<DWORD>(0x3F000000);
				Memory(0x534110).set<DWORD>(0x3F000000);

				// Replace the default attachment name (the specified default does not exist)
				Memory(0x74B0BC).write_string("reflex");
			}
		}

		std::shared_ptr<IZone> Linker::alloc_zone(const std::string& zone)
		{
			// Patch main thread ID
			// Memory(0x01C11BDC).Set(GetCurrentThreadId());

			auto ptr = std::make_shared<Zone>(zone, this);
			return ptr;
		}

		std::shared_ptr<ZoneBuffer> Linker::alloc_buffer()
		{
			auto ptr = std::make_shared<ZoneBuffer>();
			ptr->init_streams(9);

			return ptr;
		}

		void Linker::load_zone(const std::string& name)
		{
			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());

			XZoneInfo zone = {name.data(), 20, 0};
			DB_LoadXAssets(&zone, 1, 0);

			auto timeout = 3000u;
			ZONETOOL_INFO("Waiting %u sec for zone \"%s\" to be loaded.", timeout / 1000, name.data());
			Sleep(timeout);
		}

		void Linker::unload_zones()
		{
			ZONETOOL_INFO("Unloading zones...");

			static XZoneInfo zone = {"", 0, 20};
			DB_LoadXAssets(&zone, 1, 1);
		}

		bool Linker::is_valid_asset_type(std::string& type)
		{
			return this->type_to_int(type) >= 0;
		}

		std::int32_t Linker::type_to_int(std::string type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x7C6208);

			for (std::int32_t i = 0; i < max; i++)
			{
				if (xassettypes[i] == type)
					return i;
			}

			return -1;
		}

		std::string Linker::type_to_string(std::int32_t type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x7C6208);
			return xassettypes[type];
		}

        bool Linker::supports_building()
        {
            return true;
        }

		bool Linker::supports_version(const zone_target_version version)
		{
			return version == zone_target_version::iw5_release;
		}

        void Linker::dump_zone(const std::string& name)
		{
			is_dumping_complete = false;
			is_dumping = true;

			FileSystem::SetFastFile(name);
			AssetHandler::SetDump(true);
			load_zone(name);

			while (!is_dumping_complete)
			{
				Sleep(1);
			}
		}

		void Linker::verify_zone(const std::string& name)
		{
			AssetHandler::SetVerify(true);
			load_zone(name);
			AssetHandler::SetVerify(false);
		}

		Linker::Linker()
		{
		}

		Linker::~Linker()
		{
		}
	}
}
