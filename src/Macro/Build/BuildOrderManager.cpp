#include <BuildOrderManager.h>

BuildOrderManager::BuildOrderManager()
: BaseObject("BuildOrderManager")
{
	this->buildManager   = & BuildManager::Instance();
	this->techManager    = & TechManager::Instance();
	this->upgradeManager = & UpgradeManager::Instance();
	this->productionManager = & ProductionManager::Instance();
	this->usedMinerals   = 0;
	this->usedGas        = 0;
}

BuildOrderManager::~BuildOrderManager()
{
}

void BuildOrderManager::update()
{
	//BWAPI::Broodwar->printf("minerals = %i", BWAPI::Broodwar->self()->cumulativeMinerals()-this->usedMinerals);
  if (items.empty()) return;
  std::map< int, std::list<BuildItem> >::iterator l=items.end();
  l--;
  while (l->second.empty())
  {
    items.erase(l);
    if (items.empty()) return;
    l=items.end();
    l--;
  }
  int o=5;
  std::list<BuildItem>::iterator i_next;
  for(std::list<BuildItem>::iterator i=l->second.begin();i!=l->second.end();i=i_next)
  {
    i_next=i;
    i_next++;
    if (i->unitType!=BWAPI::UnitTypes::None)
    {
      if (i->isAdditional)
      {
        BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Screen,5,o,"buildAdditional(%d, \"%s\", %d)",i->count,i->unitType.getName().c_str(),l->first);
        if (i->count>0)
        {
          if (this->hasResources(i->unitType))
          {
            this->buildManager->build(i->unitType,i->seedPosition);
            this->spendResources(i->unitType);
            i->count--;
          }
        }
        else
        {
          l->second.erase(i);
        }
      }
      else
      {
        BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Screen,5,o,"build(%d, \"%s\", %d)",i->count,i->unitType.getName().c_str(),l->first);
        if (this->buildManager->getPlannedCount(i->unitType)>=i->count)
          l->second.erase(i);
        else
          if (this->hasResources(i->unitType))
          {
            this->buildManager->build(i->unitType,i->seedPosition);
            this->spendResources(i->unitType);
          }
      }
    }
    else if (i->techType!=BWAPI::TechTypes::None)
    {
      BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Screen,5,o,"research(\"%s\", %d)",i->techType.getName().c_str(),l->first);
      if (this->techManager->planned(i->techType))
        l->second.erase(i);
      else
        if (this->hasResources(i->techType))
        {
          this->techManager->research(i->techType);
          this->spendResources(i->techType);
        }
    }
    else
    {
      BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Screen,5,o,"upgrade(%d, \"%s\", %d)",i->count,i->upgradeType.getName().c_str(),l->first);
      if (this->upgradeManager->getPlannedLevel(i->upgradeType)>=i->count)
        l->second.erase(i);
      else
        if (!BWAPI::Broodwar->self()->isUpgrading(i->upgradeType) && this->hasResources(i->upgradeType))
        {
          this->upgradeManager->upgrade(i->upgradeType);
          this->spendResources(i->upgradeType);
        }
    }
    o+=20;
  }
}

std::string BuildOrderManager::getName() const
{
  return "Build Order Manager";
}
void BuildOrderManager::build(int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedPosition)
{
  if (t == BWAPI::UnitTypes::None || t == BWAPI::UnitTypes::Unknown) return;
  if (seedPosition == BWAPI::TilePositions::None || seedPosition == BWAPI::TilePositions::Unknown)
    seedPosition=BWAPI::Broodwar->self()->getStartLocation();

  BuildItem newItem;
  newItem.unitType=t;
  newItem.techType=BWAPI::TechTypes::None;
  newItem.upgradeType=BWAPI::UpgradeTypes::None;
  newItem.count=count;
  newItem.seedPosition=seedPosition;
  newItem.isAdditional=false;
  items[priority].push_back(newItem);
}

