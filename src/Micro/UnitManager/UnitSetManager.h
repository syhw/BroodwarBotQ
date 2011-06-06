#pragma once
#include <BWAPI.h>
#include <UnitSet.h>
#include "Utils/CSingleton.h"
class UnitSetManager: public CSingleton<UnitSetManager>
{
	friend class CSingleton<UnitSetManager>;
	
  public:
   
    void onUnitDiscover(BWAPI::Unit* unit);
    void onUnitEvade(BWAPI::Unit* unit);
    void onUnitMorph(BWAPI::Unit* unit);
    void onUnitRenegade(BWAPI::Unit* unit);
private:
	UnitSetManager();
};
UnitSet AllUnits();
UnitSet SelectAll();
UnitSet SelectAll(BWAPI::UnitType type);
UnitSet SelectAllEnemy();
UnitSet SelectAllEnemy(BWAPI::UnitType type);
UnitSet SelectAll(BWAPI::Player* player, BWAPI::UnitType type);