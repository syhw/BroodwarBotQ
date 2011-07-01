#include <PrecompiledHeader.h>
#include "Macro/MacroManager/TaskStream.h"
#include "Macro/MacroManager/TaskStreamObserver.h"
#include "Macro/MacroManager/UnitReadyTimeCalculator.h"
#include "Macro/BasicWorkerFinder.h"
#include "Macro/TerminateIfEmpty.h"
#include "Macro/BasicTaskExecutor.h"
#include "Macro/BFSBuildingPlacer.h"
using namespace BWAPI;
using namespace std;
TaskStream::TaskStream(Task t0, Task t1, Task t2, Task t3)
{
  task.clear();
  task.push_back(t0);
  task.push_back(t1);
  task.push_back(t2);
  task.push_back(t3);
  worker     = NULL;
  buildUnit  = NULL;
  status     = None;
  killSwitch = false;
  plannedAdditionalResources = false;
}
TaskStream::~TaskStream()
{
  //delete observers that we own
  for each(std::pair<TaskStreamObserver*, bool> obs in observers)
  {
    obs.first->detached(this);
    if (obs.second)
      delete obs.first;
  }
  observers.clear();
}
void TaskStream::terminate()
{
  TheMacroManager->killSet.insert(this);
  killSwitch = true;
}
void TaskStream::onRevoke(BWAPI::Unit* unit)
{
  if (worker == unit)
  {
    worker = NULL;
    workerReady = false;
  }
}
void TaskStream::computeBuildUnit()
{
  if (task[0].getType()!=TaskTypes::Unit)
  {
    buildUnit = NULL;
    return;
  }
  UnitType ut = task[0].getUnit();

  //if the building dies, or isn't the right type, set it to null
  if (!(buildUnit != NULL && buildUnit->exists() && (buildUnit->getType() == ut || buildUnit->getBuildType() == ut)))
    buildUnit = NULL;

  if (locationReady)
  {
    if (buildUnit==NULL && ut.isBuilding()) //if we don't have a building yet, look for it
    {
      TilePosition bl = task[0].getTilePosition();
      //look at the units on the tile to see if it exists yet
      for each(Unit* u in Broodwar->getUnitsOnTile(bl.x(), bl.y()))
        if (u->getType() == ut && !u->isLifted())
        {
          //we found the building
          buildUnit = u;
          break;
        }
    }
    if (buildUnit == NULL && ut.isAddon()) //if we don't have a building yet, look for it
    {
      TilePosition bl = task[0].getTilePosition();
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
  if (workerReady==false) return;
  if (!worker->exists() || !worker->isCompleted()) return;

  if (worker->exists() && worker->isCompleted() && worker->getBuildUnit() != NULL && worker->getBuildUnit()->exists() && (worker->getBuildUnit()->getType() == ut || worker->getBuildUnit()->getBuildType() == ut))
    buildUnit = worker->getBuildUnit();

  if (worker->getAddon() != NULL && worker->getAddon()->exists() && (worker->getAddon()->getType() == ut || worker->getAddon()->getBuildType() == ut))
    buildUnit = worker->getAddon();

  //check to see if the worker is the right type
  //Zerg_Nydus_Canal is special since Zerg_Nydus_Canal can construct Zerg_Nydus_Canal
  if ((worker->getType() == ut || (worker->isMorphing() && worker->getBuildType() == ut)) && worker->getType()!=UnitTypes::Zerg_Nydus_Canal)
    buildUnit = worker;
}
void TaskStream::computeStatus()
{
  locationReady = true;
  workerReady = (worker != NULL) && worker->exists() && TheArbitrator->hasBid(worker) && TheArbitrator->getHighestBidder(worker).first==TheMacroManager;
  computeBuildUnit();
  if (task[0].getType()==TaskTypes::Unit)
  {
    UnitType ut = task[0].getUnit();
    if (ut.isBuilding() && !ut.whatBuilds().first.isBuilding() && buildUnit == NULL && (worker==NULL || worker->getBuildUnit()==NULL))
    {
      TilePosition tp = task[0].getTilePosition();
      if (ut.isAddon())
      {
        tp.x()+=4;
        tp.y()++;
      }
      bool canBuildHere = Broodwar->canBuildHere(NULL,tp,ut);
      if (workerReady)
        canBuildHere = Broodwar->canBuildHere(worker,tp,ut);
      if (task[0].getTilePosition().isValid()==false || !canBuildHere)
        locationReady = false;
    }
  }
  if (task[0].getType()==TaskTypes::Unit && task[0].getTilePosition().isValid() && task[0].getUnit().isBuilding())
  {
    UnitType ut = task[0].getUnit();
    TilePosition tp = task[0].getTilePosition();
    if (ut.isAddon())
    {
      tp.x()+=4;
      tp.y()++;
    }
    if (locationReady)
      Broodwar->drawBoxMap(tp.x()*32,tp.y()*32,tp.x()*32+ut.tileWidth()*32,tp.y()*32+ut.tileHeight()*32,Colors::Green);
    else
      Broodwar->drawBoxMap(tp.x()*32,tp.y()*32,tp.x()*32+ut.tileWidth()*32,tp.y()*32+ut.tileHeight()*32,Colors::Red);
    Broodwar->drawTextMap(tp.x()*32,tp.y()*32,"%s",ut.getName().c_str());
  }
  if (task[0] == NULL)
  {
    status = Error_Task_Not_Specified;
    return;
  }
  if (task[0].isExecuting() || task[0].isCompleted() || buildUnit ||
    (task[0].hasSpentResources() && workerReady && locationReady))
    status = Executing_Task;
  else
  {
    if (task[0].getType()==TaskTypes::Unit)
    {
      UnitType ut = task[0].getUnit();
      if (ut.isBuilding() && !ut.whatBuilds().first.isBuilding() && buildUnit == NULL)
      {
        if (task[0].getTilePosition().isValid()==false)
        {
          status = Error_Location_Not_Specified;
          return;
        }
        TilePosition tp = task[0].getTilePosition();
        if (ut.isAddon())
        {
          tp.x()+=4;
          tp.y()++;
        }
        bool canBuildHere = Broodwar->canBuildHere(NULL,tp,ut);
        if (workerReady)
          canBuildHere = Broodwar->canBuildHere(worker,tp,ut);
        if (!canBuildHere) //doesn't work for blocked addons!
        {
          status = Error_Location_Blocked;
          return;
        }
      }
    }
    if (task[0].getType()==TaskTypes::Unit && workerReady)
    {
      UnitType ut = task[0].getUnit();
      for each(std::pair<UnitType, int> t in ut.requiredUnits())
      {
        if (t.first.isAddon() && t.first.whatBuilds().first == worker->getType() && worker->getAddon() == NULL)
        {
          status = Error_Task_Requires_Addon;
          return;
        }
      }
    }
  }
  for(int i=0;i<(int)(task.size());i++)
  {
    if (i>0 && task[i-1].getStartTime()==-1) break;
    if (task[i].getType()==TaskTypes::None) break;
    if (!task[i].hasReservedResourcesThisFrame())
    {
      UnitReadyTimeStatus::Enum reason;
      int first_valid_frame = UnitReadyTimeCalculator::getFirstFreeTime(worker, task[i], reason,true,true);
      if (first_valid_frame==Broodwar->getFrameCount() && !workerReady)
      {
        first_valid_frame = Broodwar->getFrameCount()+10;
        if (worker==NULL || worker->exists() == false)
          reason = UnitReadyTimeStatus::Error_Worker_Not_Specified;
        else
          reason = UnitReadyTimeStatus::Error_Worker_Not_Owned;
      }
      task[i].setStartTime(first_valid_frame);
      //if we need to wait to start the first task, compute the correct status
      if ( i==0 ) //compute task stream status based on status of first unit
      {
        if (task[0].getStartTime()!=-1 && task[0].getStartTime()<=Broodwar->getFrameCount() && workerReady)
          status = Executing_Task;
        else
        {
          if (reason == UnitReadyTimeStatus::Error_Task_Requires_Addon)
            status = Error_Task_Requires_Addon;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Worker_To_Be_Ready)
            status = Waiting_For_Worker_To_Be_Ready;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Free_Time)
            status = Waiting_For_Free_Time;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Earliest_Start_Time)
            status = Waiting_For_Earliest_Start_Time;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Required_Units)
            status = Waiting_For_Required_Units;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Required_Tech)
            status = Waiting_For_Required_Tech;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Required_Upgrade)
            status = Waiting_For_Required_Upgrade;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Supply)
            status = Waiting_For_Supply;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Gas)
            status = Waiting_For_Gas;
          else if (reason == UnitReadyTimeStatus::Waiting_For_Minerals)
            status = Waiting_For_Minerals;
          else if (reason == UnitReadyTimeStatus::Error_Worker_Not_Specified)
            status = Error_Worker_Not_Specified;
          else
            status = Error_Worker_Not_Owned;
        }
      }

      if (first_valid_frame==-1) break;
      if (!TheMacroManager->rtl.reserveResources(first_valid_frame,task[i].getResources()))
        Broodwar->printf("Error: Unable to reserve resources for %s",task[i].getName().c_str());

      if (workerReady)
      {
        //protoss buildings don't take up worker time.
        if (!(task[i].getType()==TaskTypes::Unit && task[i].getUnit().isBuilding() && task[i].getRace()==Races::Protoss))
        {
          if (task[i].getType() == TaskTypes::Unit && task[i].getUnit().whatBuilds().first == UnitTypes::Zerg_Larva && worker->getType().producesLarva())
          {
            if (!TheMacroManager->ltl.reserveLarva(worker,first_valid_frame))
              Broodwar->printf("Error: Unable to reserve larva for %s",task[i].getName().c_str());
          }
          else
          {
            if (!TheMacroManager->wttl.reserveTime(worker,first_valid_frame,&task[i]))
              Broodwar->printf("Error: Unable to reserve time for %s",task[i].getName().c_str());
          }
        }
      }
      TheMacroManager->plan[first_valid_frame].push_back(std::make_pair(this,task[i]));
      if (task[i].getType()==TaskTypes::Tech)
        TheMacroManager->ttl.registerTechStart(first_valid_frame,task[i].getTech());
      task[i].setReservedResourcesThisFrame(true);
    }
    if (task[i].hasReservedResourcesThisFrame() && !task[i].hasReservedFinishDataThisFrame())
    {
      if (task[i].getType()==TaskTypes::Unit)
      {
        if (task[i].getUnit().supplyProvided()>0)
          TheMacroManager->rtl.registerSupplyIncrease(task[i].getFinishTime(), task[i].getUnit().supplyProvided());
        int count = 1;
        if (task[i].getUnit().isTwoUnitsInOneEgg())
          count = 2;
        TheMacroManager->uctl.registerUnitCountChange(task[i].getFinishTime(), task[i].getUnit(), count);
        if (task[i].getType()==TaskTypes::Tech)
          TheMacroManager->ttl.registerTechFinish(task[i].getFinishTime(),task[i].getTech());
        if (task[i].getType()==TaskTypes::Upgrade)
          TheMacroManager->utl.registerUpgradeLevelIncrease(task[i].getFinishTime(),task[i].getUpgrade());
        plannedAdditionalResources = true;
      }

      task[i].setReservedFinishDataThisFrame(true);
    }
  }
  for(int i=1;i<(int)(task.size());i++)
  {
    if (task[i-1].getStartTime()==-1)
      task[i].setStartTime(-1);
  }
}
void TaskStream::update()
{
  if (killSwitch) return;
  if (status == Executing_Task)
  {
    if (task[0].isCompleted())
    {
      notifyCompletedTask();
      status = None;
      //TODOTASK Broodwar->printf("Completed Task %s!",task[0].getName().c_str());
      for(int i=0;i+1<(int)(task.size());i++)
        task[i]=task[i+1];
      task[task.size()-1] = Task();
      buildUnit = NULL;
    }
    if (workerReady)
    {
      Broodwar->drawTextMap(worker->getPosition().x(),worker->getPosition().y(),"Task: %s",task[0].getName().c_str());
    }
  }
  for each(std::pair<TaskStreamObserver*, bool> obs in observers)
  {
    obs.first->update(this);
  }
}
bool TaskStream::updateStatus()
{
  plannedAdditionalResources = false;
  if (killSwitch) return false;
  Status lastStatus = status;
  computeStatus();
  if (status != lastStatus)
    notifyNewStatus();
  return plannedAdditionalResources;
}
void TaskStream::attach(TaskStreamObserver* obs, bool owned)
{
  //add observer to our observers set
  observers.insert(std::make_pair(obs, owned));
  //let the observer know they have been attached to us
  obs->attached(this);
}
void TaskStream::detach(TaskStreamObserver* obs)
{
  //remove observer from our observers set
  observers.erase(obs);
  //let the observer know they have been detached from us
  obs->detached(this);
}

