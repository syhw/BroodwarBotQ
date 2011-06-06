#include <PrecompiledHeader.h>
#include "Macro/MacroManager.h"
#include <BWAPI.h>
#include "Macro/ResourceRates.h"
#include <math.h>
using namespace BWAPI;
using namespace std;
MacroManager* TheMacroManager = NULL;
std::set<TaskStream*> emptyTSSet;

MacroManager* MacroManager::create()
{
  if (TheMacroManager) return TheMacroManager;
  return new MacroManager();
}
void MacroManager::destroy()
{
  if (TheMacroManager)
    delete TheMacroManager;
}
MacroManager::MacroManager()
{
  TheMacroManager = this;
  taskstream_list_visible = true;
}
MacroManager::~MacroManager()
{
  TheMacroManager = NULL;
  for(std::list<TaskStream*>::iterator i=taskStreams.begin();i!=taskStreams.end();i++)
    delete *i;
}
void MacroManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for each(Unit* u in units)
  {
    if (unitToTaskStreams.find(u)!=unitToTaskStreams.end() && !unitToTaskStreams[u].empty())
    {
      //The Arbitrator has offered us a worker our task streams want to use, so accept it
      TheArbitrator->accept(this,u);
      TheArbitrator->setBid(this,u,100);
      ownedUnits.insert(u);
    }
    else
    {
      //The Arbitrator has offered us a worker we don't want, decline it and remove the bid
      TheArbitrator->decline(this,u,0);
      TheArbitrator->removeBid(this,u);
    }
  }
}
void MacroManager::onRevoke(BWAPI::Unit* unit, double bid)
{
  ownedUnits.erase(unit);
  if (unitToTaskStreams.find(unit)!=unitToTaskStreams.end())
  {
    //tell all task streams that were using this worker that it is no longer available.
    for each(TaskStream* ts in unitToTaskStreams[unit])
    {
      ts->onRevoke(unit);
    }
  }
  unitToTaskStreams.erase(unit);
}
void MacroManager::update()
{
  bool done= false;
  while (!done)
  {
    done=true;
    for(std::map<BWAPI::Unit*, std::set<TaskStream*> >::iterator i=unitToTaskStreams.begin();i!=unitToTaskStreams.end();i++)
    {
      if (i->second.empty())
      {
        TheArbitrator->removeBid(this,i->first);
        unitToTaskStreams.erase(i);
        done = false;
        break;
      }
    }
  }

  spentResources.setSupply(0);//don't keep track of spent supply
  Resources r = CumulativeResources(Broodwar->self())-spentResources;
  rtl.reset(r,Broodwar->self()->supplyTotal(),TheResourceRates->getGatherRate().getMinerals(),TheResourceRates->getGatherRate().getGas());
  uctl.reset();
  ttl.reset();
  utl.reset();
  wttl.reset();
  ltl.reset();
  plan.clear();
  for each(TaskStream* ts in killSet)
  {
    Unit* worker = ts->getWorker();
    if (worker!=NULL)
    {
      //remove the task stream from the worker's set in unitToTaskStreams
      unitToTaskStreams[worker].erase(ts);
    }

    //find and remove the task stream from the taskStreams list
    for(std::list<TaskStream*>::iterator i=taskStreams.begin();i!=taskStreams.end();i++)
    {
      if (*i == ts)
      {
        taskStreams.erase(i);
        break;
      }
    }
    //finally, delete the task stream
    delete ts;
  }
  //clear the planning data of each task stream
  for each(TaskStream* ts in taskStreams)
    ts->clearPlanningData();
  Broodwar->drawTextScreen(452,16,"\x07%d",(int)(TheResourceRates->getGatherRate().getMinerals()*23*60));
  Broodwar->drawTextScreen(520,16,"\x07%d",(int)(TheResourceRates->getGatherRate().getGas()*23*60));
  killSet.clear();
  int y=30;
  Broodwar->drawTextScreen(10,5,"Frame: %d",Broodwar->getFrameCount());
  bool plannedAdditionalResources = true;
  while(plannedAdditionalResources)
  {
    plannedAdditionalResources = false;
    for each(TaskStream* ts in taskStreams)
    {
      if (ts->updateStatus())
      {
        plannedAdditionalResources = true;
        break;
      }
    }
  }
  for each(TaskStream* ts in taskStreams)
    ts->update();

  if (taskstream_list_visible)
  {
    for each(TaskStream* ts in taskStreams)
    {
      ts->printToScreen(10,y);
      y+=20;
    }
  }
  {
    int currentFrameCount = Broodwar->getFrameCount();
    double ymax = 200;
    double y=ymax;
    double hscale = 0.3;
    double vscale = 0.3;
    int v=0;
    if (v==0)
    {
      y+=r.getMinerals()*vscale;
      for each(std::pair<int, std::list< std::pair<TaskStream*, Task> > > tl in plan)
      {
        int frame = tl.first;
        for each(std::pair<TaskStream*, Task> tp in tl.second)
        {
          Task t=tp.second;
          if (t.getName()=="None") continue;
          double x=(frame - currentFrameCount)*hscale;
          double res=t.getResources().getMinerals()*vscale;
          y-=res;
          double duration=t.getTime()*hscale;
          Broodwar->drawBoxScreen((int)(x),(int)(y),(int)(x+duration),(int)(y+res),Colors::White);
          Broodwar->drawTextScreen((int)(x),(int)(y),"%s",t.getName().c_str());
        }
      }
      y=ymax;
      int y2=(int)(y-TheResourceRates->getGatherRate().getMinerals()*(640/hscale)*vscale);
      if (y2>-1000 && y2<=y)
        Broodwar->drawLineScreen(0,(int)(y),640,y2,Colors::Cyan);
    }
    else if (v==1)
    {
      y+=r.getGas()*vscale;
      for each(std::pair<int, std::list< std::pair<TaskStream*, Task> > > tl in plan)
      {
        int frame = tl.first;
        for each(std::pair<TaskStream*, Task> tp in tl.second)
        {
          Task t=tp.second;
          double x=(frame - currentFrameCount)*hscale;
          double res=t.getResources().getGas()*vscale;
          y-=res;
          double duration=t.getTime()*hscale;
          Broodwar->drawBoxScreen((int)(x),(int)(y),(int)(x+duration),(int)(y+res),Colors::White);
          Broodwar->drawTextScreen((int)(x),(int)(y),"%s",t.getName().c_str());
        }
      }
      y=ymax;
      int y2=(int)(y-TheResourceRates->getGatherRate().getGas()*(640/hscale)*vscale);
      if (y2>-1000 && y2<=y)
        Broodwar->drawLineScreen(0,(int)(y),640,y2,Colors::Cyan);
    }
    
    Unit* worker = NULL;
    for each(Unit* u in Broodwar->self()->getUnits())
      if (u->getType().producesLarva()) worker = u;
    if (worker)
    {
    std::list<int>& testNewLarvaUseTimes = ltl.larvaUseTimes[worker];
    std::list<int>& testNewLarvaSpawnTimes = ltl.larvaSpawnTimes[worker];
    std::list<int>::iterator i_u = testNewLarvaUseTimes.begin();
    std::list<int>::iterator i_s = testNewLarvaSpawnTimes.begin();
    int larvaCount = worker->getLarva().size();
    int curFrame=Broodwar->getFrameCount();
    int prvFrame=curFrame;
    for(;i_u!=testNewLarvaUseTimes.end() || i_s!=testNewLarvaSpawnTimes.end();)
    {
      if (i_u==testNewLarvaUseTimes.end() || (i_s!=testNewLarvaSpawnTimes.end() && *i_s<=*i_u))
      {
        curFrame = *i_s;
        double x1=(prvFrame - currentFrameCount)*hscale;
        double x2=(curFrame - currentFrameCount)*hscale;
        Broodwar->drawLineScreen(static_cast<int>(x1),static_cast<int>(280-larvaCount*20),static_cast<int>(x2),static_cast<int>(280-larvaCount*20),Colors::Green);
        Broodwar->drawLineScreen(static_cast<int>(x2),static_cast<int>(280-larvaCount*20),static_cast<int>(x2),static_cast<int>(280-(larvaCount+1)*20),Colors::Green);
        larvaCount++;
        i_s++;
      }
      else
      {
        curFrame = *i_u;
        double x1=(prvFrame - currentFrameCount)*hscale;
        double x2=(curFrame - currentFrameCount)*hscale;
        Broodwar->drawLineScreen(static_cast<int>(x1),static_cast<int>(280-larvaCount*20),static_cast<int>(x2),static_cast<int>(280-larvaCount*20),Colors::Green);
        Broodwar->drawLineScreen(static_cast<int>(x2),static_cast<int>(280-larvaCount*20),static_cast<int>(x2),static_cast<int>(280-(larvaCount-1)*20),Colors::Green);
        larvaCount--;
        i_u++;
      }
      prvFrame = curFrame;
    }
    double x1=(prvFrame - currentFrameCount)*hscale;
    Broodwar->drawLineScreen(static_cast<int>(x1),static_cast<int>(280-larvaCount*20),640,static_cast<int>(280-larvaCount*20),Colors::Green);
    Broodwar->drawLineScreen(0,280-0*20,640,280-0*20,Colors::Red);
  }
  }

  /*
    for(std::map<int, Resources>::iterator i=rtl.resourceEvents.begin();i!=rtl.resourceEvents.end();i++)
    {
      Broodwar->drawTextScreen(10,y,"%d: %s",(*i).first,(*i).second.toString().c_str());
      y+=20;
    }
  */
  //bid on all the workers our task streams want to use
  for(std::map<BWAPI::Unit*, std::set<TaskStream*> >::iterator i=unitToTaskStreams.begin();i!=unitToTaskStreams.end();i++)
  {
    if (!i->second.empty())
      TheArbitrator->setBid(this,i->first,100);
  }
  for each(Unit* u in ownedUnits)
  {
    std::map<BWAPI::Unit*, std::set<TaskStream*> >::iterator i=unitToTaskStreams.find(u);
    if (i==unitToTaskStreams.end() || i->second.empty())
      TheArbitrator->removeBid(this,u);
    if (i==unitToTaskStreams.end())
      unitToTaskStreams.erase(u);
  }
}
std::string MacroManager::getName() const
{
  return "Macro Manager";
}
std::string MacroManager::getShortName() const
{
  return "Macro";
}

