// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include <Dbghelp.h>

// include zonetool linkers
#include <IW3/IW3.hpp>
#include <IW4/IW4.hpp>
#include <IW5/IW5.hpp>
#include <CODO/CODO.hpp>
#include "Utils/Swizzle.hpp"

#pragma comment(lib, "Dbghelp")

std::string currentzone;

namespace ZoneTool
{
	std::vector<std::shared_ptr<ILinker>> linkers;
	std::map<std::string, std::function<void(std::vector<std::string>)>> commands;

	void register_command(const std::string& name, std::function<void(std::vector<std::string>)> cb)
	{
		commands[name] = cb;
	}

	void execute_command(std::vector<std::string> args)
	{
		const auto itr = commands.find(args[0]);
		if (itr != commands.end())
		{
			itr->second(args);
		}
		else
		{
			ZONETOOL_ERROR("Unknown command \"%s\".", args[0].data());
		}
	}

	void command_thread()
	{
		while (true)
		{
			// Get console input
			std::string input;
			std::getline(std::cin, input);

			std::vector<std::string> args;

			// Load arguments into vector
			if (input.find(' ') != std::string::npos)
			{
				args = split(input, ' ');
			}
			else
			{
				args.push_back(input);
			}

			// Execute command
			execute_command(args);
		}
	}

	void add_assets_using_iterator(const std::string& fastfile, const std::string& type, const std::string& folder,
	                            const std::string& extension, bool skip_reference, IZone* zone)
	{
		if (std::filesystem::is_directory("zonetool\\" + fastfile + "\\" + folder))
		{
			for (auto& file : std::filesystem::recursive_directory_iterator(
				     "zonetool\\" + fastfile + "\\" + folder))
			{
				if (is_regular_file(file))
				{
					auto filename = file.path().filename().string();

					if (skip_reference && filename[0] == ',')
					{
						// skip this file
						continue;
					}

					// check if the filename contains the correct extension
					if (filename.length() > extension.length() &&
						filename.substr(filename.length() - extension.length()) == extension)
					{
						// remove the extension
						filename = filename.substr(0, filename.length() - extension.length());

						// add asset to disk
						zone->add_asset_of_type(type, filename);
					}
				}
			}
		}
	}

