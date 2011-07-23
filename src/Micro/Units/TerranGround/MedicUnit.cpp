#include <PrecompiledHeader.h>
#include "Micro/Units/TerranGround/MedicUnit.h"
#include "Micro/UnitsGroup.h"
#include "Regions/MapManager.h"

using namespace std;
using namespace BWAPI;

ProbTables MedicUnit::_sProbTables = ProbTables(BWAPI::UnitTypes::Terran_Medic.getID());

std::set<BWAPI::UnitType> MedicUnit::setPrio;

MedicUnit::MedicUnit(BWAPI::Unit* u,UnitsGroup* ug)
: GroundUnit(u, ug, &_sProbTables)
{
    _fleeingDmg = 32; // one round of storm = 14
}

MedicUnit::~MedicUnit()
{
}

#ifdef __LEARNING_PROB_TABLES__
void MedicUnit::initProbTables()
{
	_sProbTables = ProbTables(BWAPI::UnitTypes::Terran_Medic.getID());
}
#endif

bool MedicUnit::decideToFlee()
{
    if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() 
        && Broodwar->getGroundHeight(TilePosition(targetEnemy->getPosition())) > Broodwar->getGroundHeight(TilePosition(_unitPos)))
    {
        if (_unitsGroup && _unitsGroup->nearestChoke && _unitsGroup->nearestChoke->getCenter().getDistance(_unitPos) < 128)
        {
            _fleeing = false;
            return false;
        }
    }
    // TODO complete conditions
    int diff = _lastTotalHP - (unit->getShields() + unit->getHitPoints());
    _HPLosts.push_back(diff);
    _sumLostHP += diff;
    if (_HPLosts.size() > 19)
    {
        _sumLostHP -= _HPLosts.front();
        _HPLosts.pop_front();
    }
    if (_sumLostHP > _fleeingDmg)
        _fleeing = true;
    else
        _fleeing = false;
    if (!_fleeing)
    {
        int incDmg = 0;
        for (std::set<Unit*>::const_iterator it = _targetingMe.begin();
            it != _targetingMe.end(); ++it)
        {
            if ((*it)->getDistance(_unitPos) <= (*it)->getType().groundWeapon().maxRange() + 16)
                incDmg += (*it)->getType().groundWeapon().damageAmount() * (*it)->getType().maxGroundHits();
        }
        if (incDmg + _sumLostHP > _fleeingDmg)
            _fleeing = true;
    }
    return _fleeing;
}

void MedicUnit::micro()
{
    decideToFlee();
    int currentFrame = Broodwar->getFrameCount();
    if (unit->isStartingAttack())
        _lastAttackFrame = currentFrame;
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (unit->getGroundWeaponCooldown() > Broodwar->getLatencyFrames()*2 + 2) // == (Broodwar->getLatencyFrames()+1)*2, safety
    {
        if (_fleeing)
        {
            simpleFlee();
        }
        else
        {
			unit->attack(_unitsGroup->center);
        }
		_lastAttackFrame = currentFrame;
    }
    return;
}

void MedicUnit::check()
{
}

int MedicUnit::getAttackDuration()
{
    return 1;
}

std::set<BWAPI::UnitType> MedicUnit::getSetPrio()
{
    return MedicUnit::setPrio;
}