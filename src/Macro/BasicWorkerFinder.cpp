#include <PrecompiledHeader.h>
#include <BasicWorkerFinder.h>
#include <Arbitrator.h>
#include <MacroManager.h>
using namespace BWAPI;
void BasicWorkerFinder::update(TaskStream* ts)
{
  newStatus(ts);
}
void BasicWorkerFinder::attached(TaskStream* ts)
{
  this->ts = ts;
  newStatus(ts);
}
void BasicWorkerFinder::newStatus(TaskStream* ts)
{
  if (ts->isWorkerReady()==false)
  {
    //don't look for a worker unless we have a build location (if needed)
    if (ts->getTask(0).getType()==TaskTypes::Unit && ts->getTask(0).getUnit().isBuilding() && (!ts->getTask(0).getTilePosition().isValid()))
      return;
    //don't look for a worker if we have a build unit as protoss
    if (ts->getBuildUnit()!=NULL && ts->getBuildUnit()->exists() && ts->getBuildUnit()->getType().isBuilding() && ts->getBuildUnit()->getType().getRace()==Races::Protoss)
      return;
    if (ts->getStartTime()<0 || ts->getStartTime()>Broodwar->getFrameCount()+20*24)
      return;

    std::set<BWAPI::Unit*> units;
    for each(Unit* u in Broodwar->self()->getUnits())
    {
      if (u->exists() && u->isCompleted() && u->isLoaded()==false)
      {
        bool isCorrectType = false;
        if (ts->getTask(0).getWorkerType()==UnitTypes::Zerg_Larva)
          isCorrectType = u->getType().producesLarva();
        else
          isCorrectType = (u->getType()==ts->getTask(0).getWorkerType());
        if (isCorrectType)
        {
          if (ts->getTask(0).getType()==TaskTypes::Unit && ts->getTask(0).getUnit().isAddon() && u->getAddon()!=NULL)
            continue;
            if (TheArbitrator->hasBid(u) && TheArbitrator->getHighestBidder(u).second>=100.0 && TheArbitrator->getHighestBidder(u).first!=TheMacroManager)
            continue;
          if (TheMacroManager->getTaskStreams(u).empty()==false)
            continue;

          units.insert(u);
        }
      }
    }
    if (units.empty())
    {
      for each(Unit* u in Broodwar->self()->getUnits())
      {
        if (u->exists() && u->isCompleted() && u->isLoaded()==false)
        {
          bool isCorrectType = false;
          if (ts->getTask(0).getWorkerType()==UnitTypes::Zerg_Larva)
            isCorrectType = u->getType().producesLarva();
          else
            isCorrectType = (u->getType()==ts->getTask(0).getWorkerType());
          if (isCorrectType)
          {
            if (ts->getTask(0).getType()==TaskTypes::Unit && ts->getTask(0).getUnit().isAddon() && u->getAddon()!=NULL)
              continue;
            if (TheArbitrator->hasBid(u) && TheArbitrator->getHighestBidder(u).second>=100.0 && TheArbitrator->getHighestBidder(u).first!=TheMacroManager)
              continue;

            units.insert(u);
          }
        }
      }

    }
    Unit* chosenWorker = NULL;
    if (ts->getTask(0).getTilePosition().isValid())
    {
      //if the task has a tile position, choose the worker closest to it
      Position p=Position(ts->getTask(0).getTilePosition());
      double minDist=-1;
      for each(Unit* u in units)
      {
        double d = u->getDistance(p);
        if (minDist < 0 || d<minDist)
        {
          minDist = d;
          chosenWorker = u;
        }
      }
    }
    else
    {
      //otherwise just choose the first worker we find
      if (units.empty()==false)
        chosenWorker = (*units.begin());
    }
    if (units.size()>0)
      chosenWorker = (*units.begin());
    ts->setWorker(chosenWorker);
    if (chosenWorker!=NULL)
    {
      if (ts->getTask(0).getType()==TaskTypes::Unit && ts->getTask(0).getUnit().isAddon())
        ts->getTask(0).setTilePosition(chosenWorker->getTilePosition());
    }
  }
}
std::string BasicWorkerFinder::getName() const
{
  return "Basic Worker Finder";
}
std::string BasicWorkerFinder::getShortName() const
{
  return "BWF";
}