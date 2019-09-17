#include "stdafx.hpp"

std::string currentzone;

namespace ZoneTool
{
	std::vector<std::shared_ptr<ILinker>> linkers;
	std::map<std::string, std::function<void(std::vector<std::string>)>> commands;

	void RegisterCommand(std::string& name, std::function<void(std::vector<std::string>)> cb)
	{
		commands[name] = cb;
	}

	void ExecuteCommand(std::vector<std::string> args)
	{
		if (commands.find(args[0]) != commands.end())
		{
			commands[args[0]](args);
		}
		else
		{
			ZONETOOL_ERROR("Unknown command \"%s\".", args[0].data());
		}
	}

	void CommandThread()
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
			ExecuteCommand(args);
		}
	}

	void AddAssetsUsingIterator(const std::string& fastfile, const std::string& type, const std::string& folder,
	                            const std::string& extension, bool skip_reference, IZone* zone)
	{
		if (std::experimental::filesystem::is_directory("zonetool\\" + fastfile + "\\" + folder))
		{
			for (auto& file : std::experimental::filesystem::recursive_directory_iterator(
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
						zone->AddAssetOfType(type, filename);
					}
				}
			}
		}
	}

	void BuildZone(std::shared_ptr<ILinker>& linker, std::string& fastfile)
	{
		// make sure FS is correct.
		FileSystem::SetFastFile(fastfile);

		ZONETOOL_INFO("Building fastfile \"%s\" for game \"%s\"", fastfile.data(), linker->Version());

		auto& zone = linker->AllocZone(fastfile);
		if (zone == nullptr)
		{
			ZONETOOL_ERROR("An error occured while building fastfile \"%s\": Are you out of memory?", fastfile.data());
			return;
		}

		std::string path = "zone_source\\" + fastfile + ".csv";
		auto parser = CsvParser_new(path.data(), ",", false);

		if (!parser)
		{
			ZONETOOL_ERROR("Couldn't build zone \"%s\": CSV file does not exist!", fastfile.data());
			return;
		}

		bool isReferencing = false;
		auto row = CsvParser_getRow(parser);
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
				linker->LoadZone(std::string(row->fields_[1]));
			}
				// this allows us to reference assets instead of rewriting them
			else if (row->fields_[0] == "reference"s)
			{
				if (row->numOfFields_ >= 2)
				{
					isReferencing = (row->fields_[1] == "true"s) ? true : false;
				}
			}
				// this will use a directory iterator to automatically add assets
			else if (row->fields_[0] == "iterate"s)
			{
				AddAssetsUsingIterator(fastfile, "fx", "fx", ".fxe", true, zone.get());
				AddAssetsUsingIterator(fastfile, "xanimparts", "XAnim", ".xae", true, zone.get());
				AddAssetsUsingIterator(fastfile, "xmodel", "XModel", ".xme5", true, zone.get());

				// debugging purposes
				// AddAssetsUsingIterator(fastfile, "techset", "techsets", ".techset", true, zone.get());
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
					ZONETOOL_INFO("Adding lcoalized string to zone...");

					struct LocalizeStruct
					{
						const char* value;
						const char* name;
					};

					auto loc = new LocalizeStruct;
					loc->name = _strdup(row->fields_[1]);
					loc->value = _strdup(row->fields_[2]);

					auto type = zone->GetTypeByName(row->fields_[0]);
					if (type == -1)
					{
						ZONETOOL_ERROR("Could not translate typename %s to an integer!", row->fields_[0]);
					}
					zone->AddAssetOfTypePtr(type, loc);
				}
				else
				{
					if (row->numOfFields_ >= 2)
					{
						if (linker->IsValidAssetType(std::string(row->fields_[0])))
						{
							zone->AddAssetOfType(
								row->fields_[0],
								((isReferencing) ? ","s : ""s) + row->fields_[1]
							);
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

		// allocate zone buffer
		auto buffer = linker->AllocBuffer();

		// add branding asset
		zone->AddAssetOfType("rawfile", fastfile);

		// compile zone
		zone->Build(buffer);

		// unload fastfiles
		// linker->UnloadZones();
	}

	void CreateConsole()
	{
#ifdef USE_VMPROTECT
		VMProtectBeginUltra("CreateConsole");
#endif

		// Allocate console
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);

		// Set console name
		SetConsoleTitleA("ZoneTool");

		// Spawn command thread
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(CommandThread), nullptr, 0, nullptr);

		// Commands
		RegisterCommand("quit"s, [](std::vector<std::string>)
		{
			ExitProcess(0);
		});
		RegisterCommand("buildzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: buildzone <zone>");
				return;
			}

			// Pass build command on to linkers
			for (auto& linker : linkers)
			{
				if (!linker->InUse()) continue;
				BuildZone(linker, args[1]);
			}
		});
		RegisterCommand("loadzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: loadzone <zone>");
				return;
			}

			// Load zone
			for (auto& linker : linkers)
			{
				if (!linker->InUse()) continue;
				linker->LoadZone(args[1]);
			}
		});
		RegisterCommand("verifyzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: verifyzone <zone>");
				return;
			}

			// Load zone
			for (auto& linker : linkers)
			{
				if (!linker->InUse()) continue;
				linker->VerifyZone(args[1]);
			}
		});
		RegisterCommand("dumpzone"s, [](std::vector<std::string> args)
		{
			// Check if enough arguments have been passed to the command
			if (args.size() == 1)
			{
				ZONETOOL_ERROR("usage: dumpzone <zone>");
				return;
			}

			// Load zone
			for (auto& linker : linkers)
			{
				if (!linker->InUse()) continue;
				linker->DumpZone(args[1]);
			}
		});

