#include <PrecompiledHeader.h>
#include <PrecompiledHeader.h>
#include "UnitCountTimeline.h"
#include <BWAPI.h>
using namespace BWAPI;
UnitCountTimeline::UnitCountTimeline()
{
  unitEvents.clear();
}
void UnitCountTimeline::reset()
{
  unitEvents.clear();
  for each(UnitType t in UnitTypes::allUnitTypes())
  {
    unitFinalCounts[t] = Broodwar->self()->completedUnitCount(t);
  }
}
int UnitCountTimeline::getFirstTime(BWAPI::UnitType t, int count)
{
  int c = Broodwar->self()->completedUnitCount(t);
  int frame = Broodwar->getFrameCount();
  if (c >= count)
    return frame;
  if (unitFinalCounts[t] < count)
    return -1;
  if (unitEvents.find(t)==unitEvents.end())
    return -1;
  std::map<int, int>* events = &(unitEvents.find(t)->second);
  for(std::map<int, int>::iterator i=events->begin();i!=events->end();i++)
  {
    frame = (*i).first;
    c+=(*i).second;
    if ( c>= count)
      return frame;
  }
  return -1;
}
void UnitCountTimeline::registerUnitCountChange(int frame, BWAPI::UnitType t, int count)
{
  unitEvents[t][frame]+=count;
  unitFinalCounts[t]  +=count;
}
int UnitCountTimeline::getFinalCount(BWAPI::UnitType t)
{
  return unitFinalCounts[t];
}