void TaskStream::notifyNewStatus()
{
  //notify all observers of our new status
  for each(std::pair<TaskStreamObserver*, bool> obs in observers)
  {
    obs.first->newStatus(this);
  }
}
void TaskStream::notifyCompletedTask()
{
  //notify all observers that we have completed a task
  for each(std::pair<TaskStreamObserver*, bool> obs in observers)
  {
    obs.first->completedTask(this,task[0]);
  }
}
void TaskStream::notifyForkedTask(TaskStream* newTS)
{
  //notify all observers that we have forked a task
  for each(std::pair<TaskStreamObserver*, bool> obs in observers)
  {
    obs.first->forkedTask(this,newTS->task[0],newTS);
  }
}
TaskStream::Status TaskStream::getStatus() const
{
  return status;
}
std::string TaskStream::getStatusString() const
{
  //turn the status into a string
  switch (status)
  {
    case None:
      return "None";
    break;
    case Error_Worker_Not_Specified:
      return "Error_Worker_Not_Specified";
    break;
    case Error_Worker_Not_Owned:
      return "Error_Worker_Not_Owned";
    break;
    case Error_Task_Not_Specified:
      return "Error_Task_Not_Specified";
    break;
    case Error_Location_Not_Specified:
      return "Error_Location_Not_Specified";
    break;
    case Error_Location_Blocked:
      return "Error_Location_Blocked";
    break;
    case Error_Task_Requires_Addon:
      return "Error_Task_Requires_Addon";
    break;
    case Waiting_For_Worker_To_Be_Ready:
      return "Waiting_For_Worker_To_Be_Ready";
    break;
    case Waiting_For_Free_Time:
      return "Waiting_For_Free_Time";
    break;
    case Waiting_For_Earliest_Start_Time:
      return "Waiting_For_Earliest_Start_Time";
    break;
    case Waiting_For_Required_Units:
      return "Waiting_For_Required_Units";
    break;
    case Waiting_For_Required_Tech:
      return "Waiting_For_Required_Tech";
    break;
    case Waiting_For_Required_Upgrade:
      return "Waiting_For_Required_Upgrade";
    break;
    case Waiting_For_Supply:
      return "Waiting_For_Supply";
    break;
    case Waiting_For_Gas:
      return "Waiting_For_Gas";
    break;
    case Waiting_For_Minerals:
      return "Waiting_For_Minerals";
    break;
    case Executing_Task:
      return "Executing_Task";
    break;
    default:
      return "Unknown";
  }
  return "Unknown";
}
void TaskStream::setWorker(BWAPI::Unit* w)
{
  BWAPI::Unit* oldWorker = worker;
  worker = w;
  if (oldWorker!=NULL)
  {
    //tell the macro manager the old worker no longer belongs to us
    if (TheMacroManager->unitToTaskStreams.find(oldWorker)!=TheMacroManager->unitToTaskStreams.end())
      TheMacroManager->unitToTaskStreams[oldWorker].erase(this);
  }
  if (worker!=NULL)
  {
    //tell the macro manager the new worker now belongs to us
    TheMacroManager->unitToTaskStreams[worker].insert(this);
  }

  //workerReady is false until we own the new worker
  workerReady = false;
}
BWAPI::Unit* TaskStream::getWorker() const
{
  return worker;
}
void TaskStream::setBuildUnit(BWAPI::Unit* b)
{
  buildUnit = b;
}
BWAPI::Unit* TaskStream::getBuildUnit() const
{
  return buildUnit;
}
void TaskStream::setTask(int index, Task t)
{
  if (index<0 || index>=(int)(task.size())) return;
  task[index] = t;
}
Task& TaskStream::getTask(int index)
{
  return task[index];
}
void TaskStream::setUrgent(bool isUrgent)
{
  urgent = isUrgent;
}
bool TaskStream::isUrgent() const
{
  return urgent;
}
void TaskStream::printToScreen(int x, int y)
{
  Broodwar->drawTextScreen(x,y,"[ ] %s - w=%x bu=%x",
    getStatusString().c_str(),
    getWorker(),
    getBuildUnit());
  Broodwar->drawTextScreen(x+200,y,"%s %d",
    task[0].getName().c_str(),
    task[0].getStartTime());
  Broodwar->drawTextScreen(x+300,y,"%s %d",
    task[1].getName().c_str(),
    task[1].getStartTime());
  Broodwar->drawTextScreen(x+400,y,"%s %d",
    task[2].getName().c_str(),
    task[2].getStartTime());
  Broodwar->drawTextScreen(x+500,y,"%s %d",
    task[3].getName().c_str(),
    task[3].getStartTime());
}

