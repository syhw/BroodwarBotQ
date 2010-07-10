#include <BaseManager.h>

BaseManager::BaseManager()
: BaseObject("BaseManager")
{
  this->builder = NULL;
}

BaseManager::~BaseManager()
{
}

void BaseManager::setBuildOrderManager(BuildOrderManager* builder)
{
  this->builder = builder;
}

void BaseManager::update()
{
  for(std::set<Base*>::iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
  {
    if (!(*b)->isActive())
    {
      if ((*b)->getResourceDepot() == NULL)
      {
        BWAPI::TilePosition tile = (*b)->getBaseLocation()->getTilePosition();
        std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(tile.x(),tile.y());
        for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
          if ((*u)->getPlayer() == BWAPI::Broodwar->self() && (*u)->getType().isResourceDepot())
          {
            (*b)->setResourceDepot(*u);
            break;
          }
      }
      if ((*b)->getResourceDepot()!=NULL)
        if ((*b)->getResourceDepot()->isCompleted() || (*b)->getResourceDepot()->getRemainingBuildTime() < 250)
          (*b)->setActive(true);
    }
  }

  //check to see if any new base locations need to be added
  for(std::set<BWTA::BaseLocation*>::const_iterator bl = BWTA::getBaseLocations().begin(); bl != BWTA::getBaseLocations().end(); bl++)
    if (location2base.find(*bl) == location2base.end())
    {
      BWAPI::TilePosition tile = (*bl)->getTilePosition();
      std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(tile.x(), tile.y());
      for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
        if ((*u)->getPlayer() == BWAPI::Broodwar->self() && (*u)->getType().isResourceDepot())
          addBase(*bl);
    }
}

void BaseManager::addBase(BWTA::BaseLocation* location)
{
  Base* newBase = new Base(location);
  allBases.insert(newBase);
  this->location2base[location] = newBase;
}

void BaseManager::expand(BWTA::BaseLocation* location)
{
  addBase(location);
  this->builder->buildAdditional(1, BWAPI::Broodwar->self()->getRace().getCenter(),100,location->getTilePosition());
}
std::set<Base*> BaseManager::getActiveBases() const
{
  std::set<Base*> activeBases;
  for(std::set<Base*>::const_iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
    if ((*b)->isActive())
      activeBases.insert(*b);
  return activeBases;
}
std::set<Base*> BaseManager::getAllBases() const
{
  std::set<Base*> allBases;
  for(std::set<Base*>::const_iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
    allBases.insert(*b);
  return allBases;
}
std::string BaseManager::getName()
{
  return "Base Manager";
}
void BaseManager::onUnitDestroy(BWAPI::Unit* unit)
{
	if( unit->getType().isResourceDepot())
		for( std::map<BWTA::BaseLocation*,Base*>::iterator it = location2base.begin(); it != location2base.end(); it++)
			if( it->second->getResourceDepot() == unit)
			{
				allBases.erase( it->second);
				location2base.erase( it);
				break;
			}
}

QWidget* BaseManager::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}

void BaseManager::refreshWidget(QWidget* widget) const
{
// TODO update your widget after having defined it in the previous method :)
}
