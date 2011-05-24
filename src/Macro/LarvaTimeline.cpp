#include <PrecompiledHeader.h>
#include <MacroManager/LarvaTimeline.h>
#include <MacroManager.h>
using namespace std;
using namespace BWAPI;

LarvaTimeline::LarvaTimeline()
{
}

void LarvaTimeline::reset()
{
  larvaSpawnTimes.clear();
  larvaUseTimes.clear();
  for each(Unit* u in Broodwar->self()->getUnits())
  {
    if (u->getType().producesLarva())
    {
      if (u->isCompleted() || u->getType()!=UnitTypes::Zerg_Hatchery)
      {
        if (u->getLarva().size()<3)
        {
          larvaSpawnTimes[u].push_back(Broodwar->getFrameCount()+u->getRemainingTrainTime());
          if (u->getLarva().size()<2)
            larvaSpawnTimes[u].push_back(Broodwar->getFrameCount()+u->getRemainingTrainTime()+334);
          if (u->getLarva().size()<1)
            larvaSpawnTimes[u].push_back(Broodwar->getFrameCount()+u->getRemainingTrainTime()+334+334);
        }
      }
      else
      {
        larvaSpawnTimes[u].push_back(Broodwar->getFrameCount()+u->getRemainingBuildTime());
        larvaSpawnTimes[u].push_back(Broodwar->getFrameCount()+u->getRemainingBuildTime()+334);
        larvaSpawnTimes[u].push_back(Broodwar->getFrameCount()+u->getRemainingBuildTime()+334+334);
      }
    }
  }
}
int LarvaTimeline::getFirstFreeTime(BWAPI::Unit* worker, int earliestStartTime)
{
  if (!worker || !worker->exists()) return -1;
  if (canReserveLarva(worker,earliestStartTime))
    return earliestStartTime;

  std::list<int>& testNewLarvaUseTimes = larvaUseTimes[worker];
  std::list<int>& testNewLarvaSpawnTimes = larvaSpawnTimes[worker];
  std::list<int>::iterator i_u = testNewLarvaUseTimes.begin();
  std::list<int>::iterator i_s = testNewLarvaSpawnTimes.begin();
  int larvaCount = worker->getLarva().size();
  bool waitingForZero = false;
  for(;i_u!=testNewLarvaUseTimes.end() || i_s!=testNewLarvaSpawnTimes.end();)
  {
    if (i_u==testNewLarvaUseTimes.end() || (i_s!=testNewLarvaSpawnTimes.end() && *i_s<=*i_u))
    {
      larvaCount++;
      if (*i_s>=earliestStartTime && !waitingForZero)
      {
        if (canReserveLarva(worker,*i_s))
          return *i_s;
      }
      i_s++;
    }
    else
    {
      larvaCount--;
      if (larvaCount == 0)
        waitingForZero = false;
      i_u++;
    }
  }
  //should never get here
  return -1;
}
bool LarvaTimeline::canReserveLarva(BWAPI::Unit* worker, int frame)
{
  std::list<int>& testNewLarvaUseTimes = larvaUseTimes[worker];
  std::list<int> testNewLarvaSpawnTimes = larvaSpawnTimes[worker];

  std::list<int>::iterator addedLarvaUse = addLarvaUseAt(testNewLarvaUseTimes,frame);
  addLarvaSpawnAtOrAfter(testNewLarvaSpawnTimes,frame+334);

  std::list<int>::iterator i_u = testNewLarvaUseTimes.begin();
  std::list<int>::iterator i_s = testNewLarvaSpawnTimes.begin();
  int larvaCount = worker->getLarva().size();
  for(;i_u!=testNewLarvaUseTimes.end() || i_s!=testNewLarvaSpawnTimes.end();)
  {
    if (i_u==testNewLarvaUseTimes.end() || (i_s!=testNewLarvaSpawnTimes.end() && *i_s<=*i_u))
    {
      larvaCount++;
      i_s++;
    }
    else
    {
      larvaCount--;
      i_u++;
    }
    if (larvaCount<0)
    {
      testNewLarvaUseTimes.erase(addedLarvaUse);
      return false;
    }
  }
  testNewLarvaUseTimes.erase(addedLarvaUse);
  return true;
}
bool LarvaTimeline::reserveLarva(BWAPI::Unit* worker, int frame)
{
  std::list<int>& testNewLarvaUseTimes = larvaUseTimes[worker];
  std::list<int> testNewLarvaSpawnTimes = larvaSpawnTimes[worker];

  std::list<int>::iterator addedLarvaUse = addLarvaUseAt(testNewLarvaUseTimes,frame);
  addLarvaSpawnAtOrAfter(testNewLarvaSpawnTimes,frame+334);

  std::list<int>::iterator i_u = testNewLarvaUseTimes.begin();
  std::list<int>::iterator i_s = testNewLarvaSpawnTimes.begin();
  int larvaCount = worker->getLarva().size();
  for(;i_u!=testNewLarvaUseTimes.end() || i_s!=testNewLarvaSpawnTimes.end();)
  {
    if (i_u==testNewLarvaUseTimes.end() || (i_s!=testNewLarvaSpawnTimes.end() && *i_s<=*i_u))
    {
      larvaCount++;
      i_s++;
    }
    else
    {
      larvaCount--;
      i_u++;
    }
    if (larvaCount<0)
    {
      testNewLarvaUseTimes.erase(addedLarvaUse);
      return false;
    }
  }
  //copy over new spawn times
  larvaSpawnTimes[worker]=testNewLarvaSpawnTimes;
  return true;
}
std::list<int>::iterator LarvaTimeline::addLarvaUseAt(std::list<int>& l, int frame)
{
  for(list<int>::iterator i=l.begin();i!=l.end();i++)
  {
    if (*i>=frame)
    {
      return l.insert(i,frame);
    }
  }
  return l.insert(l.end(),frame);
}
void LarvaTimeline::addLarvaSpawnAtOrAfter(std::list<int>& l, int frame)
{
  int previousSpawnFrame=-1000;
  //add larva spawn
  bool added = false;
  for(list<int>::iterator i=l.begin();i!=l.end();i++)
  {
    if (*i>=frame)
    {
      added = true;
      l.insert(i,frame);
      break;
    }
  }
  if (!added)
    l.push_back(frame);
  //correct spawn timeline
  for(list<int>::iterator i=l.begin();i!=l.end();i++)
  {
    if (*i<previousSpawnFrame+334)
      *i=previousSpawnFrame+334;
    previousSpawnFrame = *i;
  }
}
int LarvaTimeline::getPlannedLarvaCount(BWAPI::Unit* worker, int frame)
{
  std::list<int>& testNewLarvaUseTimes = larvaUseTimes[worker];
  std::list<int>& testNewLarvaSpawnTimes = larvaSpawnTimes[worker];
  std::list<int>::iterator i_u = testNewLarvaUseTimes.begin();
  std::list<int>::iterator i_s = testNewLarvaSpawnTimes.begin();
  int larvaCount = worker->getLarva().size();
  for(;i_s!=testNewLarvaUseTimes.end() && i_u!=testNewLarvaUseTimes.end();)
  {
    if (*i_s<=*i_u)
    {
      larvaCount++;
      if (*i_s>frame)
        return larvaCount;
    }
    else
    {
      larvaCount--;
      if (*i_u>frame)
        return larvaCount;
    }
  }
  return larvaCount;
}