	void parse_csv_file(ILinker* linker, IZone* zone, const std::string& fastfile, const std::string& csv_file)
	{
		auto path = "zone_source\\" + csv_file + ".csv";
		auto* parser = CsvParser_new(path.data(), ",", false);

		if (!parser)
		{
			ZONETOOL_ERROR("Could not find csv file \"%s\" to build zone!", csv_file.data());
			return;
		}

		auto is_referencing = false;
		auto* row = CsvParser_getRow(parser);
		while (row != nullptr)
		{
			// parse options
			if ((strlen(row->fields_[0]) >= 1 && row->fields_[0][0] == '#') || (strlen(row->fields_[0]) >= 2 && row->
				fields_[0][0] == '/' && row->fields_[0][1] == '/'))
			{
				// comment line, go to next line.
				goto nextRow;
			}
			if (!strlen(row->fields_[0]))
			{
				// empty line, go to next line.
				goto nextRow;
			}
			if (row->fields_[0] == "require"s)
			{
				linker->load_zone(row->fields_[1]);
			}
			else if (row->fields_[0] == "include"s)
			{
				parse_csv_file(linker, zone, fastfile, row->fields_[1]);
			}
			// 
			else if (row->fields_[0] == "target"s)
			{
				if (row->fields_[1] == "xbox360"s)
				{
					zone->set_target(zone_target::xbox360);
				}
				else if (row->fields_[1] == "ps3"s)
				{
					zone->set_target(zone_target::ps3);
				}
				else if (row->fields_[1] == "pc"s)
				{
					zone->set_target(zone_target::pc);
				}
				else
				{
					ZONETOOL_ERROR("Invalid zone target \"%s\"!", row->fields_[1]);
				}
			}
			//
			else if (row->fields_[0] == "target_version"s)
			{
				auto found_version = false;
				for (auto i = 0u; i < static_cast<std::size_t>(zone_target_version::max); i++)
				{
					if (zone_target_version_str[i] == row->fields_[1])
					{
						const auto target_version = static_cast<zone_target_version>(i);
						if (!linker->supports_version(target_version))
						{
							ZONETOOL_FATAL("Current linker (%s) does not support target version %s.", linker->version(), row->fields_[1]);
						}

						zone->set_target_version(target_version);
						found_version = true;
					}
				}

				if (!found_version)
				{
					ZONETOOL_FATAL("Invalid target version \"%s\".", row->fields_[1]);
				}
			}
			// this allows us to reference assets instead of rewriting them
			else if (row->fields_[0] == "reference"s)
			{
				if (row->numOfFields_ >= 2)
				{
					is_referencing = row->fields_[1] == "true"s;
				}
			}
			// add assets that are required for maps
			else if (row->fields_[0] == "map"s)
			{
				zone->add_asset_of_type("techset", "wc_l_hsm_r0c0n0s0");
			}
			// this will use a directory iterator to automatically add assets
			else if (row->fields_[0] == "iterate"s)
			{
				try
				{
					add_assets_using_iterator(fastfile, "fx", "fx", ".fxe", true, zone);
					add_assets_using_iterator(fastfile, "xanimparts", "XAnim", ".xae2", true, zone);
					add_assets_using_iterator(fastfile, "xmodel", "XModel", ".xme6", true, zone);
				}
				catch (std::exception& ex)
				{
					ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: %s\n", fastfile.data(), ex.what());
				}
			}
			// this will force external assets to be used
			else if (row->fields_[0] == "forceExternalAssets"s)
			{
				ZONETOOL_WARNING("forceExternalAssets has been turned on!");
				FileSystem::ForceExternalAssets(true);
			}
			// if entry is not an option, it should be an asset.
			else
			{
				if (row->fields_[0] == "localize"s && row->numOfFields_ >= 3)
				{
					ZONETOOL_INFO("Adding localized string to zone...");

					struct LocalizeStruct
					{
						const char* value;
						const char* name;
					};

					auto loc = new LocalizeStruct;
					loc->name = _strdup(row->fields_[1]);
					loc->value = _strdup(row->fields_[2]);

					auto type = zone->get_type_by_name(row->fields_[0]);
					if (type == -1)
					{
						ZONETOOL_ERROR("Could not translate typename %s to an integer!", row->fields_[0]);
					}

					try
					{
						zone->add_asset_of_type_by_pointer(type, loc);
					}
					catch (std::exception& ex)
					{
						ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: %s\n", fastfile.data(), ex.what());
					}
				}
				else
				{
					if (row->numOfFields_ >= 2)
					{
						if (linker->is_valid_asset_type(row->fields_[0]))
						{
							try
							{
								zone->add_asset_of_type(
									row->fields_[0],
									((is_referencing) ? ","s : ""s) + row->fields_[1]
								);
							}
							catch (std::exception& ex)
							{
								ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: %s\n", fastfile.data(), ex.what());
							}
						}
					}
				}
			}

		nextRow:
			// destroy row and alloc next one.
			CsvParser_destroy_row(row);
			row = CsvParser_getRow(parser);
		}

		// free csv parser
		CsvParser_destroy(parser);
	}
	
	void build_zone(ILinker* linker, const std::string& fastfile)
	{
		// make sure FS is correct.
		FileSystem::SetFastFile(fastfile);

		ZONETOOL_INFO("Building fastfile \"%s\" for game \"%s\"", fastfile.data(), linker->version());

		auto zone = linker->alloc_zone(fastfile);
		if (zone == nullptr)
		{
			ZONETOOL_ERROR("An error occured while building fastfile \"%s\": Are you out of memory?", fastfile.data());
			return;
		}

		// set default zone target to PC
		zone->set_target(zone_target::pc);

		if (linker->version() == "IW4"s)
		{
			zone->set_target_version(zone_target_version::iw4_release);
		}
		else if (linker->version() == "IW5"s)
		{
			zone->set_target_version(zone_target_version::iw5_release);
		}
		
		parse_csv_file(linker, zone.get(), fastfile, fastfile);

		// allocate zone buffer
		auto buffer = linker->alloc_buffer();

		// add branding asset
		zone->add_asset_of_type("rawfile", fastfile);

		// compile zone
		zone->build(buffer.get());

		// unload fastfiles
		// linker->UnloadZones();
	}

