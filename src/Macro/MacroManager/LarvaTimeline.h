#pragma once
#include <BWAPI.h>
#include <map>
#include <Task.h>

//Timeline of the planned amount of each UnitType
class LarvaTimeline
{
  public:
    LarvaTimeline();
    void reset();
    int getFirstFreeTime(BWAPI::Unit* worker, int earliestStartTime = -1);
    bool reserveLarva(BWAPI::Unit* worker, int frame);
    bool canReserveLarva(BWAPI::Unit* worker, int frame);
    int getPlannedLarvaCount(BWAPI::Unit* worker, int frame);
//  private:
    std::list<int>::iterator addLarvaUseAt(std::list<int>& l, int frame);
    void addLarvaSpawnAtOrAfter(std::list<int>& l, int frame);
    std::map<BWAPI::Unit*, std::list<int> > larvaUseTimes;
    std::map<BWAPI::Unit*, std::list<int> > larvaSpawnTimes;
};