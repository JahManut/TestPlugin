#pragma once

#include <fstream>
#include "json.hpp"

inline void InitConfig()
{
	const std::string config_path = AsaApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/DinoColourCommand/config.json";
	std::ifstream file{ config_path };
	if (!file.is_open())
	{
		throw std::runtime_error("Can't open config.json");
		RequiresAdmin = true;
		ArkShopEnable = false;
		TokenBankEnable = false;
		NeuterAfterUse = false;
		RequireLvl = false;
		cnfSdcCommand = "/sdc";
		cnfCdcCommand = "/cdc";
		return;
	}

	nlohmann::json configData;
	file >> configData;
	file.close();

	RequiresAdmin = configData["DinoColourCommand"]["RequireAdmin"];
	RequiresPermission = configData["DinoColourCommand"]["Permission"].value("Enable", false);
	NeuterAfterUse = configData["DinoColourCommand"].value("NeuterAfterUse", false);
	ArkShopEnable = configData["DinoColourCommand"]["ArkShop"].value("Enable", false);
	ShopCost = configData["DinoColourCommand"]["ArkShop"].value("Cost", 1);
	TokenBankEnable = configData["DinoColourCommand"]["TokenBank"].value("Enable", false);
	TBCost = configData["DinoColourCommand"]["TokenBank"].value("Cost", 1);
	TBType = FString(AsaApi::Tools::Utf8Decode(configData["DinoColourCommand"]["TokenBank"].value("Type", "DinoColorCommand")));
	PermissionName = FString(AsaApi::Tools::Utf8Decode(configData["DinoColourCommand"]["Permission"].value("Name", "DinoColorCommand")));
	PermissionArkShopFree = FString(AsaApi::Tools::Utf8Decode(configData["DinoColourCommand"]["ArkShop"].value("PermissionForFree", "DinoColorFree")));
	PermissionTokenBankFree = FString(AsaApi::Tools::Utf8Decode(configData["DinoColourCommand"]["TokenBank"].value("PermissionForFree", "DinoColorFree")));
	cnfSdcCommand = FString(AsaApi::Tools::Utf8Decode(configData["DinoColourCommand"].value("SdcCommand", "/sdc")));
	cnfCdcCommand = FString(AsaApi::Tools::Utf8Decode(configData["DinoColourCommand"].value("CdcCommand", "/cdc")));
	RequireLvl = configData["DinoColourCommand"]["RequireLvl"].value("Enable", false);
	MinLvl = configData["DinoColourCommand"]["RequireLvl"].value("MinLvl", 1);
	NoPermission = configData["Messages"]["NoPermission"];
	NoPoints = configData["Messages"]["NoPoints"];
	NoTokens = configData["Messages"]["NoTokens"];
	SyntaxSdc = configData["Messages"]["SyntaxSdc"];
	SyntaxCdc = configData["Messages"]["SyntaxCdc"];
	ColourError = configData["Messages"]["ColourError"];
	RegionError = configData["Messages"]["RegionError"];
	TribeError = configData["Messages"]["TribeError"];
	DinoLevelError = configData["Messages"]["DinoLevelError"];
	NoTarget = configData["Messages"]["NoTarget"];
	NameRepeat = configData["Messages"]["NameRepeat"];
	DinoNotFound = configData["Messages"]["DinoNotFound"];
	Updated = configData["Messages"]["Updated"];
}
