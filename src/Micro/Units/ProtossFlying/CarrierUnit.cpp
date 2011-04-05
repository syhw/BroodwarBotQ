#include <PrecompiledHeader.h>
#include "CarrierUnit.h"

std::set<BWAPI::UnitType> CarrierUnit::setPrio;

CarrierUnit::CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Scout);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Dragoon);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Archon);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Carrier);
        setPrio.insert(BWAPI::UnitTypes::Terran_Marine);
        setPrio.insert(BWAPI::UnitTypes::Terran_Ghost);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setPrio.insert(BWAPI::UnitTypes::Terran_Valkyrie);
        setPrio.insert(BWAPI::UnitTypes::Terran_Science_Vessel);
        setPrio.insert(BWAPI::UnitTypes::Terran_Battlecruiser);
        setPrio.insert(BWAPI::UnitTypes::Terran_Goliath);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Hydralisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Devourer);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
    }
}

CarrierUnit::~CarrierUnit()
{
}

void CarrierUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("CarrierUnit::micro non implémenté !");
#endif
}

void CarrierUnit::check()
{
}

int CarrierUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> CarrierUnit::getSetPrio()
{
    return CarrierUnit::setPrio;
}