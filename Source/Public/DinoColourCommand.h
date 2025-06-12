#pragma once
#include "json.hpp"

inline bool RequiresAdmin, RequiresPermission, RequireLvl, NeuterAfterUse, ArkShopEnable, TokenBankEnable;
FString PermissionName, PermissionArkShopFree, PermissionTokenBankFree, cnfSdcCommand, cnfCdcCommand, TBType;
int LicenseID, MinLvl, ShopCost, TBCost;
nlohmann::json NoPermission, NoPoints, NoTokens, SyntaxSdc, SyntaxCdc, ColourError, RegionError, TribeError, DinoLevelError, NoTarget, NameRepeat, DinoNotFound, Updated;