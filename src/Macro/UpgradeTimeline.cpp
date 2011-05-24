#include <PrecompiledHeader.h>
#include "UpgradeTimeline.h"
using namespace BWAPI;
UpgradeTimeline::UpgradeTimeline()
{
  upgradeEvents.clear();
}
void UpgradeTimeline::reset()
{
  upgradeEvents.clear();
}
int UpgradeTimeline::getFirstTime(BWAPI::UpgradeType t, int level)
{
  //get current upgrade level and frame
  int l = Broodwar->self()->getUpgradeLevel(t);
  int frame = Broodwar->getFrameCount();

  //if current level is not less than specified level, return current frame
  if (l >= level)
    return frame;

  //if there are no events for this upgrade type, it will never reach the specified level
  if (upgradeEvents.find(t)==upgradeEvents.end())
    return -1;//return never

  //get events for this upgrade type
  std::map<int, int>* events = &(upgradeEvents.find(t)->second);

  //iterate until we reach the specified level
  for(std::map<int, int>::iterator i=events->begin();i!=events->end();i++)
  {
    frame = (*i).first;
    l+=(*i).second;
    if ( l>= level)
      return frame; //reached the specified level, so return the frame on which it will be reached
  }
  return -1;//return never
}
void UpgradeTimeline::registerUpgradeLevelIncrease(int frame, BWAPI::UpgradeType t)
{
  upgradeEvents[t][frame]+=1;
}
int UpgradeTimeline::getFinalLevel(BWAPI::UpgradeType t)
{
  //returns the final planned level for this upgrade type

  //get current level
  int l = Broodwar->self()->getUpgradeLevel(t);

  //if there are no events for this upgrade type, the current level is the final level
  if (upgradeEvents.find(t)==upgradeEvents.end())
    return l;

  //otherwise, iterate over list of events for this upgrade type
  std::map<int, int>* events = &(upgradeEvents.find(t)->second);
  for(std::map<int, int>::iterator i=events->begin();i!=events->end();i++)
    l+=(*i).second;

  //return final level
  return l;
}