#include <BuildManager.h>
#include <BuildingPlacer.h>
#include <ConstructionManager.h>
#include <ProductionManager.h>
#include <MorphManager.h>
BuildManager::BuildManager()
{
	this->arbitrator = &Arbitrator::Arbitrator<BWAPI::Unit*,double>::Instance();
	this->buildingPlacer= & BuildingPlacer::Instance();
	this->constructionManager= & ConstructionManager::Instance();
	this->productionManager= & ProductionManager::Instance();
	this->morphManager= & MorphManager::Instance();
}

BuildManager::~BuildManager()
{
	BuildingPlacer::Destroy();
  ConstructionManager::Destroy();
	ProductionManager::Destroy();
  MorphManager::Destroy();
}

void BuildManager::update()
{
  this->constructionManager->update();
  this->productionManager->update();
  this->morphManager->update();
}

std::string BuildManager::getName() const
{
  return "Build Manager";
}

BuildingPlacer* BuildManager::getBuildingPlacer() const
{
  return this->buildingPlacer;
}
void BuildManager::onUnitDestroy(BWAPI::Unit* unit)
{
  this->constructionManager->onUnitDestroy(unit);
  this->productionManager->onUnitDestroy(unit);
  this->morphManager->onUnitDestroy(unit);
}

bool BuildManager::build(BWAPI::UnitType type)
{
  return build(type, BWAPI::Broodwar->self()->getStartLocation());
}

bool BuildManager::build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition)
{
  if (type==BWAPI::UnitTypes::None || type==BWAPI::UnitTypes::Unknown) return false;
  if (type.getRace()==BWAPI::Races::Zerg && type.isBuilding()==type.whatBuilds().first.isBuilding())
    return this->morphManager->morph(type);
  else
  {
    if (type.isBuilding())
      return this->constructionManager->build(type, goalPosition);
    else
      return this->productionManager->train(type);
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