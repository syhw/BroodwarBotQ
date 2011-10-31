#pragma once
#include <BWAPI.h>
class TechItem
{
  public:
    TechItem(BWAPI::TechType techType) : techType(techType), upgradeType(BWAPI::UpgradeTypes::None), level(1) {}
    TechItem(BWAPI::UpgradeType upgradeType, int level) : techType(BWAPI::TechTypes::None), upgradeType(upgradeType), level(level) {}
    BWAPI::TechType techType;
    BWAPI::UpgradeType upgradeType;
    int level;
};