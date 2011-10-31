#pragma once
#include <BWAPI.h>
#include <map>

//Timeline of when TechTypes will be started and finished.
class TechTimeline
{
  public:
    TechTimeline();
    void reset();
    int getStartTime(BWAPI::TechType t);
    int getFinishTime(BWAPI::TechType t);
    bool isPlanned(BWAPI::TechType t);
    void registerTechStart(int frame, BWAPI::TechType t);
    void registerTechFinish(int frame, BWAPI::TechType t);
    std::map< BWAPI::TechType, std::pair< int, int > > techEvents;
};