#ifdef USE_VMPROTECT
		VMProtectEnd();
#endif
	}

	template <typename T>
	void RegisterLinker()
	{
		linkers.push_back(std::make_shared<T>());
	}

	void Branding(ILinker* linker)
	{
		ZONETOOL_INFO("ZoneTool initialization complete!");
		ZONETOOL_INFO("Welcome to ZoneTool r%i written by RektInator.", ZONETOOL_VERSION);
		ZONETOOL_INFO("Special thanks to: NTAuthority, momo5502, TheApadayo, localhost & X3RX35.");

		if (linker)
		{
			ZONETOOL_INFO("Initializing linker for game \"%s\"...\n", linker->Version());
		}
	}

	std::vector<std::string> GetCommandLineArguments()
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

	std::shared_ptr<ILinker> currentLinker;

	void HandleParams()
	{
		// Wait until the game is loaded
		Sleep(5000);

		// Execute command line commands?
		auto& args = GetCommandLineArguments();
		if (args.size() > 1)
		{
			for (auto i = 0u; i < args.size(); i++)
			{
				if (i < args.size() - 1)
				{
					if (args[i] == "-buildzone")
					{
						BuildZone(currentLinker, args[i + 1]);
						i++;
					}
					else if (args[i] == "-loadzone")
					{
						currentLinker->LoadZone(args[i + 1]);
						i++;
					}
					else if (args[i] == "-dumpzone")
					{
						currentLinker->DumpZone(args[i + 1]);
						i++;
					}
				}
			}

			std::exit(0);
		}
	}

	void Startup()
	{
#ifdef USE_VMPROTECT
		VMProtectBeginUltra("Startup");
#endif

		// Create stdout console
		CreateConsole();

		// Register linkers
		RegisterLinker<IW3::Linker>();
		RegisterLinker<IW4::Linker>();
		RegisterLinker<IW5::Linker>();

		// Startup compatible linkers
		for (auto& linker : linkers)
		{
			linker->Startup();

			if (linker->InUse())
			{
				currentLinker = linker;
			}
		}

		// Startup complete, show branding
		Branding(currentLinker.get());

		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HandleParams, nullptr, 0, nullptr);

#ifdef USE_VMPROTECT
		VMProtectEnd();
#endif
	}
}
