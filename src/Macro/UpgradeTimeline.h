#pragma once
#include <BWAPI.h>
#include <map>

//Timeline of when the levels of UpgradeTypes will be increased.
class UpgradeTimeline
{
  public:
    UpgradeTimeline();
    void reset();
    int getFirstTime(BWAPI::UpgradeType t, int level);
    int getFinalLevel(BWAPI::UpgradeType t);
    void registerUpgradeLevelIncrease(int frame, BWAPI::UpgradeType t);
    std::map< BWAPI::UpgradeType, std::map< int, int > > upgradeEvents;
};