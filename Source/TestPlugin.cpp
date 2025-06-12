#include <iostream>
#include <API/ARK/Ark.h>
#include "DinoColourCommand.h"
#include "DinoColourCommandConfig.h"
#include "DinoColourCommandCommands.h"

#pragma comment(lib, "AsaApi.lib")
#pragma comment(lib, "Permissions.lib")
#pragma comment(lib, "ArkShop.lib")
#pragma comment(lib, "TokenBank.lib")

// Declaración de la función para cargar la DLL
typedef void(*LoadFunction)();

void ReloadConfig(APlayerController* player_controller, FString* /*unused*/, bool /*unused*/)
{
    auto* shooter_controller = static_cast<AShooterPlayerController*>(player_controller);
    try
    {
        InitConfig();
        InitCommands();
    }
    catch (const std::exception& error)
    {
        AsaApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::Red, "Failed to reload config");

        Log::GetLog()->error(error.what());
        return;
    }

    AsaApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::Green, "Reloaded config");
}

void ReloadConfigRcon(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/)
{
    FString reply;
    try
    {
        InitConfig();
        InitCommands();
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error(error.what());

        reply = error.what();
        rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
        return;
    }

    reply = "Reloaded config";
    rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
}

// Función para cargar la DLL y llamar a su función de inicialización
void Load()
{
    Log::Get().Init("DinoColourCommand");
    try
    {
        InitConfig();
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error(error.what());
        throw;
    }

    try
    {
        InitCommands();
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error(error.what());
        throw;
    }

    try
    {
        AsaApi::GetCommands().AddConsoleCommand("DinoColourCommand.Reload", &ReloadConfig);
        AsaApi::GetCommands().AddRconCommand("DinoColourCommand.Reload", &ReloadConfigRcon);
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error(error.what());
        throw;
    }
}

void Unload()
{
    AsaApi::GetCommands().RemoveConsoleCommand("DinoColourCommand.Reload");
    AsaApi::GetCommands().RemoveRconCommand("DinoColourCommand.Reload");
    RemoveCommands();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Load();
        break;
    case DLL_PROCESS_DETACH:
        Unload();
        break;
    }
    return TRUE;
}