	ILinker* current_linker;

	LONG NTAPI exception_handler(_EXCEPTION_POINTERS* info)
	{
		if (info->ExceptionRecord->ExceptionCode == STATUS_INTEGER_OVERFLOW ||
			info->ExceptionRecord->ExceptionCode == STATUS_FLOAT_OVERFLOW ||
			info->ExceptionRecord->ExceptionCode == 0x406D1388 || 
			info->ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT)
		{
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		
		if (ZONETOOL_VERSION == "0.0.0"s)
		{
			MessageBoxA(nullptr, va("An exception occured (0x%08X) and ZoneTool must be restarted to continue. However, ZoneTool has detected that you are using a custom DLL. If you want to submit an issue, try to reproduce the bug with the latest release of ZoneTool. The latest version can be found here: https://github.com/ZoneTool/zonetool/releases", info->ExceptionRecord->ExceptionCode).data(), "ZoneTool", MB_ICONERROR);
			std::exit(0);
		}
		
		std::filesystem::create_directories("zonetool/crashdumps");

		const auto exception_time = std::time(nullptr);
		const auto linker_name = (current_linker) ? current_linker->version() : "unknown";
		const auto file_name = va("zonetool/crashdumps/zonetool-exception-%s-%s-%llu.dmp", linker_name, ZONETOOL_VERSION, exception_time);
		
		DWORD dump_type = MiniDumpIgnoreInaccessibleMemory;
		dump_type |= MiniDumpWithHandleData;
		dump_type |= MiniDumpScanMemory;
		dump_type |= MiniDumpWithProcessThreadData;
		dump_type |= MiniDumpWithFullMemoryInfo;
		dump_type |= MiniDumpWithThreadInfo;
		dump_type |= MiniDumpWithCodeSegs;
		dump_type |= MiniDumpWithDataSegs;
		
		const DWORD file_share = FILE_SHARE_READ | FILE_SHARE_WRITE;
		const HANDLE file_handle = CreateFileA(file_name.data(), GENERIC_WRITE | GENERIC_READ, file_share, nullptr, (file_share & FILE_SHARE_WRITE) > 0 ? OPEN_ALWAYS : OPEN_EXISTING, NULL, nullptr);
		MINIDUMP_EXCEPTION_INFORMATION ex = { GetCurrentThreadId(), info, FALSE };
		if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle, static_cast<MINIDUMP_TYPE>(dump_type), &ex, nullptr, nullptr))
		{
			
		}

		const auto message = va("An exception occured and ZoneTool must be restarted to continue. If this keeps happening, create an issue on https://github.com/ZoneTool/zonetool with the crashdump attached. The crashdump can be found at: \"%s\".", file_name.data());
		MessageBoxA(nullptr, message.data(), "ZoneTool", MB_ICONERROR);
		std::exit(0);
	}
	
	void create_console()
	{
#ifdef USE_VMPROTECT
		VMProtectBeginUltra("CreateConsole");
#endif

		if (!IsDebuggerPresent())
		{
			// Catch exceptions
			AddVectoredExceptionHandler(TRUE, exception_handler);
		}
		
		// Allocate console
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);

		// Set console name
		SetConsoleTitleA("ZoneTool");

		// Spawn command thread
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(command_thread), nullptr, 0, nullptr);

		// Commands
		register_command("quit"s, [](std::vector<std::string>)
		{
			ExitProcess(0);
		});
		register_command("buildzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: buildzone <zone>");
				return;
			}

			if (current_linker)
			{
				if(current_linker->supports_building())
				{
					build_zone(current_linker, args[1]);
				}
				else
				{
					ZONETOOL_ERROR("Current linker does not support zone building.");
				}
			}
		});
		register_command("loadzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: loadzone <zone>");
				return;
			}

			// Load zone
			if (current_linker)
			{
				current_linker->load_zone(args[1]);
			}
		});
		register_command("verifyzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: verifyzone <zone>");
				return;
			}

			// Load zone
			if (current_linker)
			{
				current_linker->verify_zone(args[1]);
			}
		});
		register_command("dumpzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: dumpzone <zone>");
				return;
			}

			// Load zone
			if (current_linker)
			{
				current_linker->dump_zone(args[1]);
			}
		});