bool MacroManager::insertTaskStreamAbove(TaskStream* newTS, TaskStream* existingTS)
{
  if (newTS==NULL || existingTS==NULL) return false;
  std::list<TaskStream*>::iterator e_iter = taskStreams.end();
  for(std::list<TaskStream*>::iterator i=taskStreams.begin();i!=taskStreams.end();i++)
  {
    if (*i==existingTS)
    {
      e_iter = i;
      break;
    }
  }
  if (e_iter == taskStreams.end()) return false;
  taskStreams.insert(e_iter,newTS);
  return true;
}
bool MacroManager::insertTaskStreamBelow(TaskStream* newTS, TaskStream* existingTS)
{
  if (newTS==NULL || existingTS==NULL) return false;
  std::list<TaskStream*>::iterator e_iter = taskStreams.end();
  for(std::list<TaskStream*>::iterator i=taskStreams.begin();i!=taskStreams.end();i++)
  {
    if (*i==existingTS)
    {
      e_iter = i;
      break;
    }
  }
  if (e_iter == taskStreams.end()) return false;
  e_iter++;
  taskStreams.insert(e_iter,newTS);
  return true;
}
bool MacroManager::swapTaskStreams(TaskStream* a, TaskStream* b)
{
  if (a==NULL || b==NULL) return false;
  std::list<TaskStream*>::iterator a_iter = taskStreams.end();
  std::list<TaskStream*>::iterator b_iter = taskStreams.end();
  for(std::list<TaskStream*>::iterator i=taskStreams.begin();i!=taskStreams.end();i++)
  {
    if (*i==a)
      a_iter = i;
    if (*i==b)
      b_iter = i;
  }
  if (a_iter == taskStreams.end() || b_iter == taskStreams.end()) return false;
  *a_iter = b;
  *b_iter = a;
  return true;
}
const std::set<TaskStream*>& MacroManager::getTaskStreams(BWAPI::Unit* unit) const
{
  //returns the set of task streams that are using this worker
  std::map<BWAPI::Unit*, std::set<TaskStream*> >::const_iterator i=unitToTaskStreams.find(unit);

  if (i==unitToTaskStreams.end()) //nothing is using this worker, so return an empty set
    return emptyTSSet;

  //return the set of tsak streams
  return i->second;
}