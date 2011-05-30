#include <PrecompiledHeader.h>
#include <BWTA.h>
#include <ScoutManager.h>
#include <UnitGroupManager.h>
std::pair<std::list<BWTA::BaseLocation*>, double> getBestPathHelper(std::set<BWTA::BaseLocation* > baseLocations)
{
  std::pair<std::list<BWTA::BaseLocation*>, double> shortest_path;
  shortest_path.second=0;
  if (baseLocations.empty()) return shortest_path;
  if (baseLocations.size()==1)
  {
    shortest_path.first.push_back(*baseLocations.begin());
    return shortest_path;
  }
  for(std::set<BWTA::BaseLocation*>::iterator i=baseLocations.begin();i!=baseLocations.end();i++)
  {
    BWTA::BaseLocation* node=*i;
    std::set<BWTA::BaseLocation* > baseLocations2=baseLocations;
    baseLocations2.erase(*i);
    std::pair<std::list<BWTA::BaseLocation*>, double> path_result=getBestPathHelper(baseLocations2);
    double dist=path_result.second+node->getGroundDistance(path_result.first.front());
    if (dist<shortest_path.second || shortest_path.first.empty())
    {
      path_result.first.push_front(node);
      shortest_path=std::make_pair(path_result.first,dist);
    }
  }
  return shortest_path;
}

std::pair<std::list<BWTA::BaseLocation*>, double> getBestPath(std::set<BWTA::BaseLocation* > baseLocations)
{
  std::pair<std::list<BWTA::BaseLocation*>, double> shortest_path;
  shortest_path.second=0;
  if (baseLocations.empty()) return shortest_path;
  BWTA::BaseLocation* start=BWTA::getStartLocation(BWAPI::Broodwar->self());
  baseLocations.erase(start);
  if (baseLocations.size()==1)
  {
    shortest_path.first.push_back(*baseLocations.begin());
    return shortest_path;
  }
  for(std::set<BWTA::BaseLocation*>::iterator i=baseLocations.begin();i!=baseLocations.end();i++)
  {
    BWTA::BaseLocation* node=*i;
    std::set<BWTA::BaseLocation* > baseLocations2=baseLocations;
    baseLocations2.erase(*i);
    std::pair<std::list<BWTA::BaseLocation*>, double> path_result=getBestPathHelper(baseLocations2);
    double dist=start->getGroundDistance(node)+node->getGroundDistance(path_result.first.front())+path_result.second;
    if (dist<shortest_path.second || shortest_path.first.empty())
    {
      path_result.first.push_front(node);
      shortest_path=std::make_pair(path_result.first,dist);
    }
  }
  return shortest_path;
}
ScoutManager::ScoutManager(Arbitrator::Arbitrator<BWAPI::Unit*,double> *arbitrator)
{
  this->arbitrator = arbitrator;
  desiredScoutCount = 0;

  myStartLocation = BWTA::getStartLocation(BWAPI::Broodwar->self());
  std::set<BWTA::BaseLocation *> startLocations = BWTA::getStartLocations();
  startLocations.erase(myStartLocation);
  std::set<BWTA::BaseLocation *>::iterator l_next;
  for(std::set<BWTA::BaseLocation *>::iterator l = startLocations.begin(); l != startLocations.end(); l=l_next)
  {
    l_next=l;
    l_next++;
    if (myStartLocation->getGroundDistance(*l) <= 0)
    {
      startLocations.erase(*l);
    }
  }

  std::list<BWTA::BaseLocation*> path=getBestPath(startLocations).first;
  for(std::list<BWTA::BaseLocation*>::iterator p=path.begin();p!=path.end();p++)
    baseLocationsToScout.push_back(*p);
  baseLocationsExplored.insert(myStartLocation);
  this->debugMode=false;
}

void ScoutManager::onOffer(std::set<BWAPI::Unit*> units)
{
  //First find an overlord
  std::set<BWAPI::Unit*>::iterator u2;
  for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u=u2)
  {
    u2=u;
    u2++;
     //ignore if its already a scout
    if (scouts.find(*u) != scouts.end())
    {
      arbitrator->accept(this, *u);
      units.erase(u);
    }
    else if ((*u)->getType() == BWAPI::UnitTypes::Zerg_Overlord && needMoreScouts())
    {
      arbitrator->accept(this, *u);
      addScout(*u);
      units.erase(u);
    }
  }

  for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u=u2)
  {
    u2=u;
    u2++;
    //ignore if its already a scout
    if (scouts.find(*u) != scouts.end())
    {
      arbitrator->accept(this, *u);
      units.erase(u);
    }
    else if ((*u)->getType().isWorker() && needMoreScouts())
    {
      arbitrator->accept(this, *u);
      addScout(*u);
      units.erase(u);
    }
  }

  //decline remaining units
  for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
  {
    arbitrator->decline(this, *u, 0);
  }
}

void ScoutManager::onRevoke(BWAPI::Unit *unit, double bid)
{
  onRemoveUnit(unit);
}

void ScoutManager::update()
{
  if (needMoreScouts())
  {
    requestScout(/* bid = */ 30); // Bid 30.
  }
  else
  {
    int sCount = desiredScoutCount;
    if (baseLocationsExplored.size()==BWAPI::Broodwar->getStartLocations().size())
      sCount = 0;
    while ((int)scouts.size()>sCount)
    {
      arbitrator->removeBid(this, scouts.begin()->first);
      scouts.erase(scouts.begin());
    }
  }
  updateScoutAssignments();
  if (debugMode)
  {
    drawAssignments();
  }
}
void ScoutManager::setInformationManager(InformationManager* infoManager)
{
  this->informationManager = infoManager;
}

