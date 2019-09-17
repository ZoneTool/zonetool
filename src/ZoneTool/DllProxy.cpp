#include "stdafx.hpp"

#undef LoadLibrary

// Macro to declare an export
// --------------------------------------+
#define EXPORT(_export) extern "C" __declspec(naked) __declspec(dllexport) void _export() \
{ \
	SDLLP::GetExport(__FUNCTION__, LIBRARY); \
	__asm { jmp eax } \
}

// Static class
// --------------------------------------+
class SDLLP
{
private:
	static std::map<std::string, HINSTANCE> mLibraries;

	static void Log(const char* message, ...);
	static void LoadLibrary(const char* library);
	static bool IsLibraryLoaded(const char* library);

public:
	static FARPROC GetExport(const char* function, const char* library);
};

//	Class variable declarations
// --------------------------------------+
std::map<std::string, HINSTANCE> SDLLP::mLibraries;

// Load necessary library
// --------------------------------------+
void SDLLP::LoadLibrary(const char* library)
{
	CHAR mPath[MAX_PATH];

	GetSystemDirectoryA(mPath, MAX_PATH);
	strcat_s(mPath, "\\");
	strcat_s(mPath, library);

	mLibraries[library] = LoadLibraryA(mPath);
}

// Check if export already loaded
// --------------------------------------+
bool SDLLP::IsLibraryLoaded(const char* library)
{
	return (mLibraries.find(library) != mLibraries.end() && mLibraries[library]);
}

// Get export address
// --------------------------------------+
FARPROC SDLLP::GetExport(const char* function, const char* library)
{
	if (!IsLibraryLoaded(library)) LoadLibrary(library);
	FARPROC address = GetProcAddress(mLibraries[library], function);
	return address;
}

// Write debug string
// --------------------------------------+
void SDLLP::Log(const char* message, ...)
{
	CHAR buffer[1024];
	va_list ap;

	va_start(ap, message);
	vsprintf(buffer, message, ap);
	va_end(ap);

	OutputDebugStringA(buffer);
}

// --------------------------------------+
//	Adapt export functions and library
// --------------------------------------+

namespace
{
	// steam
#define LIBRARY "steam_api.dll"
	EXPORT(SteamAPI_Init)
	EXPORT(SteamAPI_Shutdown)
	EXPORT(SteamAPI_RestartAppIfNecessary)
	EXPORT(SteamAPI_ReleaseCurrentThreadMemory)
	EXPORT(SteamAPI_WriteMiniDump)
	EXPORT(SteamAPI_SetMiniDumpComment)
	EXPORT(SteamAPI_UnregisterCallback)
	EXPORT(SteamAPI_UnregisterCallResult)
	EXPORT(SteamAPI_RegisterCallback)
	EXPORT(SteamAPI_RegisterCallResult)
	EXPORT(SteamAPI_RunCallbacks)
	EXPORT(SteamAPI_IsSteamRunning)
	EXPORT(SteamClient)
	EXPORT(SteamUser)
	EXPORT(SteamFriends)
	EXPORT(SteamUtils)
	EXPORT(SteamMatchmaking)
	EXPORT(SteamUserStats)
	EXPORT(SteamApps)
	EXPORT(SteamNetworking)
	EXPORT(SteamMasterServerUpdater)
	EXPORT(SteamMatchmakingServers)
	EXPORT(SteamRemoteStorage)
	EXPORT(SteamScreenshots)
	EXPORT(SteamHTTP)
	EXPORT(SteamUnifiedMessages)
	EXPORT(SteamController)
	EXPORT(SteamUGC)
	EXPORT(SteamAppList)
	EXPORT(SteamMusic)
	EXPORT(SteamMusicRemote)
	EXPORT(SteamHTMLSurface)
	EXPORT(SteamInventory)
	EXPORT(SteamVideo)
	EXPORT(SteamGameServerNetworking)
	EXPORT(SteamGameServerUtils)
	EXPORT(SteamGameServer)
	EXPORT(SteamGameServer_Init)
	EXPORT(SteamGameServer_Shutdown)
	EXPORT(SteamGameServer_RunCallbacks)

	// d3d9
#define LIBRARY "d3d9.dll"
	EXPORT(Direct3DShaderValidatorCreate9)
	EXPORT(PSGPError)
	EXPORT(PSGPSampleTexture)
	EXPORT(D3DPERF_BeginEvent)
	EXPORT(D3DPERF_EndEvent)
	EXPORT(D3DPERF_GetStatus)
	EXPORT(D3DPERF_QueryRepeatFrame)
	EXPORT(D3DPERF_SetMarker)
	EXPORT(D3DPERF_SetOptions)
	EXPORT(D3DPERF_SetRegion)
	EXPORT(DebugSetLevel)
	EXPORT(DebugSetMute)
	EXPORT(Direct3DCreate9)
	EXPORT(Direct3DCreate9Ex)
}
