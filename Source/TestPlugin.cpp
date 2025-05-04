#include <API/ARK/Ark.h>
#include <Tools.h>

#include <fstream>
#include <string>
#include <json.hpp>
#include <fmt/format.h>
#include <format.h>

#pragma comment(lib, "AsaApi.lib")

#define PROJECT_NAME "TestPlugin"

// Variables globales configurables
std::string welcome_message = "¡Bienvenido, {}!";
FColor message_color = FColorList::Green;

// ----------------------------------------
// Función para convertir string a FColor
FColor GetColorFromName(const std::string& color_name)
{
    if (color_name == "red") return FColorList::Red;
    if (color_name == "green") return FColorList::Green;
    if (color_name == "blue") return FColorList::Blue;
    if (color_name == "yellow") return FColorList::Yellow;
    if (color_name == "orange") return FColorList::Orange;
    if (color_name == "white") return FColorList::White;
    if (color_name == "black") return FColorList::Black;
    if (color_name == "cyan") return FColorList::Cyan;
    if (color_name == "magenta") return FColorList::Magenta;

    return FColorList::Green; // por defecto
}

// ----------------------------------------
// Carga de config.json
void LoadConfig()
{
    try
    {
        const std::string config_path = AsaApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/TestPlugin/Config/config.json";
        std::ifstream file(config_path);
        if (!file.is_open())
        {
            Log::GetLog()->warn("No se pudo abrir config.json, se usará configuración por defecto.");
            return;
        }

        nlohmann::json config;
        file >> config;

        if (config.contains("welcome_message"))
        {
            welcome_message = config["welcome_message"].get<std::string>();
            Log::GetLog()->info("Mensaje cargado: {}", welcome_message);
        }

        if (config.contains("message_color"))
        {
            std::string color_str = config["message_color"].get<std::string>();
            message_color = GetColorFromName(color_str);
            Log::GetLog()->info("Color del mensaje: {}", color_str);
        }
    }
    catch (const std::exception& ex)
    {
        Log::GetLog()->error("Error al cargar config.json: {}", ex.what());
    }
}

// ----------------------------------------
// Hook: jugador entra al mundo
DECLARE_HOOK(AShooterPlayerController_ServerRequestJoinWorld, void, AShooterPlayerController*);

void Hook_AShooterPlayerController_ServerRequestJoinWorld(AShooterPlayerController* _this)
{
    if (!_this)
        return;

    FString f_player_name = AsaApi::IApiUtils::GetCharacterName(_this);
    std::string player_name = TCHAR_TO_UTF8(*f_player_name);

    std::string formatted = fmt::format(welcome_message, player_name);
    FString message(formatted.c_str());

    AsaApi::GetApiUtils().SendServerMessage(_this, message_color, *message);

    AShooterPlayerController_ServerRequestJoinWorld_original(_this);
}

// ----------------------------------------
// Hook: OnServerReady (BeginPlay)
void OnServerReady()
{
    Log::GetLog()->info("TestPlugin Initialized");
    LoadConfig();
}

DECLARE_HOOK(AShooterGameMode_BeginPlay, void, AShooterGameMode*);
void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _this)
{
    AShooterGameMode_BeginPlay_original(_this);
    OnServerReady();
}

// ----------------------------------------
// INIT y UNLOAD del plugin
extern "C" __declspec(dllexport) void Plugin_Init()
{
    Log::Get().Init(PROJECT_NAME);

    AsaApi::GetHooks().SetHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay, &AShooterGameMode_BeginPlay_original);
    AsaApi::GetHooks().SetHook("AShooterPlayerController.ServerRequestJoinWorld", Hook_AShooterPlayerController_ServerRequestJoinWorld, &AShooterPlayerController_ServerRequestJoinWorld_original);


    if (AsaApi::GetApiUtils().GetStatus() == AsaApi::ServerStatus::Ready)
    {
        OnServerReady();
    }
}

extern "C" __declspec(dllexport) void Plugin_Unload()
{
    AsaApi::GetHooks().DisableHook("AShooterGameMode.BeginPlay()", &AShooterGameMode_BeginPlay_original);
    AsaApi::GetHooks().DisableHook("AShooterPlayerController.ServerRequestJoinWorld", &AShooterPlayerController_ServerRequestJoinWorld_original);

    Log::GetLog()->info("TestPlugin Unloaded");
}