std::string ScoutManager::getName() const
{
  return "Scout Manager";
}

std::string ScoutManager::getShortName() const
{
  return "Scout";
}

void ScoutManager::onRemoveUnit(BWAPI::Unit* unit)
{
  if (scouts.find(unit) != scouts.end())
  {
    BWTA::BaseLocation* lostTarget = scouts[unit].target;
    if (baseLocationsExplored.find(lostTarget) == baseLocationsExplored.end())
    {
      baseLocationsToScout.push_back(lostTarget);
      if (debugMode)
      {
        BWAPI::Broodwar->printf("Reassigning (%d,%d)", lostTarget->getPosition().x(), lostTarget->getPosition().y());
      }
    }
    scouts.erase(unit);
  }
}

void ScoutManager::setScoutCount(int count)
{
  this->desiredScoutCount=count;
}

void ScoutManager::setDebugMode(bool debugMode)
{
  this->debugMode = debugMode;
}

void ScoutManager::drawAssignments()
{
  //draw target vector for each scout
  for (std::map<BWAPI::Unit*,ScoutData>::iterator s = scouts.begin(); s != scouts.end(); s++)
  {
    if ((*s).second.mode != ScoutData::Idle)
    {
      BWAPI::Position scoutPos = (*s).first->getPosition();
      BWAPI::Position targetPos = (*s).second.target->getPosition();
      BWAPI::Broodwar->drawLineMap(scoutPos.x(), scoutPos.y(), targetPos.x(), targetPos.y(), BWAPI::Colors::Yellow);
      BWAPI::Broodwar->drawCircleMap(scoutPos.x(), scoutPos.y(), 6, BWAPI::Colors::Yellow);
      BWAPI::Broodwar->drawCircleMap(targetPos.x(), targetPos.y(), (*s).first->getType().sightRange(), BWAPI::Colors::Yellow);
    }
  }
}

bool ScoutManager::isScouting() const
{
  return scouts.size() >= 1;
}

bool ScoutManager::needMoreScouts() const
{
  int sCount = desiredScoutCount;
  if (baseLocationsExplored.size()==BWAPI::Broodwar->getStartLocations().size())
    sCount = 0;
  return (int)scouts.size() < sCount;
}

void ScoutManager::requestScout(double bid)
{
  // Bid on all completed workers.
  std::set<BWAPI::Unit*> usefulUnits=SelectAll()(isWorker,Overlord)(isCompleted).not(isCarryingMinerals,isCarryingGas,isGatheringGas);
  arbitrator->setBid(this,usefulUnits,bid);
}

void ScoutManager::addScout(BWAPI::Unit* u)
{
  ScoutData temp;
  scouts.insert(std::make_pair(u,temp));
}

void ScoutManager::updateScoutAssignments()
{
  // Remove scout positions if the enemy is not there.
  std::map<BWAPI::Unit*, ScoutData>::iterator u;
  for(u = scouts.begin(); u != scouts.end(); u++)
  {
    if ( (*u).second.mode == ScoutData::Searching
      && (*u).first->getPosition().getDistance((*u).second.target->getPosition()) < (*u).first->getType().sightRange())
    {
      bool empty = true;
      for(int x=(*u).second.target->getTilePosition().x();x<(*u).second.target->getTilePosition().x()+4;x++)
      {
        for(int y=(*u).second.target->getTilePosition().y();y<(*u).second.target->getTilePosition().y()+3;y++)
        {
          std::set<BWAPI::Unit*> unitsOnTile = BWAPI::Broodwar->getUnitsOnTile(x,y);
          for each(BWAPI::Unit* u in unitsOnTile)
          {
            if (u->getType().isResourceDepot())
            {
              empty = false;
              break;
            }
          }
          if (!empty) break;
        }
        if (!empty) break;
      }
      if (empty)
      {
        this->informationManager->setBaseEmpty((*u).second.target);
      }
      BWTA::BaseLocation* exploredBaseLocation  = (*u).second.target;
      baseLocationsToScout.remove(exploredBaseLocation);
      
      baseLocationsExplored.insert(exploredBaseLocation);
      (*u).second.mode = ScoutData::Idle;
      if (debugMode)
      {
        BWAPI::Broodwar->printf("Sucessfully scouted (%d,%d)", exploredBaseLocation->getPosition().x(), exploredBaseLocation->getPosition().y());
      }
    }
  }

  // Set scouts to scout.
  if (baseLocationsToScout.size() > 0) // are there still positions to scout?
  {
    std::list<BWTA::BaseLocation*>::iterator p;
    for(u = scouts.begin(); u != scouts.end() && !baseLocationsToScout.empty(); u++) 
    { // for
      if ((*u).second.mode == ScoutData::Idle)
      {
        std::map<double, BWTA::BaseLocation*> distanceMap;
        for (p = baseLocationsToScout.begin(); p != baseLocationsToScout.end(); p++)
        {
          double distance = (*u).first->getPosition().getDistance((*p)->getPosition());
          distanceMap[distance] = *p;
        }
        BWTA::BaseLocation* target = distanceMap.begin()->second;
        (*u).second.mode = ScoutData::Searching;
        (*u).first->rightClick(target->getPosition());
        (*u).second.target = target;
        baseLocationsToScout.remove(target);
        if (debugMode)
        {
          BWAPI::Broodwar->printf("Scouting (%d,%d)", target->getPosition().x(), target->getPosition().y());
        }
      }
    } // for
  }

}