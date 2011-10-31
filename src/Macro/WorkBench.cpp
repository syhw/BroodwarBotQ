#include <PrecompiledHeader.h>
#include <Macro/MacroManager/WorkBench.h>
#include <Macro/MacroManager.h>
using namespace BWAPI;
WorkBench::WorkBench(TaskStream* ts, Unit* worker)
{
  this->ts = ts;
  this->worker = NULL;
  if (worker!=NULL)
    setWorker(worker);
}

void WorkBench::setCurrentTask(Task* t)
{
  currentTask = t;
}

Task* WorkBench::getCurrentTask()
{
  return currentTask;
}


void WorkBench::setLikelyNextTask(Task* t)
{
  likelyNextTask = t;
}

Task* WorkBench::getLikelyNextTask()
{
  return likelyNextTask;
}

void WorkBench::setWorker(Unit* w)
{
  Unit* oldWorker = worker;
  worker = w;
  if (oldWorker!=NULL)
  {
    //tell the macro manager the old worker no longer belongs to us
    if (TheMacroManager->unitToWorkBenches.find(oldWorker)!=TheMacroManager->unitToWorkBenches.end())
      TheMacroManager->unitToWorkBenches[oldWorker].erase(this);
  }
  if (worker!=NULL)
  {
    //tell the macro manager the new worker now belongs to us
    TheMacroManager->unitToWorkBenches[worker].insert(this);
  }
}

Unit* WorkBench::getWorker() const
{
  return worker;
}

void WorkBench::onRevoke(BWAPI::Unit* unit)
{
  if (worker == unit)
  {
    worker = NULL;
  }
}

void WorkBench::setBuildUnit(Unit* b)
{
  buildUnit = b;
}

Unit* WorkBench::getBuildUnit() const
{
  return buildUnit;
}

void WorkBench::computeBuildUnit()
{
  if (currentTask==NULL) return;
  if (currentTask->getType()!=TaskTypes::Unit)
  {
    buildUnit = NULL;
    return;
  }
  UnitType ut = currentTask->getUnit();

  //if the building dies, or isn't the right type, set it to null
  if (!(buildUnit != NULL && buildUnit->exists() && (buildUnit->getType() == ut || buildUnit->getBuildType() == ut)))
    buildUnit = NULL;

  if (buildUnit==NULL && ut.isBuilding()) //if we don't have a building yet, look for it
  {
    TilePosition bl = currentTask->getTilePosition();
    //look at the units on the tile to see if it exists yet
    for each(Unit* u in Broodwar->getUnitsOnTile(bl.x(), bl.y()))
      if (u->getType() == ut && !u->isLifted())
      {
        //we found the building
        buildUnit = u;
        break;
      }
    if (buildUnit == NULL && ut.isAddon()) //if we don't have a building yet, look for it
    {
      TilePosition bl = currentTask->getTilePosition();
      bl.x()+=4;
      bl.y()++;
      for each(Unit* u in Broodwar->getUnitsOnTile(bl.x(), bl.y()))
        if (u->getType() == ut && !u->isLifted())
        {
          //we found the building
          buildUnit = u;
          break;
        }
    }
  }
  if (isWorkerReady()==false) return;

  if (worker->getBuildUnit() != NULL && worker->getBuildUnit()->exists() && (worker->getBuildUnit()->getType() == ut || worker->getBuildUnit()->getBuildType() == ut))
    buildUnit = worker->getBuildUnit();

  if (worker->getAddon() != NULL && worker->getAddon()->exists() && (worker->getAddon()->getType() == ut || worker->getAddon()->getBuildType() == ut))
    buildUnit = worker->getAddon();

  //check to see if the worker is the right type
  //Zerg_Nydus_Canal is special since Zerg_Nydus_Canal can construct Zerg_Nydus_Canal
  if ((worker->getType() == ut || (worker->isMorphing() && worker->getBuildType() == ut)) && worker->getType()!=UnitTypes::Zerg_Nydus_Canal)
    buildUnit = worker;
}

bool WorkBench::isWorkerReady()
{
  return (worker != NULL) && worker->exists() && worker->isCompleted() && TheArbitrator->hasBid(worker) && TheArbitrator->getHighestBidder(worker).first==TheMacroManager;
}

bool WorkBench::isLocationReady()
{
  computeBuildUnit();
  if (currentTask==NULL || currentTask->getType()!=TaskTypes::Unit || currentTask->getUnit().isBuilding()==false) return true;
  if (currentTask->getTilePosition().isValid()==false)
    return false;
  UnitType ut = currentTask->getUnit();
  TilePosition tp = currentTask->getTilePosition();
  if (ut.isAddon())
  {
    tp.x()+=4;
    tp.y()++;
  }
  bool canBuildHere = Broodwar->canBuildHere(NULL,tp,ut);
  if (isWorkerReady())
    canBuildHere = Broodwar->canBuildHere(worker,tp,ut);
  if (!canBuildHere)
    return false;
  return true;
}

void WorkBench::update()
{
  if (currentTask && currentTask->isCompleted())
  {
    ts->completeTask(this,currentTask);
    Broodwar->printf("Completed Task %s!",currentTask->getName().c_str());
    currentTask = NULL;
    buildUnit = NULL;
  }
  if (isWorkerReady() && currentTask)
  {
    Broodwar->drawTextMap(worker->getPosition().x(),worker->getPosition().y(),"Task: %s",currentTask->getName().c_str());
  }
  if (currentTask && currentTask->getType()==TaskTypes::Unit && currentTask->getTilePosition().isValid() && currentTask->getUnit().isBuilding())
  {
    UnitType ut = currentTask->getUnit();
    TilePosition tp = currentTask->getTilePosition();
    if (ut.isAddon())
    {
      tp.x()+=4;
      tp.y()++;
    }
    if (isLocationReady())
      Broodwar->drawBoxMap(tp.x()*32,tp.y()*32,tp.x()*32+ut.tileWidth()*32,tp.y()*32+ut.tileHeight()*32,Colors::Green);
    else
      Broodwar->drawBoxMap(tp.x()*32,tp.y()*32,tp.x()*32+ut.tileWidth()*32,tp.y()*32+ut.tileHeight()*32,Colors::Red);
    Broodwar->drawTextMap(tp.x()*32,tp.y()*32,"%s",ut.getName().c_str());
  }
}