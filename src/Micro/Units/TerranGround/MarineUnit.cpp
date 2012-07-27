#include <PrecompiledHeader.h>
#include "Micro/Units/TerranGround/MarineUnit.h"
#include "Micro/UnitsGroup.h"
#include "Regions/MapManager.h"

using namespace std;
using namespace BWAPI;

ProbTables MarineUnit::_sProbTables = ProbTables(UnitTypes::Terran_Marine.getID());

std::set<UnitType> MarineUnit::setPrio;

MarineUnit::MarineUnit(Unit* u)
: GroundUnit(u, &_sProbTables)
{
    if (setPrio.empty())
    {
        setPrio.insert(UnitTypes::Protoss_Reaver);
        setPrio.insert(UnitTypes::Protoss_High_Templar);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(UnitTypes::Terran_Siege_Tank_Tank_Mode);
    }
    _fleeingDmg = 24;
}

MarineUnit::~MarineUnit()
{
    if (Broodwar->getFrameCount() - _lastAttackFrame <= getAttackDuration())
        clearDamages();
}

#ifdef __LEARNING_PROB_TABLES__
void MarineUnit::initProbTables()
{
	_sProbTables = ProbTables(UnitTypes::Terran_Marine.getID());
}
#endif


bool MarineUnit::decideToFlee()
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
    if (_HPLosts.size() > 19) // summed over 20 frames
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

void MarineUnit::micro()
{
    updateTargetingMe();
    decideToFlee();
    int currentFrame = Broodwar->getFrameCount();
    if (unit->isStartingAttack())
        _lastAttackFrame = currentFrame;
    if (currentFrame - _lastAttackFrame <= getAttackDuration()) // not interrupting attacks
        return;
    if (currentFrame - _lastAttackFrame == getAttackDuration() + 1)
        clearDamages();
    updateRangeEnemies();
	/// Stim if we can

    /// Dodge storm, drag mine, drag scarab
    if (dodgeStorm() || dragMine() || dragScarab()) 
        return;

    if (unit->getGroundWeaponCooldown() <= Broodwar->getLatencyFrames() + 1)
    {
        if (!inRange(targetEnemy))
        {
            clearDamages();
        }
        /// ATTACK
        if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible() && inRange(targetEnemy))
            attackEnemyUnit(targetEnemy);
        else if (oorTargetEnemy && oorTargetEnemy->exists() && oorTargetEnemy->isVisible() && oorTargetEnemy->getDistance(_unitPos) < 92.0) // TOCHANGE 92
            attackEnemyUnit(oorTargetEnemy);
        else if (targetEnemy && targetEnemy->exists() && targetEnemy->isVisible())
            attackEnemyUnit(targetEnemy);
        else
        {
            if (currentFrame - _lastClickFrame > Broodwar->getLatencyFrames())
            {
                unit->attack(_unitsGroup->enemiesCenter);
                _lastMoveFrame = Broodwar->getFrameCount();
                _lastClickFrame = Broodwar->getFrameCount();
            }
        }
    }
    else if (unit->getGroundWeaponCooldown() > Broodwar->getLatencyFrames()*2 + 2) // == (Broodwar->getLatencyFrames()+1)*2, safety
    {
        if (_fleeing)
        {
            simpleFlee();
        }
        else
        {
            fightMove();
        }
    }
    return;
}

void MarineUnit::check()
{
}

int MarineUnit::getAttackDuration()
{
    return 1;
}

std::set<UnitType> MarineUnit::getSetPrio()
{
    return MarineUnit::setPrio;
}