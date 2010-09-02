#include "CarrierUnit.h"

std::set<BWAPI::UnitType> CarrierUnit::setUnitsPrio;

CarrierUnit::CarrierUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
    if (setUnitsPrio.empty())
    {
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Corsair);
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Scout);
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Dragoon);
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Archon);
        setUnitsPrio.insert(BWAPI::UnitTypes::Protoss_Carrier);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Marine);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Ghost);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Valkyrie);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Science_Vessel);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Battlecruiser);
        setUnitsPrio.insert(BWAPI::UnitTypes::Terran_Goliath);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Hydralisk);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Devourer);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Scourge);
        setUnitsPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
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

bool CarrierUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("CarrierUnit::canHit non implémenté !");
#endif
    return false;
}

int CarrierUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("CarrierUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> CarrierUnit::getUnitsPrio()
{
    return CarrierUnit::setUnitsPrio;
}