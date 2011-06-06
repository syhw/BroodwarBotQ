#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossSpecial/ArbiterUnit.h"

std::set<BWAPI::UnitType> ArbiterUnit::setPrio;

ArbiterUnit::ArbiterUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
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

int ArbiterUnit::getAttackDuration()
{
    return 0;
}

std::set<BWAPI::UnitType> ArbiterUnit::getSetPrio()
{
    return ArbiterUnit::setPrio;
}