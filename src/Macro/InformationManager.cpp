#include <InformationManager.h>
#include "Util.h"
using namespace std;
using namespace BWAPI;
InformationManager::InformationManager()
{
  buildTime[Broodwar->enemy()->getRace().getCenter()]=0;
  buildTime[Broodwar->enemy()->getRace().getWorker()]=0;
  if (Broodwar->enemy()->getRace()==Races::Zerg)
  {
    buildTime[UnitTypes::Zerg_Larva]=0;
    buildTime[UnitTypes::Zerg_Overlord]=0;
  }
  startLocationCouldContainEnemy = BWTA::getStartLocations();
  startLocationCouldContainEnemy.erase(BWTA::getStartLocation(Broodwar->self()));
  scoutedAnEnemyBase = false;
  if (startLocationCouldContainEnemy.size()==1)
  {
    enemyBases.insert(*startLocationCouldContainEnemy.begin());
    scoutedAnEnemyBase = true;
  }
}
void InformationManager::onUnitDiscover(Unit* unit)
{
  savedData[unit].exists = true;
  int test2=0;
  if (!Broodwar->self()->isEnemy(unit->getPlayer())) return;
  int time=Broodwar->getFrameCount();
  UnitType type=unit->getType();
  updateBuildTime(type,time-type.buildTime());
  if (scoutedAnEnemyBase == false && unit->getType().isBuilding())
  {
    BWTA::Region* r=BWTA::getRegion(unit->getTilePosition());
    if (r->getBaseLocations().size()==1)
    {
      BWTA::BaseLocation* b = *(r->getBaseLocations().begin());
      enemyBases.insert(b);
      scoutedAnEnemyBase = true;
    }
  }
  if (unit->getType().isResourceDepot())
  {
    BWTA::BaseLocation* b=BWTA::getNearestBaseLocation(unit->getTilePosition());
    enemyBases.insert(b);
    enemyBaseCenters[b]=unit;
    scoutedAnEnemyBase = true;
  }
}
void InformationManager::onUnitEvade(Unit* unit)
{
  savedData[unit].player=unit->getPlayer();
  savedData[unit].type=unit->getType();
  savedData[unit].position=unit->getPosition();
  savedData[unit].lastSeenTime=Broodwar->getFrameCount();
}
void InformationManager::onUnitDestroy(Unit* unit)
{
  this->onUnitEvade(unit);
  savedData[unit].exists=false;
  if (!Broodwar->self()->isEnemy(unit->getPlayer())) return;
  if (unit->getType().isResourceDepot())
  {
    BWTA::BaseLocation* b=BWTA::getNearestBaseLocation(unit->getTilePosition());
    if (this->enemyBaseCenters[b]==unit)
    {
      this->enemyBases.erase(b);
      this->enemyBaseCenters.erase(b);
    }
  }
}

Player* InformationManager::getPlayer(Unit* unit) const
{
  if (unit->exists())
    return unit->getPlayer();
  map<Unit*,UnitData>::const_iterator i=savedData.find(unit);
  if (i==savedData.end())
    return NULL;
  return (*i).second.player;
}

UnitType InformationManager::getType(Unit* unit) const
{
  if (unit->exists())
    return unit->getType();
  map<Unit*,UnitData>::const_iterator i=savedData.find(unit);
  if (i==savedData.end())
    return UnitTypes::None;
  return (*i).second.type;
}

Position InformationManager::getLastPosition(Unit* unit) const
{
  if (unit->exists())
    return unit->getPosition();
  map<Unit*,UnitData>::const_iterator i=savedData.find(unit);
  if (i==savedData.end())
    return Positions::None;
  return (*i).second.position;
}

int InformationManager::getLastSeenTime(Unit* unit) const
{
  if (unit->exists())
    return Broodwar->getFrameCount();
  map<Unit*,UnitData>::const_iterator i=savedData.find(unit);
  if (i==savedData.end())
    return -1;
  return (*i).second.lastSeenTime;
}

bool InformationManager::exists(Unit* unit) const
{
  if (unit->exists())
    return true;
  map<Unit*,UnitData>::const_iterator i=savedData.find(unit);
  if (i==savedData.end())
    return false;
  return (*i).second.exists;
}

bool InformationManager::enemyHasBuilt(UnitType type) const
{
  return (buildTime.find(type)!=buildTime.end());
}

int InformationManager::getBuildTime(UnitType type) const
{
  map<UnitType, int>::const_iterator i=buildTime.find(type);
  if (i==buildTime.end())
    return -1;
  return i->second;
}

const set<BWTA::BaseLocation*>& InformationManager::getEnemyBases() const
{
  return this->enemyBases;
}

void InformationManager::setBaseEmpty(BWTA::BaseLocation* base)
{
  this->startLocationCouldContainEnemy.erase(base);
  if (startLocationCouldContainEnemy.size()==1)
  {
    enemyBases.insert(*startLocationCouldContainEnemy.begin());
    scoutedAnEnemyBase = true;
  }
}

void InformationManager::updateBuildTime(UnitType type, int time)
{
  map<UnitType, int>::iterator i=buildTime.find(type);
  if (i!=buildTime.end() && (i->second<=time || i->second==0)) return;
  buildTime[type]=time;
  if (time<0) return;
  for(map< UnitType,int>::const_iterator i=type.requiredUnits().begin();i!=type.requiredUnits().end();i++)
  {
    updateBuildTime(i->first,time-i->first.buildTime());
  }
}

InformationManager::UnitData::UnitData()
{
  position     = Positions::Unknown;
  type         = UnitTypes::Unknown;
  player       = NULL;
  lastSeenTime = -1;
  exists       = false;
}