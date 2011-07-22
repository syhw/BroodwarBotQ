#include <UnitSetManager.h>

std::map<BWAPI::Unit*,BWAPI::Player*> unitOwner;
std::map<BWAPI::Unit*,BWAPI::UnitType> unitType;
std::map<BWAPI::Player*, std::map<BWAPI::UnitType,UnitSet > > data;
std::map<BWAPI::Player*, UnitSet> allOwnedUnits;
UnitSet allUnits;
BWAPI::Player* neutral;

UnitSetManager::UnitSetManager()
{
  for(std::set<BWAPI::Unit*>::iterator i=BWAPI::Broodwar->getAllUnits().begin();i!=BWAPI::Broodwar->getAllUnits().end();i++)
  {
    onUnitDiscover(*i);
  }
  neutral=NULL;
  for(std::set<BWAPI::Player*>::iterator i=BWAPI::Broodwar->getPlayers().begin();i!=BWAPI::Broodwar->getPlayers().end();i++)
  {
    if ((*i)->isNeutral())
      neutral=*i;
  }
}
void UnitSetManager::onUnitDiscover(BWAPI::Unit* unit)
{
  unitOwner[unit]=unit->getPlayer();
  unitType[unit]=unit->getType();
  data[unit->getPlayer()][unit->getType()].insert(unit);
  allOwnedUnits[unit->getPlayer()].insert(unit);
  allUnits.insert(unit);
}
void UnitSetManager::onUnitEvade(BWAPI::Unit* unit)
{
  unitOwner[unit]=unit->getPlayer();
  unitType[unit]=unit->getType();
  data[unit->getPlayer()][unit->getType()].erase(unit);
  allOwnedUnits[unit->getPlayer()].erase(unit);
  allUnits.erase(unit);
}
void UnitSetManager::onUnitMorph(BWAPI::Unit* unit)
{
  data[unitOwner[unit]][unitType[unit]].erase(unit);
  unitType[unit]=unit->getType();
  unitOwner[unit]=unit->getPlayer();
  data[unit->getPlayer()][unit->getType()].insert(unit);
}
void UnitSetManager::onUnitRenegade(BWAPI::Unit* unit)
{
  data[unitOwner[unit]][unitType[unit]].erase(unit);
  allOwnedUnits[unitOwner[unit]].erase(unit);
  unitType[unit]=unit->getType();
  unitOwner[unit]=unit->getPlayer();
  data[unit->getPlayer()][unit->getType()].insert(unit);
  allOwnedUnits[unit->getPlayer()].insert(unit);
}
UnitSet AllUnits()
{
  return allUnits;
}
UnitSet SelectAll()
{
  return allOwnedUnits[BWAPI::Broodwar->self()];
}
UnitSet SelectAll(BWAPI::UnitType type)
{
  if (type.isNeutral() && neutral!=NULL)
    return data[neutral][type];
  return data[BWAPI::Broodwar->self()][type];
}
UnitSet SelectAllEnemy()
{
  return allOwnedUnits[BWAPI::Broodwar->enemy()];
}
UnitSet SelectAllEnemy(BWAPI::UnitType type)
{
  return data[BWAPI::Broodwar->enemy()][type];
}
UnitSet SelectAll(BWAPI::Player* player, BWAPI::UnitType type)
{
  return data[player][type];
}