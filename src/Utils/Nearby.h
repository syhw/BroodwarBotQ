#pragma once
#include <BWAPI.h>
#include "Macro/UnitGroupManager.h"

/*********
DO NOT USE: BWAPI IS NOW USING QUADTREES FOR THAT (== BWAPI IS BETTER)
*********/

std::set<BWAPI::Unit*> nearbyUnits(BWAPI::Unit* unit, double radius = 400.0)
{
    return AllUnits().inRadius(radius, unit->getPosition());
}

std::set<BWAPI::Unit*> nearbyUnits(BWAPI::Position p, double radius = 400.0)
{
    return AllUnits().inRadius(radius, p);
}

std::set<BWAPI::Unit*> nearbyAllyUnits(BWAPI::Unit* unit, double radius = 400.0)
{
    return SelectAll().inRadius(radius, unit->getPosition());
}

std::set<BWAPI::Unit*> nearbyAllyUnits(BWAPI::Position p, double radius = 400.0)
{
    return SelectAll().inRadius(radius, p);
}

std::set<BWAPI::Unit*> nearbyEnemyUnits(BWAPI::Unit* unit, double radius = 400.0)
{
    return SelectAllEnemy().inRadius(radius, unit->getPosition());
}

std::set<BWAPI::Unit*> nearbyEnemyUnits(BWAPI::Position p, double radius = 400.0)
{
    return SelectAllEnemy().inRadius(radius, p);
}