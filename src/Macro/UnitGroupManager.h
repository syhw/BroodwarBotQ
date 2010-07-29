#pragma once
#include <BWAPI.h>
#include <UnitGroup.h>
#include "CSingleton.h"
class UnitGroupManager: public CSingleton<UnitGroupManager>
{
	friend class CSingleton<UnitGroupManager>;
	
  public:
   
    void onUnitDiscover(BWAPI::Unit* unit);
    void onUnitEvade(BWAPI::Unit* unit);
    void onUnitMorph(BWAPI::Unit* unit);
    void onUnitRenegade(BWAPI::Unit* unit);
private:
	UnitGroupManager();
};
UnitGroup AllUnits();
UnitGroup SelectAll();
UnitGroup SelectAll(BWAPI::UnitType type);
UnitGroup SelectAllEnemy();
UnitGroup SelectAllEnemy(BWAPI::UnitType type);
UnitGroup SelectAll(BWAPI::Player* player, BWAPI::UnitType type);