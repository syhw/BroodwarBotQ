#include "HighTemplarUnit.h"
#include <UnitsGroup.h>

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug):SpecialUnit(u, ug)
{
}

HighTemplarUnit::~HighTemplarUnit()
{
}

std::pair<BWAPI::Position, int> HighTemplarUnit::bestStormPos()
{
    std::set<Position> possiblePos;
    for (std::multimap<double, BWAPI::Unit*>::const_iterator it = _rangeEnemies.begin();
        it != _rangeEnemies.end(); ++it)
    {
        Position tmpPos = it->second->getPosition();
        possiblePos.insert(tmpPos);
        possiblePos.insert(Position(tmpPos.x() - 32, tmpPos.y()));
        possiblePos.insert(Position(tmpPos.x() + 32, tmpPos.y()));
        possiblePos.insert(Position(tmpPos.x(), tmpPos.y() - 32));
        possiblePos.insert(Position(tmpPos.x(), tmpPos.y() + 32));
        possiblePos.insert(Position(tmpPos.x() - 32, tmpPos.y() - 32));
        possiblePos.insert(Position(tmpPos.x() + 32, tmpPos.y() + 32));
        possiblePos.insert(Position(tmpPos.x() + 32, tmpPos.y() - 32));
        possiblePos.insert(Position(tmpPos.x() - 32, tmpPos.y() + 32));
    }
    Position bestPos;
    int max = -1000000;
    for (std::set<Position>::const_iterator it = possiblePos.begin();
        it != possiblePos.end(); ++it)
    {
        int tmp = 0;
        for (std::vector<pBayesianUnit>::const_iterator uit = _unitsGroup->units.begin();
            uit != _unitsGroup->units.end(); ++uit)
        {
            if ((*uit)->unit->getDistance(*it) < 32.0+16.0+5.0)
                --tmp;
        }
        for (std::set<BWAPI::Unit*>::const_iterator eit = _unitsGroup->enemies.begin();
            eit != _unitsGroup->enemies.end(); ++eit)
        {
            if ((*eit)->getDistance(*it) < 32.0+16.0+5.0)
                ++tmp;
        }
        if (tmp > max)
        {
            max = tmp;
            bestPos = *it;
        }
    }
    return std::pair<Position, int>(bestPos, max);
}

void HighTemplarUnit::micro()
{
    if (this->unit->getEnergy() >= 75)
    {
        updateRangeEnemies();
        std::pair<Position, int> p = bestStormPos();
        if (p.second > 2)
            unit->useTech(BWAPI::TechTypes::Psionic_Storm, p.first);
    }
    else if (_fleeing || this->unit->getEnergy() < 74)
    {
        flee();
    }
    else
    {
        fightMove();
    }
}

void HighTemplarUnit::check()
{
}

bool HighTemplarUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("HighTemplarUnit::canHit non implémenté !");
#endif
    return false;
}

int HighTemplarUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("HighTemplarUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> HighTemplarUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}