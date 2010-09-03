#include "ArbiterUnit.h"

std::set<BWAPI::UnitType> ArbiterUnit::setPrio;

ArbiterUnit::ArbiterUnit(BWAPI::Unit* u, UnitsGroup* ug)
: SpecialUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Dragoon);
        setPrio.insert(BWAPI::UnitTypes::Protoss_Scout);
        setPrio.insert(BWAPI::UnitTypes::Terran_Wraith);
        setPrio.insert(BWAPI::UnitTypes::Terran_Vulture_Spider_Mine);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Hydralisk);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Mutalisk);
    }
}

ArbiterUnit::~ArbiterUnit()
{
}

void ArbiterUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ArbiterUnit::micro non implémenté !");
#endif
}

void ArbiterUnit::check()
{
}

bool ArbiterUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ArbiterUnit::canHit non implémenté !");
#endif
    return false;
}

int ArbiterUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ArbiterUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> ArbiterUnit::getSetPrio()
{
    return ArbiterUnit::setPrio;
}