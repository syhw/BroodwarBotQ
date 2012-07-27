#include <PrecompiledHeader.h>
#include "Micro/Units/ProtossGround/DarkTemplarUnit.h"

using namespace BWAPI;

std::set<UnitType> DarkTemplarUnit::setPrio;

DarkTemplarUnit::DarkTemplarUnit(Unit* u)
: GroundUnit(u)
{
    if (setPrio.empty())
    {
        setPrio.insert(UnitTypes::Terran_Missile_Turret);
        setPrio.insert(UnitTypes::Zerg_Spore_Colony);
        setPrio.insert(UnitTypes::Protoss_High_Templar);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(UnitTypes::Zerg_Defiler);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Tank_Mode);
    }
}

DarkTemplarUnit::~DarkTemplarUnit()
{
}

void DarkTemplarUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    Broodwar->printf("DarkTemplarUnit::micro non implémenté !");
#endif
}

void DarkTemplarUnit::check()
{
}

int DarkTemplarUnit::getAttackDuration()
{
    return 0;
}

std::set<UnitType> DarkTemplarUnit::getSetPrio()
{
    return DarkTemplarUnit::setPrio;
}