#include <BuildManager.h>
#include <BuildingPlacer.h>
#include <ConstructionManager.h>
#include <ProductionManager.h>
#include <MorphManager.h>
BuildManager::BuildManager()
{
	this->arbitrator = NULL;
	this->buildingPlacer= NULL;
	this->constructionManager= NULL;
	this->productionManager= NULL;
	this->morphManager= NULL;
	this->debugMode = true;
}

BuildManager::~BuildManager()
{
	BuildingPlacer::Destroy();
	ConstructionManager::Destroy();
    ProductionManager::Destroy();
    MorphManager::Destroy();
}

void BuildManager::setDependencies(
Arbitrator::Arbitrator<BWAPI::Unit*,double>* arb, BuildingPlacer * bp, ConstructionManager * cm,
ProductionManager * pm, MorphManager * mm){
	this->arbitrator = arb;
	this->buildingPlacer = bp;
	this->constructionManager = cm;
	this->productionManager = pm;
	this->morphManager = mm;
}

BuildingPlacer* BuildManager::getBuildingPlacer(){
	return this->buildingPlacer;
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