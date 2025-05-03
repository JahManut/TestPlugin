#include "API/ARK/Ark.h"
#include "format.h"
#include "fmt/format.h"


#include "AsaApi.TestPlugin.h"



#pragma comment(lib, "AsaApi.lib")

#define PROJECT_NAME "AsaApiTestPlugin"

void OnServerReady()
{
	Log::GetLog()->info("PluginTest Initialized");
}


DECLARE_HOOK(AShooterGameMode_BeginPlay, void, AShooterGameMode*);
void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _this)
{
	AShooterGameMode_BeginPlay_original(_this);

	OnServerReady();
}


extern "C" __declspec(dllexport) void Plugin_Init()
{
	Log::Get().Init(PROJECT_NAME);

	AsaApi::GetHooks().SetHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay, &AShooterGameMode_BeginPlay_original);

	if (AsaApi::GetApiUtils().GetStatus() == AsaApi::ServerStatus::Ready)
		OnServerReady();


}
extern "C" __declspec(dllexport) void Plugin_Unload()
{
	AsaApi::GetHooks().SetHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay, &AShooterGameMode_BeginPlay_original);
}
