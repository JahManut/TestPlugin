#include "API/ARK/Ark.h"

#include "AsaApi.TestPlugin.h"


#pragma comment(lib, "AsaApi.lib")

void OnServerReady()
{
	Log::GetLog()->info("PluginTest Initialized");
}

DECLARE_HOOK(ASHooterGameMode_BeginPlay, void, AShooterGameMode*);
void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _this)
{
	AShoteerGameMode_BeginPlay_original(_this);

	OnServerReady();
}

extern "C" __declspec(dllexport) void Plugin_Init()
{
	Log::Get().Init(PROJECT_NAME)
}

extern "C" __declspec(dllexport) void Plugin_Unload()
{
	AsaApi::GetHooks().DisableHook("AShooterGameMode.BeginPlay()", Hook_ASHooterGameMode_BeginPlay);
}

DECLARE_HOOK(ASHooterGameMode_BeginPlay, void, AShooterGameMode*)
{
}
