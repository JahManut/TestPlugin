#include <json.hpp>

#include <API/ARK/Ark.h>
#include <Tools.h>
#include "ihooks.h"
#include "Ark/ArkApiUtils.h"
#include <Permissions.h>

#include <fstream>
#include <string>
#include <fmt/format.h>

#pragma comment(lib, "AsaApi.lib")
#pragma comment(lib, "Permissions.lib")
#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "mysqlclient.lib")

#define PROJECT_NAME "TestPlugin"

// Variables globales configurables
std::string dm_command = "/dm";  // comando por defecto, configurable desde JSON
std::string direct_message_template = "{0} recibió: '{1}' de {2}";
FLinearColor direct_message_color = FLinearColor(0.f, 1.f, 0.f, 1.f);

// ----------------------------------------
// Función para obtener la ruta de config.json
inline std::string GetConfigPath()
{
    return AsaApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/TestPlugin/config.json";
}

// ----------------------------------------
// Carga de config.json
void LoadConfig()
{
    try
    {
        std::ifstream file(GetConfigPath());
        if (!file.is_open())
        {
            Log::GetLog()->warn("No se pudo abrir config.json, se usará configuración por defecto.");
            return;
        }

        nlohmann::json config;
        file >> config;

        // Comando DM configurable
        dm_command = config.value("dm_command", dm_command);

        direct_message_template = config.value("direct_message_template", direct_message_template);
        if (config.contains("direct_message_color") && config["direct_message_color"].is_array()) {
            auto arr = config["direct_message_color"];
            direct_message_color.R = arr[0].get<float>();
            direct_message_color.G = arr[1].get<float>();
            direct_message_color.B = arr[2].get<float>();
            direct_message_color.A = arr[3].get<float>();
        }

        Log::GetLog()->info("Config cargado correctamente");
    }
    catch (const std::exception& ex)
    {
        Log::GetLog()->error("Error al cargar config.json: {}", ex.what());
    }
}

// ----------------------------------------
// Hook: enviar mensaje directo
DECLARE_HOOK(AShooterGameMode_SendServerDirectMessage, void,
    AShooterGameMode*, FString*, FString*, FLinearColor, bool, int, int, FString*, FString*);

void Hook_AShooterGameMode_SendServerDirectMessage(
    AShooterGameMode* _this,
    FString* PlayerId,
    FString* MessageText,
    FLinearColor OriginalColor,
    bool bIsBold,
    int ReceiverTeamId,
    int ReceiverPlayerID,
    FString* PlayerName,
    FString* SenderId)
{
    std::string eos_id = TCHAR_TO_UTF8(**PlayerId);
    std::string original = TCHAR_TO_UTF8(**MessageText);
    std::string sender_id = TCHAR_TO_UTF8(**SenderId);

    std::string formatted = fmt::format(
        direct_message_template,
        eos_id,
        original,
        sender_id
    );
    FString fmsg(formatted.c_str());

    AShooterGameMode_SendServerDirectMessage_original(
        _this,
        PlayerId,
        &fmsg,
        direct_message_color,
        bIsBold,
        ReceiverTeamId,
        ReceiverPlayerID,
        PlayerName,
        SenderId);
}

// ----------------------------------------
// Comando chat personalizado: '/dm <PlayerName> <Message>'
void DirectMessageCmd(AShooterPlayerController* caller, FString* fullCmd, int arg1, int arg2)
{
    TArray<FString> parts;
    fullCmd->ParseIntoArray(parts, TEXT(" "), true);

    if (parts.Num() < 3)
    {
        // 1) Construye el texto de ayuda en un FString
        FString usage = FString::Printf(
            TEXT("Uso: %s <PlayerName> <mensaje>"),
            UTF8_TO_TCHAR(dm_command.c_str())
        );

        // 2) Elige un FLinearColor para rojo:
        FLinearColor redLinear = FLinearColor(1.f, 0.f, 0.f, 1.f);
        //    — o usa `direct_message_color` si ya lo tienes configurado.

        // 3) Pasa *usage para obtener const TCHAR*
        AsaApi::GetApiUtils().SendServerMessage(
            caller,
            redLinear,
            *usage
        );
        return;
    }

    FString targetName = parts[1];
    FString msg;
    for (int32 i = 2; i < parts.Num(); ++i)
    {
        msg += parts[i];
        if (i < parts.Num() - 1)
            msg += TEXT(" ");
    }

    UWorld* world = AsaApi::GetApiUtils().GetWorld();
    if (!world) return;

    for (auto weakPC : world->PlayerControllerListField())
    {
        APlayerController* controller = weakPC.Get(false);
        if (!controller) continue;
        auto* pc = static_cast<AShooterPlayerController*>(controller);
        if (!pc) continue;

        if (AsaApi::IApiUtils::GetCharacterName(pc) == targetName)
        {
            AsaApi::GetApiUtils().SendServerMessage(pc, direct_message_color, *msg);
            break;
        }
    }
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


// 1) Prototype (forward-declaration) — debe ir **antes** de Plugin_Init:
void ConsoleReloadConfig(APlayerController* controller, FString* cmd, bool writeToLog);

// 2) Plugin_Init con el registro del comando:
extern "C" __declspec(dllexport) void Plugin_Init()
{
    Log::Get().Init(PROJECT_NAME);
    LoadConfig();
    // … tus hooks …

    AsaApi::GetCommands().AddChatCommand(dm_command.c_str(), &DirectMessageCmd);
    AsaApi::GetCommands().AddConsoleCommand(
        "TestPlugin.reload",
        &ConsoleReloadConfig
    );

    if (AsaApi::GetApiUtils().GetStatus() == AsaApi::ServerStatus::Ready)
        OnServerReady();
}

// 3) ÚNICA definición de ConsoleReloadConfig — **después** de Plugin_Init:
void ConsoleReloadConfig(APlayerController* controller, FString* /*cmd*/, bool writeToLog)
{
    try
    {
        LoadConfig();
        Log::GetLog()->info("[TestPlugin] Config recargado por consola.");

        if (controller)
            if (auto* pc = static_cast<AShooterPlayerController*>(controller))
                AsaApi::GetApiUtils().SendServerMessage(
                    pc,
                    FLinearColor(0.f, 1.f, 0.f, 1.f),
                    TEXT("TestPlugin: configuración recargada.")
                );
    }
    catch (const std::exception& ex)
    {
        Log::GetLog()->error("[TestPlugin] Error recargando config: {}", ex.what());
        if (controller)
            if (auto* pc = static_cast<AShooterPlayerController*>(controller))
                AsaApi::GetApiUtils().SendServerMessage(
                    pc,
                    FLinearColor(1.f, 0.f, 0.f, 1.f),
                    TEXT("TestPlugin: fallo al recargar configuración.")
                );
    }
}
