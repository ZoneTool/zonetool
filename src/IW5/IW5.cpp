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
		bool isDumpingComplete = false;
		bool isDumping = false;

		const char* Linker::Version()
		{
			return "IW5";
		}

		bool Linker::InUse()
		{
			return !strncmp(reinterpret_cast<char*>(0x770C6C), this->Version(), 3);
		}

		__declspec(naked) void Linker::MessageLoop()
		{
			while (true)
			{
			}
		}

		void Linker::Startup()
		{
			// AssetHandler::SetDump(true);

			if (this->InUse())
			{
				// Kill original console window
				Memory(0x53CB60).Set<std::uint8_t>(0xC3);
				Memory(0x53C850).Set<std::uint8_t>(0xC3);

				// Message loop
				Memory(0x53A2E0).Jump(MessageLoop);

				// Load patches
				RegisterPatch<FFCompression>();
				RegisterPatch<AssetHandler>();

				// Disable DW
				Memory(0x53821C).Nop(5);

				// Do not read server config, we don't need it anyways.
				Memory(0x4DAEFE).Nop(5);

				// Do not interact with the steamapi functions.
				Memory(0x599EAE).Nop(5);

				// Sys_Error -> printf
				Memory(0x539A50).Jump(printf);

				// Patch hunk size (Allows us to load more assets)
				Memory(0x5340D9).Set<DWORD>(0x3F000000);
				Memory(0x534110).Set<DWORD>(0x3F000000);

				// Replace the default attachment name (the specified default does not exist)
				Memory(0x74B0BC).WriteString("reflex");
			}
		}

		std::shared_ptr<IZone> Linker::AllocZone(std::string& zone)
		{
			// Patch main thread ID
			// Memory(0x01C11BDC).Set(GetCurrentThreadId());

			auto ptr = std::make_shared<Zone>(zone, this);
			return ptr;
		}

		std::shared_ptr<ZoneBuffer> Linker::AllocBuffer()
		{
			auto ptr = std::make_shared<ZoneBuffer>();
			ptr->init_streams(9);

			return ptr;
		}

		void Linker::LoadZone(std::string& name)
		{
			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());

			XZoneInfo zone = {name.data(), 20, 0};
			DB_LoadXAssets(&zone, 1, 0);

			auto timeout = 3000u;
			ZONETOOL_INFO("Waiting %u sec for zone \"%s\" to be loaded.", timeout / 1000, name.data());
			Sleep(timeout);
		}

		void Linker::UnloadZones()
		{
			ZONETOOL_INFO("Unloading zones...");

			static XZoneInfo zone = {"", 0, 20};
			DB_LoadXAssets(&zone, 1, 1);
		}

		bool Linker::IsValidAssetType(std::string& type)
		{
			return this->TypeToInt(type) >= 0;
		}

		std::int32_t Linker::TypeToInt(std::string type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x7C6208);

			for (std::int32_t i = 0; i < max; i++)
			{
				if (xassettypes[i] == type)
					return i;
			}

			return -1;
		}

		std::string Linker::TypeToString(std::int32_t type)
		{
			auto xassettypes = reinterpret_cast<char**>(0x7C6208);
			return xassettypes[type];
		}

		void Linker::DumpZone(std::string& name)
		{
			isDumpingComplete = false;
			isDumping = true;

			FileSystem::SetFastFile(name);
			AssetHandler::SetDump(true);
			LoadZone(name);

			while (!isDumpingComplete)
			{
				Sleep(1);
			}
		}

		void Linker::VerifyZone(std::string& name)
		{
			AssetHandler::SetVerify(true);
			LoadZone(name);
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
