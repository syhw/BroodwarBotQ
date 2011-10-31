#include <PrecompiledHeader.h>
#include "Macro/BuildManager.h"
#include "Macro/BuildingPlacer.h"
#include "Macro/ConstructionManager.h"
#include "Macro/ProductionManager.h"
#include "Macro/MorphManager.h"
BuildManager::BuildManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator)
{
  this->arbitrator=arbitrator;
  this->buildingPlacer=new BuildingPlacer();
  this->constructionManager=new ConstructionManager(this->arbitrator,this->buildingPlacer);
  this->productionManager=new ProductionManager(this->arbitrator,this->buildingPlacer);
  this->morphManager=new MorphManager(this->arbitrator);
  this->debugMode = false;
}

BuildManager::~BuildManager()
{
  delete this->buildingPlacer;
  delete this->constructionManager;
  delete this->productionManager;
  delete this->morphManager;
}

void BuildManager::update()
{
  this->constructionManager->update();
  this->productionManager->update();
  this->morphManager->update();
  if (this->debugMode)
  {
    for(int x=0;x<BWAPI::Broodwar->mapWidth();x++)
      for(int y=0;y<BWAPI::Broodwar->mapHeight();y++)
        if (this->buildingPlacer->isReserved(x,y))
          BWAPI::Broodwar->drawBoxMap(x*32,y*32,x*32+32,y*32+32,BWAPI::Colors::Red);
  }
}

std::string BuildManager::getName() const
{
  return "Build Manager";
}

BuildingPlacer* BuildManager::getBuildingPlacer() const
{
  return this->buildingPlacer;
}
void BuildManager::onRemoveUnit(BWAPI::Unit* unit)
{
  this->constructionManager->onRemoveUnit(unit);
  this->productionManager->onRemoveUnit(unit);
  this->morphManager->onRemoveUnit(unit);
}

bool BuildManager::build(BWAPI::UnitType type)
{
  return build(type, BWAPI::Broodwar->self()->getStartLocation(),false);
}

bool BuildManager::build(BWAPI::UnitType type, bool forceNoAddon)
{
  return build(type, BWAPI::Broodwar->self()->getStartLocation(),forceNoAddon);
}
bool BuildManager::build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition)
{
  return build(type, goalPosition,false);
}

bool BuildManager::build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition, bool forceNoAddon)
{
  if (type==BWAPI::UnitTypes::None || type==BWAPI::UnitTypes::Unknown) return false;

  //send this order off to the right sub-manager
  if (type.getRace()==BWAPI::Races::Zerg && type.isBuilding()==type.whatBuilds().first.isBuilding())
    return this->morphManager->morph(type);
  else
  {
    if (type.isBuilding())
      return this->constructionManager->build(type, goalPosition);
    else
      return this->productionManager->train(type,forceNoAddon);
  }
  return false;
}

int BuildManager::getPlannedCount(BWAPI::UnitType type) const
{
  if (type.getRace()==BWAPI::Races::Zerg && type.isBuilding()==type.whatBuilds().first.isBuilding())
    return BWAPI::Broodwar->self()->completedUnitCount(type)+this->morphManager->getPlannedCount(type);
  else
  {
    if (type.isBuilding())
      return BWAPI::Broodwar->self()->completedUnitCount(type)+this->constructionManager->getPlannedCount(type);
    else
      return BWAPI::Broodwar->self()->completedUnitCount(type)+this->productionManager->getPlannedCount(type);
  }
}

int BuildManager::getStartedCount(BWAPI::UnitType type) const
{
  if (type.getRace()==BWAPI::Races::Zerg && type.isBuilding()==type.whatBuilds().first.isBuilding())
    return BWAPI::Broodwar->self()->completedUnitCount(type)+this->morphManager->getStartedCount(type);
  else
  {
    if (type.isBuilding())
      return BWAPI::Broodwar->self()->completedUnitCount(type)+this->constructionManager->getStartedCount(type);
    else
      return BWAPI::Broodwar->self()->completedUnitCount(type)+this->productionManager->getStartedCount(type);
  }
}

int BuildManager::getCompletedCount(BWAPI::UnitType type) const
{
  return BWAPI::Broodwar->self()->completedUnitCount(type);
}

void BuildManager::setBuildDistance(int distance)
{
  this->buildingPlacer->setBuildDistance(distance);
}
BWAPI::UnitType BuildManager::getBuildType(BWAPI::Unit* unit) const
{
  BWAPI::UnitType t=this->productionManager->getBuildType(unit);
  if (t==BWAPI::UnitTypes::None)
    t=this->morphManager->getBuildType(unit);
  return t;
}
void BuildManager::setDebugMode(bool debugMode)
{
  this->debugMode=debugMode;
}