void BuildOrderManager::buildAdditional(int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedPosition)
{
  if (t == BWAPI::UnitTypes::None || t == BWAPI::UnitTypes::Unknown) return;
  if (seedPosition == BWAPI::TilePositions::None || seedPosition == BWAPI::TilePositions::Unknown)
    seedPosition=BWAPI::Broodwar->self()->getStartLocation();

  BuildItem newItem;
  newItem.unitType=t;
  newItem.techType=BWAPI::TechTypes::None;
  newItem.upgradeType=BWAPI::UpgradeTypes::None;
  newItem.count=count;
  newItem.seedPosition=seedPosition;
  newItem.isAdditional=true;
  items[priority].push_back(newItem);
}

void BuildOrderManager::research(BWAPI::TechType t, int priority)
{
  if (t==BWAPI::TechTypes::None || t==BWAPI::TechTypes::Unknown) return;
  BuildItem newItem;
  newItem.unitType=BWAPI::UnitTypes::None;
  newItem.techType=t;
  newItem.upgradeType=BWAPI::UpgradeTypes::None;
  newItem.count=1;
  newItem.isAdditional=false;
  items[priority].push_back(newItem);
}

void BuildOrderManager::upgrade(int level, BWAPI::UpgradeType t, int priority)
{
  if (t==BWAPI::UpgradeTypes::None || t==BWAPI::UpgradeTypes::Unknown) return;
  BuildItem newItem;
  newItem.unitType=BWAPI::UnitTypes::None;
  newItem.techType=BWAPI::TechTypes::None;
  newItem.upgradeType=t;
  newItem.count=level;
  newItem.isAdditional=false;
  items[priority].push_back(newItem);
}

bool BuildOrderManager::hasResources(BWAPI::UnitType t)
{
  if (BWAPI::Broodwar->self()->cumulativeMinerals()-this->usedMinerals<t.mineralPrice())
    return false;
  if (BWAPI::Broodwar->self()->cumulativeGas()-this->usedGas<t.gasPrice())
    return false;
  return true;
}

bool BuildOrderManager::hasResources(BWAPI::TechType t)
{
  if (BWAPI::Broodwar->self()->cumulativeMinerals()-this->usedMinerals<t.mineralPrice())
    return false;
  if (BWAPI::Broodwar->self()->cumulativeGas()-this->usedGas<t.gasPrice())
    return false;
  return true;
}

bool BuildOrderManager::hasResources(BWAPI::UpgradeType t)
{
  if (BWAPI::Broodwar->self()->cumulativeMinerals()-this->usedMinerals< t.mineralPriceBase()+t.mineralPriceFactor()*(BWAPI::Broodwar->self()->getUpgradeLevel(t)-1))
    return false;
  if (BWAPI::Broodwar->self()->cumulativeGas()-this->usedGas<t.gasPriceBase()+t.gasPriceFactor()*(BWAPI::Broodwar->self()->getUpgradeLevel(t)-1))
    return false;
  return true;
}

void BuildOrderManager::spendResources(BWAPI::UnitType t)
{
  this->usedMinerals+=t.mineralPrice();
  this->usedGas+=t.gasPrice();
}

void BuildOrderManager::spendResources(BWAPI::TechType t)
{
  this->usedMinerals+=t.mineralPrice();
  this->usedGas+=t.gasPrice();
}

void BuildOrderManager::spendResources(BWAPI::UpgradeType t)
{
  this->usedMinerals+=t.mineralPriceBase()+t.mineralPriceFactor()*(BWAPI::Broodwar->self()->getUpgradeLevel(t)-1);
  this->usedGas+=t.gasPriceBase()+t.gasPriceFactor()*(BWAPI::Broodwar->self()->getUpgradeLevel(t)-1);
}

int BuildOrderManager::getPlannedCount( BWAPI::UnitType unitType)
{
	int plannedCount = 0;
	for( std::map<int, std::list<BuildItem> >::iterator it = items.begin(); it != items.end(); it++)
		for( std::list<BuildItem>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
			if( it2->isAdditional && it2->unitType == unitType)
				plannedCount += it2->count;
	return plannedCount + productionManager->getPlannedCount( unitType);
}

QWidget* BuildOrderManager::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}

void BuildOrderManager::refreshWidget(QWidget* widget) const
{
// TODO update your widget after having defined it in the previous method :)
}
