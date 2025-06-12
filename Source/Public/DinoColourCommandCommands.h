#pragma once
#include <API/ARK/Ark.h>
#include <iostream>
#include <Permissions.h>
#include <IApiUtils.h>
#include <Tokens.h>
#include <points.h>
#include <fmt/format.h>

inline void SetDinoColour(AShooterPlayerController* player, AActor* Actor, int Region, int Colour)
{
	const FString eos_id = AsaApi::IApiUtils::GetEOSIDFromController(player);
	if (Actor && Actor->IsA(APrimalDinoCharacter::GetPrivateStaticClass()))
	{
		APrimalDinoCharacter* Dino = static_cast<APrimalDinoCharacter*>(Actor);
		if (Dino && !Dino->IsDead())
		{
			AShooterPlayerState* ASPS = static_cast<AShooterPlayerState*>(player->PlayerStateField().Get());
			if (ASPS)
			{
				if (RequireLvl)
				{
					int DinoLvl = Dino->GetCharacterStatusComponent()->GetCharacterLevel();
					if (DinoLvl < MinLvl)
					{
						AsaApi::GetApiUtils().SendNotification(player, FLinearColor(DinoLevelError["Color"][0] / 255.0f, DinoLevelError["Color"][1] / 255.0f, DinoLevelError["Color"][2] / 255.0f), static_cast<float>(DinoLevelError["TextSize"]), int(DinoLevelError["DisplayTime"]), nullptr,
							*FString(AsaApi::Tools::Utf8Decode(DinoLevelError["Message"])));
						return;
					}
				}
				if (Dino->TargetingTeamField() != player->TargetingTeamField())
				{
					AsaApi::GetApiUtils().SendNotification(player, FLinearColor(TribeError["Color"][0] / 255.0f, TribeError["Color"][1] / 255.0f, TribeError["Color"][2] / 255.0f), static_cast<float>(TribeError["TextSize"]), int(TribeError["DisplayTime"]), nullptr,
						*FString(AsaApi::Tools::Utf8Decode(TribeError["Message"])));
					return;
				}
				if (ArkShopEnable && !Permissions::IsPlayerHasPermission(eos_id, PermissionArkShopFree))
				{
					int playerCoins = ArkShop::Points::GetPoints(eos_id);
					if (playerCoins < ShopCost)
					{
						AsaApi::GetApiUtils().SendNotification(player, FLinearColor(NoPoints["Color"][0] / 255.0f, NoPoints["Color"][1] / 255.0f, NoPoints["Color"][2] / 255.0f), static_cast<float>(NoPoints["TextSize"]), int(NoPoints["DisplayTime"]), nullptr,
							*FString(AsaApi::Tools::Utf8Decode(NoPoints["Message"])));
						return;
					}
					ArkShop::Points::SpendPoints(ShopCost, eos_id);
				}
				if (TokenBankEnable && !Permissions::IsPlayerHasPermission(eos_id, PermissionTokenBankFree))
				{
					int getTokens = TokenBank::Tokens::GetTokens(eos_id, TBType.ToString());
					if (getTokens >= TBCost)
					{
						bool setSpendTokens = TokenBank::Tokens::SpendTokens(TBCost, eos_id, TBType.ToString());
						if (!setSpendTokens)
						{
							AsaApi::GetApiUtils().SendNotification(player, FLinearColor(NoTokens["Color"][0] / 255.0f, NoTokens["Color"][1] / 255.0f, NoTokens["Color"][2] / 255.0f), static_cast<float>(NoTokens["TextSize"]), int(NoTokens["DisplayTime"]), nullptr,
								*FString(AsaApi::Tools::Utf8Decode(NoTokens["Message"])));
							return;
						}
					}
					else
					{
						AsaApi::GetApiUtils().SendNotification(player, FLinearColor(NoTokens["Color"][0] / 255.0f, NoTokens["Color"][1] / 255.0f, NoTokens["Color"][2] / 255.0f), static_cast<float>(NoTokens["TextSize"]), int(NoTokens["DisplayTime"]), nullptr,
							*FString(AsaApi::Tools::Utf8Decode(NoTokens["Message"])));
						return;
					}
				}

				UFunction* Func = Dino->FindFunctionChecked(FName("ForceUpdateColorSets"));
				int Args[] = { Region, Colour };
				if (Func) Dino->ProcessEvent(Func, Args);

				FString DinoName = Dino->TamedNameField();
				if (DinoName.Len() == 0) DinoName = Dino->DinoNameTagField().ToString();
				if (NeuterAfterUse)
				{
					Dino->bNeutered() = true;
				}

				AsaApi::GetApiUtils().SendNotification(player, FLinearColor(Updated["Color"][0] / 255.0f, Updated["Color"][1] / 255.0f, Updated["Color"][2] / 255.0f), static_cast<float>(Updated["TextSize"]), int(Updated["DisplayTime"]), nullptr,
					*FString(AsaApi::Tools::Utf8Decode(Updated["Message"])), DinoName.ToString().c_str());
			}
		}
	}
	else
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(NoTarget["Color"][0] / 255.0f, NoTarget["Color"][1] / 255.0f, NoTarget["Color"][2] / 255.0f), static_cast<float>(NoTarget["TextSize"]), int(NoTarget["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(NoTarget["Message"])));
	}
}

inline void SdcDinoColour(AShooterPlayerController* player, FString* message, int mode, int)
{
	if (!player || !player->PlayerStateField() || RequiresAdmin && !player->bIsAdmin()())
		return;

	const FString eos_id = AsaApi::IApiUtils::GetEOSIDFromController(player);
	if (RequiresPermission && !Permissions::IsPlayerHasPermission(eos_id, PermissionName))
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(NoPermission["Color"][0] / 255.0f, NoPermission["Color"][1] / 255.0f, NoPermission["Color"][2] / 255.0f), static_cast<float>(NoPermission["TextSize"]), int(NoPermission["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(NoPermission["Message"])));
		return;
	}

	TArray<FString> Parsed;
	message->ParseIntoArray(Parsed, L" ", true);

	if (Parsed.Num() < 3)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(SyntaxSdc["Color"][0] / 255.0f, SyntaxSdc["Color"][1] / 255.0f, SyntaxSdc["Color"][2] / 255.0f), static_cast<float>(SyntaxSdc["TextSize"]), int(SyntaxSdc["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(SyntaxSdc["Message"])));
		return;
	}

	int Region = 0, Colour = 0;
	try
	{
		Region = std::stoi(Parsed[1].ToString().c_str());
		Colour = std::stoi(Parsed[2].ToString().c_str());
	}
	catch (...)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(SyntaxSdc["Color"][0] / 255.0f, SyntaxSdc["Color"][1] / 255.0f, SyntaxSdc["Color"][2] / 255.0f), static_cast<float>(SyntaxSdc["TextSize"]), int(SyntaxSdc["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(SyntaxSdc["Message"])));
	}

	if (Colour < 1 || Colour > 100 && Colour < 128 || Colour > 254)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(ColourError["Color"][0] / 255.0f, ColourError["Color"][1] / 255.0f, ColourError["Color"][2] / 255.0f), static_cast<float>(ColourError["TextSize"]), int(ColourError["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(ColourError["Message"])));
		return;
	}
	if (Region < 0 || Region > 5)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(RegionError["Color"][0] / 255.0f, RegionError["Color"][1] / 255.0f, RegionError["Color"][2] / 255.0f), static_cast<float>(RegionError["TextSize"]), int(RegionError["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(RegionError["Message"])));
		return;
	}

	AActor* Actor = player->GetPlayerCharacter()->GetAimedActor(ECollisionChannel::ECC_GameTraceChannel2, nullptr, 0.0, 0.0, nullptr, nullptr,
		false, false, false, false);
	SetDinoColour(player, Actor, Region, Colour);
}

inline void CdcDinoColour(AShooterPlayerController* player, FString* message, int mode, int)
{
	if (!player || !player->PlayerStateField() || RequiresAdmin && !player->bIsAdmin()())
		return;

	const FString eos_id = AsaApi::IApiUtils::GetEOSIDFromController(player);
	if (RequiresPermission && !Permissions::IsPlayerHasPermission(eos_id, PermissionName))
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(NoPermission["Color"][0] / 255.0f, NoPermission["Color"][1] / 255.0f, NoPermission["Color"][2] / 255.0f), static_cast<float>(NoPermission["TextSize"]), int(NoPermission["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(NoPermission["Message"])));
		return;
	}

	TArray<FString> Parsed;
	message->ParseIntoArray(Parsed, L" ", true);

	if (Parsed.Num() < 4)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(SyntaxSdc["Color"][0] / 255.0f, SyntaxSdc["Color"][1] / 255.0f, SyntaxSdc["Color"][2] / 255.0f), static_cast<float>(SyntaxSdc["TextSize"]), int(SyntaxSdc["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(SyntaxSdc["Message"])));
		return;
	}
	FString DinoName;
	int Region = 0, Colour = 0;
	try
	{
		DinoName = Parsed[1];
		Region = std::stoi(Parsed[2].ToString().c_str());
		Colour = std::stoi(Parsed[3].ToString().c_str());
	}
	catch (...)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(SyntaxSdc["Color"][0] / 255.0f, SyntaxSdc["Color"][1] / 255.0f, SyntaxSdc["Color"][2] / 255.0f), static_cast<float>(SyntaxSdc["TextSize"]), int(SyntaxSdc["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(SyntaxSdc["Message"])));
	}

	if (Colour < 1 || Colour > 100 && Colour < 128 || Colour > 254)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(ColourError["Color"][0] / 255.0f, ColourError["Color"][1] / 255.0f, ColourError["Color"][2] / 255.0f), static_cast<float>(ColourError["TextSize"]), int(ColourError["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(ColourError["Message"])));
		return;
	}
	if (Region < 0 || Region > 5)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(RegionError["Color"][0] / 255.0f, RegionError["Color"][1] / 255.0f, RegionError["Color"][2] / 255.0f), static_cast<float>(RegionError["TextSize"]), int(RegionError["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(RegionError["Message"])));
		return;
	}

	//Aquí incluir la búsqueda de dino por nombre...
	UWorld* world = AsaApi::GetApiUtils().GetWorld();

	TArray<AActor*>* FoundActors = new TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(world, APrimalDinoCharacter::GetPrivateStaticClass(), FoundActors);

	AActor* Actor = nullptr;
	int CountByName = 0;
	for (size_t i = 0; i < FoundActors->Num(); i++)
	{
		AActor* actorCheck = (*FoundActors)[i];

		APrimalDinoCharacter* findDino = static_cast<APrimalDinoCharacter*>(actorCheck);

		int dinoTeam = findDino->TargetingTeamField();
		if (dinoTeam == player->TargetingTeamField())
		{
			if (DinoName == findDino->TamedNameField())
			{
				Actor = (*FoundActors)[i];
				CountByName++;
				if (CountByName > 1)
				{
					AsaApi::GetApiUtils().SendNotification(player, FLinearColor(NameRepeat["Color"][0] / 255.0f, NameRepeat["Color"][1] / 255.0f, NameRepeat["Color"][2] / 255.0f), static_cast<float>(NameRepeat["TextSize"]), int(NameRepeat["DisplayTime"]), nullptr,
						*FString(AsaApi::Tools::Utf8Decode(NameRepeat["Message"])));
					return;
				}
			}
		}
	}
	if (CountByName == 0)
	{
		AsaApi::GetApiUtils().SendNotification(player, FLinearColor(DinoNotFound["Color"][0] / 255.0f, DinoNotFound["Color"][1] / 255.0f, DinoNotFound["Color"][2] / 255.0f), static_cast<float>(DinoNotFound["TextSize"]), int(DinoNotFound["DisplayTime"]), nullptr,
			*FString(AsaApi::Tools::Utf8Decode(DinoNotFound["Message"])));
		return;
	}
	else {
		SetDinoColour(player, Actor, Region, Colour);
	}
}

inline void InitCommands()
{
	AsaApi::GetCommands().AddChatCommand(cnfSdcCommand, &SdcDinoColour);
	AsaApi::GetCommands().AddChatCommand(cnfCdcCommand, &CdcDinoColour);
}

inline void RemoveCommands()
{
	AsaApi::GetCommands().RemoveChatCommand(cnfSdcCommand);
	AsaApi::GetCommands().RemoveChatCommand(cnfCdcCommand);
}