#ifdef USE_VMPROTECT
		VMProtectEnd();
#endif
	}

	template <typename T>
	void register_linker()
	{
		linkers.push_back(std::make_shared<T>());
	}

	void branding(ILinker* linker)
	{
		ZONETOOL_INFO("ZoneTool initialization complete!");
		ZONETOOL_INFO("Welcome to ZoneTool v" ZONETOOL_VERSION " written by RektInator.");
		ZONETOOL_INFO("  \"No matter how hard or unlikely, if it's possible, it will be done.\"");
		ZONETOOL_INFO("Special thanks to: Laupetin, NTAuthority, momo5502, TheApadayo, localhost, X3RX35 & homura.");

		if (linker)
		{
			ZONETOOL_INFO("Initializing linker for game \"%s\"...\n", linker->version());
		}
		else
		{
			ZONETOOL_ERROR("No linker could be found for the current binary!\n");
		}
	}

	std::vector<std::string> get_command_line_arguments()
	{
		LPWSTR* szArglist;
		int nArgs;

		szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

		std::vector<std::string> args;
		args.resize(nArgs);

		// convert all args to std::string
		for (int i = 0; i < nArgs; i++)
		{
			auto curArg = std::wstring(szArglist[i]);
			args[i] = std::string(curArg.begin(), curArg.end());
		}

		// return arguments
		return args;
	}

	void handle_params()
	{
		// Wait until the game is loaded
		Sleep(5000);

		// Execute command line commands?
		auto args = get_command_line_arguments();
		if (args.size() > 1)
		{
			for (auto i = 0u; i < args.size(); i++)
			{
				if (i < args.size() - 1)
				{
					if (args[i] == "-buildzone")
					{
						build_zone(current_linker, args[i + 1]);
						i++;
					}
					else if (args[i] == "-loadzone")
					{
						current_linker->load_zone(args[i + 1]);
						i++;
					}
					else if (args[i] == "-dumpzone")
					{
						current_linker->dump_zone(args[i + 1]);
						i++;
					}
				}
			}

			std::exit(0);
		}
	}

	bool is_custom_linker_present()
	{
		return std::filesystem::exists("linker.dll") && std::filesystem::is_regular_file("linker.dll");
	}

	void startup()
	{
#ifdef USE_VMPROTECT
		VMProtectBeginUltra("Startup");
#endif

		// Create stdout console
		create_console();

		// Register linkers
		register_linker<IW3::Linker>();
		register_linker<IW4::Linker>();
		register_linker<IW5::Linker>();
		register_linker<CODO::Linker>();

		// check if a custom linker is present in the current game directory
		if (is_custom_linker_present())
		{
			const auto linker_module = LoadLibraryA("linker.dll");

			if (linker_module != nullptr && linker_module != INVALID_HANDLE_VALUE)
			{
				const auto get_linker_func = GetProcAddress(linker_module, "GetLinker");

				if (get_linker_func != nullptr && get_linker_func != INVALID_HANDLE_VALUE)
				{
					current_linker = Function<ILinker * ()>(get_linker_func)();
				}
			}
		}

		if (!current_linker)
		{
			// Startup compatible linkers
			for (auto& linker : linkers)
			{
				linker->startup();

				if (linker->is_used())
				{
					current_linker = linker.get();
				}
			}
		}

		// Startup complete, show branding
		branding(current_linker);

		// handle startup commands
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(handle_params), nullptr, 0, nullptr);

#ifdef USE_VMPROTECT
		VMProtectEnd();
#endif
	}
}