bool TaskStream::isWorkerReady() const
{
  return workerReady;
}
bool TaskStream::isLocationReady() const
{
  return locationReady;
}
void TaskStream::clearPlanningData()
{
  //clear reserved resources and reserved finish data
  for(int i=0;i<(int)(task.size());i++)
  {
    task[i].setReservedFinishDataThisFrame(task[i].isCompleted());
    task[i].setReservedResourcesThisFrame(task[i].hasSpentResources());
    if (task[i].hasReservedResourcesThisFrame() && !task[i].hasReservedFinishDataThisFrame())
    {
      if (task[i].getType()==TaskTypes::Unit)
      {
        if (task[i].getUnit().supplyProvided()>0)
          TheMacroManager->rtl.registerSupplyIncrease(task[i].getFinishTime(), task[i].getUnit().supplyProvided());
        int count = 1;
        if (task[i].getUnit().isTwoUnitsInOneEgg())
          count = 2;
        TheMacroManager->uctl.registerUnitCountChange(task[i].getFinishTime(), task[i].getUnit(), count);
        if (task[i].getType()==TaskTypes::Tech)
          TheMacroManager->ttl.registerTechFinish(task[i].getFinishTime(),task[i].getTech());
        if (task[i].getType()==TaskTypes::Upgrade)
          TheMacroManager->utl.registerUpgradeLevelIncrease(task[i].getFinishTime(),task[i].getUpgrade());
      }
      task[i].setReservedFinishDataThisFrame(true);
    }
  }
  plannedAdditionalResources = false;
}
int TaskStream::getStartTime() const
{
  //start time of task stream is start time of first task in stream
  return task[0].getStartTime();
}
int TaskStream::getFinishTime() const
{
  //finish time of task stream is finish time of last task in stream
  if (task[0]==NULL)
    return Broodwar->getFrameCount();
  for(int i=0;i<(int)(task.size());i++)
    if (task[i].getFinishTime() == -1)
      return -1; //just to be safe, return never if any of the tasks will never finish
  return task[task.size()-1].getFinishTime();
}

int TaskStream::getFinishTime(BWAPI::UnitType t) const
{
  //returns the first time that a task of the given unit type will finish
  for(int i=0;i<(int)(task.size());i++)
    if (task[i].getType()==TaskTypes::Unit && task[i].getUnit()==t)
      return task[i].getFinishTime();
  //or returns never if the task stream will never finish a task of the given unit type
  return -1;
}

TaskStream* TaskStream::forkCurrentTask()
{
  TaskStream* ts = new TaskStream(task[0]);
  ts->attach(BasicTaskExecutor::getInstance(),false);
  ts->attach(new TerminateIfEmpty(),true);
  ts->attach(BFSBuildingPlacer::getInstance(),false);
  ts->attach(new BasicWorkerFinder(),true);
  ts->buildUnit = buildUnit;
  Broodwar->printf("Forked Task %s!",task[0].getName().c_str());
  for(int i=0;i+1<(int)(task.size());i++)
    task[i]=task[i+1];
  task[task.size()-1] = Task();
  buildUnit = NULL;
  status = None;
  notifyForkedTask(ts);
  return ts;
}