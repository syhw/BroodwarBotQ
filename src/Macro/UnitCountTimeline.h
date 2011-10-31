#pragma once
#include <BWAPI.h>
#include <map>

//Timeline of the planned amount of each UnitType
class UnitCountTimeline
{
  public:
    UnitCountTimeline();
    void reset();
    int getFirstTime(BWAPI::UnitType t, int count);
    int getFinalCount(BWAPI::UnitType t);
    void registerUnitCountChange(int frame, BWAPI::UnitType t, int count);
    std::map< BWAPI::UnitType, std::map< int, int > > unitEvents;
    std::map< BWAPI::UnitType, int > unitFinalCounts;
};