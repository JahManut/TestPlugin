#include <API/ARK/Ark.h>
#include <Tools.h>
#include <format.h>

#pragma comment(lib, "AsaApi.lib")

#define PROJECT_NAME "SpawnMessagePlugin"

// HOOK para cuando el jugador spawnea
DECLARE_HOOK(AShooterGameMode_HandleNewPlayer, void, AShooterGameMode*, AShooterPlayerController*);


void Hook_AShooterGameMode_HandleNewPlayer(AShooterGameMode* _this, AShooterPlayerController* player_controller)

{
    if (!player_controller)
        return;

    FString player_name = AsaApi::IApiUtils::GetCharacterName(player_controller);
    FString message = FString("Hola, ") + player_name + FString("!");


    AsaApi::GetApiUtils().SendServerMessageToAll(FColorList::Green, *message);

    AShooterGameMode_HandleNewPlayer_original(_this, player_controller);
}

// INICIALIZACIÓN DEL PLUGIN
extern "C" __declspec(dllexport) void Plugin_Init()
{
    Log::Get().Init(PROJECT_NAME);
    AsaApi::GetHooks().SetHook("AShooterGameMode.HandleNewPlayer", Hook_AShooterGameMode_HandleNewPlayer, &AShooterGameMode_HandleNewPlayer_original);
}

extern "C" __declspec(dllexport) void Plugin_Unload()
{ 
    AsaApi::GetHooks().DisableHook("AShooterGameMode.HandleNewPlayer", Hook_AShooterGameMode_HandleNewPlayer);
}
