#include <PrecompiledHeader.h>
#include "TechTimeline.h"
using namespace BWAPI;
TechTimeline::TechTimeline()
{
  techEvents.clear();
}
void TechTimeline::reset()
{
  techEvents.clear();
}
int TechTimeline::getStartTime(BWAPI::TechType t)
{
  if (isPlanned(t)==false)
    return -1;
  return techEvents[t].first;
}
int TechTimeline::getFinishTime(BWAPI::TechType t)
{
  if (isPlanned(t)==false)
    return -1;
  return techEvents[t].second;
}
bool TechTimeline::isPlanned(BWAPI::TechType t)
{
  return techEvents.find(t)!=techEvents.end();
}
void TechTimeline::registerTechStart(int frame, BWAPI::TechType t)
{
  if (isPlanned(t)==false)
    techEvents[t].second=frame+t.researchTime();
  techEvents[t].first=frame;
}
void TechTimeline::registerTechFinish(int frame, BWAPI::TechType t)
{
  if (isPlanned(t)==false)
    techEvents[t].first=frame-t.researchTime();
  techEvents[t].second=frame;
}