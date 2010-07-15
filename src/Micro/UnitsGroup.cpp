#include <Defines.h>
#include <UnitsGroup.h>
#include <Position.h>
#include <algorithm>
#include <util.h>
#include <cmath>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <ArbiterUnit.h>
#include <ArchonUnit.h>
#include <CarrierUnit.h>
#include <ObserverUnit.h>
#include <CorsairUnit.h>
#include <ZealotUnit.h>
#include <ShuttleUnit.h>
#include <ScoutUnit.h>
#include <ReaverUnit.h>
#include <ProbeUnit.h>
#include <HighTemplarUnit.h>
#include <DragoonUnit.h>
#include <DarkTemplarUnit.h>
#include <DarkArchonUnit.h>
#include <ScourgeUnit.h>
#include <MutaliskUnit.h>
#include <stack>
#include <typeinfo>

using namespace BWAPI;

UnitsGroup::UnitsGroup()
{
	goalManager = & GoalManager::Instance();
}

UnitsGroup::~UnitsGroup()
{
}

bool comp_i_dist(const i_dist& l, const i_dist& r) { return (r.dist < l.dist); }

void simple_align(std::vector<Position>& from, std::vector<unsigned int>& alignment)
{
    for(unsigned int i = 0; i < from.size(); i++)
    {
        alignment.push_back(i);
    }
}

/*** 
dumb O(#units^3) keeping the relative positions w.r.t. the centers
from and to are required to be of the same size()
*/
void mid_based_align(const std::vector<Position>& from, const std::vector<Position>& to, std::vector<unsigned int>& alignment)
{
    assert(to.size() == from.size());
    Position from_center;
    Position to_center;
    for (unsigned int i = 0; i < to.size(); ++i)
    {
        from_center += from[i];
        to_center += to[i];
    }
    from_center.x() /= from.size();
    from_center.y() /= from.size();
    to_center.x() /= to.size();
    to_center.y() /= to.size();
    std::vector<Vec> v_from_center(from.size(), Vec(0, 0));
    std::vector<Vec> v_to_center(to.size(), Vec(0, 0));
    for (unsigned int i = 0; i < to.size(); ++i) 
    {
        v_from_center[i] = Vec(from[i] - from_center);
        v_to_center[i] = Vec(to[i] - to_center);
    }

    std::set<unsigned int> done_j;
    if (to.size() < 40) // magic number
    {
        for (unsigned int i = 0; i < to.size(); ++i)
            alignment.push_back(i);
        std::set<unsigned int> done_i;
        while (done_j.size() < to.size()) 
        {
            double max = -1000000000000000.0; // bad
            unsigned int max_i, max_j;
            for (unsigned int i = 0; i < v_from_center.size(); ++i)
            {
                for (unsigned int j = 0; j < v_to_center.size(); ++j)
                {
                    if ((done_i.find(i) == done_i.end()) && (done_j.find(j) == done_j.end()) 
                        && fabs(v_from_center[i].dot(v_to_center[j])) > max)
                    {
                        max = v_from_center[i].dot(v_to_center[j]);
                        max_i = i;
                        max_j = j;
                    }
                }
            } 
            alignment[max_i] = max_j;
            done_i.insert(max_i);
            done_j.insert(max_j);
        }
    } else 
    {
        // non optimal, should have one more loop
        for (unsigned int i = 0; i < v_from_center.size(); ++i)
        {
            double max = -1000000000000000.0; // bad
            unsigned int max_j;
            for (unsigned int j = 0; j < v_to_center.size(); ++j)
            {
                if ((done_j.find(j) == done_j.end()) && fabs(v_from_center[i].dot(v_to_center[j])) > max)
                {
                    max = v_from_center[i].dot(v_to_center[j]);
                    max_j = j;
                }
            }
            alignment.push_back(max_j);
            done_j.insert(max_j);
        } 
    }
}

/***
/!\ dumb O(#units^2) trying (w/o collisions) to minimize the ending time
*/
void align(std::vector<Position>& from, std::vector<Position>& to, std::vector<unsigned int>& alignment)
{
    std::vector<std::vector<i_dist> > distances;
    std::vector<std::set<unsigned int> > attempts;
    std::set<unsigned int> inner_set;
    for(unsigned int i = 0; i < to.size(); i++) inner_set.insert(i);
    for(std::vector<BWAPI::Position>::iterator f = from.begin();
        f != from.end(); f++)
    {
        std::vector<i_dist> temp;
        for(unsigned int ind_to = 0; ind_to < to.size(); ind_to++)
        {
            temp.push_back(i_dist(ind_to, (*f).getDistance(to[ind_to])));
        }
        //temp.sort();
        sort(temp.begin(), temp.end(), comp_i_dist); // [0]: further, [size()-1]: closer

        distances.push_back(temp);
        attempts.push_back(inner_set);
    }
    for(unsigned int i = 0; i < distances.size(); i++)
    {
        for(unsigned int j = 0; j < distances[0].size(); j++)
        {
            // TODO
        }
    }
}

Unit* UnitsGroup::findWeakestEnemy(std::set<Unit*> enemies_in_range)
{
    Unit* weakestenemy( enemies_in_range.empty() ? NULL : *enemies_in_range.begin() );
    for each(Unit* enemy in enemies_in_range)
    {
        int weakesthp = weakestenemy->getHitPoints() + weakestenemy->getShields();
        int enemyhp = enemy->getHitPoints() + enemy->getShields();
        if (weakesthp > enemyhp && enemy->isVisible())
        {
            weakestenemy = enemy;
        }
    }
    return weakestenemy;
}

void UnitsGroup::updateEnemiesInSight(std::vector<pBayesianUnit>::iterator it)
{
    for each(Unit* v in enemies)
    {
        if ((*it)->canHit(v))
        {
            // Si l'unité u est en train d'attaquer l'unité v, alors damageTaken = u->damagesOn(v), sinon 0
            int damageDoneByUnit =  (*it)->targetEnemy == v && (*it)->unit->getGroundWeaponCooldown() != (*it)->getTimeToAttack() && ((*it)->unit->isStartingAttack() || (*it)->unit->isAttacking()) ? (*it)->damagesOn(v) : 0;

            // Si l'unité v n'existe pas dans enemiesInSight, alors je la crée et je la rajoute
            if (enemiesInSight.count(v->getHitPoints()+v->getShields()) == 0)
            {
                cEnemy temp = {v, damageDoneByUnit};
                enemiesInSight.insert(std::pair<int, cEnemy>(v->getHitPoints()+v->getShields(), temp));
            }
            // Sinon je rajoute les dégâts à ses damageTaken
            else
            {
                enemiesInSight[v->getHitPoints()+v->getShields()].damageTaken += damageDoneByUnit;
            }
        }
    }
}

void UnitsGroup::updateUnitsAvaibles(std::vector<pBayesianUnit>::iterator it)
{
    if((*it)->unit->getGroundWeaponCooldown() == (*it)->getTimeToAttack())
        unitsAvailables.push_back(*it);
}

void UnitsGroup::updateTargetOfUnitsAvailables()
{
    for (unsigned int i = 0; i < NUMBER_OF_PRIORITY; i++)
    {
        for each(std::pair<int, cEnemy> eUnit in enemiesInSight)
        {
            for (std::list<pBayesianUnit>::iterator iter = unitsAvailables.begin(); iter != unitsAvailables.end();)
            {
                if ((*iter)->getListPriorite()[i] == eUnit.second.self->getType() && eUnit.second.damageTaken < eUnit.first && (*iter)->canHit(eUnit.second.self))
                {
                    (*iter)->targetEnemy = eUnit.second.self;
                    eUnit.second.damageTaken += (*iter)->damagesOn(eUnit.second.self);

                    std::list<pBayesianUnit>::iterator tmp = iter;
                    ++iter;/*
                    if (!(*tmp)->withinRange(eUnit.second.self))
                    {
                        copy.push_front(pBayesianUnit(*tmp));
                    }*/
                    
                    unitsAvailables.erase(tmp);
                }
                else
                {
                    ++iter;
                }
            }
        }
    }/*
    if (!copy.empty())
    {
        for (unsigned int i = 0; i < NUMBER_OF_PRIORITY; i++)
        {
            for each(std::pair<int, cEnemy> eUnit in enemiesInSight)
            {
                for (std::list<pBayesianUnit>::iterator iter = copy.begin(); iter != copy.end();)
                {
                    if ((*iter)->getListPriorite()[i] == eUnit.second.self->getType() && eUnit.second.damageTaken < eUnit.first && (*iter)->canHit(eUnit.second.self) && (*iter)->withinRange(eUnit.second.self))
                    {
                        (*iter)->targetEnemyInRange = eUnit.second.self;
                        eUnit.second.damageTaken += (*iter)->damagesOn(eUnit.second.self);
                        std::list<pBayesianUnit>::iterator tmp = iter;
                        ++iter;
                        if (!(*tmp)->withinRange(eUnit.second.self))
                        {
                            copy.push_front(*tmp);
                        }
                        
                        copy.erase(tmp);
                    }
                    else
                    {
                        ++iter;
                    }
                }
            }
        }
    }*/
}

void UnitsGroup::displayTargets()
{
    for each(pBayesianUnit u in units)
    {
        if (u->unit->getOrderTarget())
        {
            int ux = u->unit->getPosition().x(); int uy = u->unit->getPosition().y();
            int ex = u->unit->getOrderTarget()->getPosition().x(); int ey = u->unit->getOrderTarget()->getPosition().y();
            BWAPI::Broodwar->drawLineMap(ux,uy,ex,ey,Colors::Orange);
        
            if (u->targetEnemy && u->unit->getOrderTarget() != u->targetEnemy)
            {
                int ux = u->unit->getPosition().x(); int uy = u->unit->getPosition().y();
                int ex = u->targetEnemy->getPosition().x(); int ey = u->targetEnemy->getPosition().y();
                BWAPI::Broodwar->drawLineMap(ux,uy,ex,ey,Colors::Blue);
            }
        }
    }
}

void UnitsGroup::update()
{
    this->totalHP = 0;
    unitsAvailables.clear();
    enemiesInSight.clear();
    enemies = MicroManager::getEnemies();

    for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
    { 
        updateUnitsAvaibles(it);
        updateEnemiesInSight(it);
    }

    updateTargetOfUnitsAvailables();

    for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
    {
        (*it)->update();
        this->totalHP += (*it)->unit->getHitPoints();
        this->totalPower += (*it)->unit->getType().groundWeapon().damageAmount();
    }

    updateCenter();
    keepDistance(); // Temporary call to test micro tech
	accomplishGoal();

#ifdef __DEBUG_NICOLAS__
    displayTargets();
#endif

    // for each(pBayesianUnit u in units)
    //     u->micro();
}

void UnitsGroup::attackMove(int x, int y)
{
    attackMove(Position(x, y));
}

void UnitsGroup::attackMove(BWAPI::Position& p)
{
    for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); it++)
    {
        (*it)->target = p;
        (*it)->attackMove(p); // TODO, for the moment, each unit keeps a path, needs to be 1 unit per UnitsGroup + flocking
    }
}

void UnitsGroup::move(BWAPI::Position& p)
{
#ifdef __DEBUG_LOUIS__
	BWAPI::Broodwar->printf("call to move");
#endif 
    for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); it++)
    {
        (*it)->target = p;
        (*it)->unit->rightClick(p);
    }
}

void UnitsGroup::formation(pFormation f)
{
    std::vector<BWAPI::Position> from;
    std::vector<pBayesianUnit> units;
    for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); it++)
    {
        from.push_back((*it)->unit->getPosition());
    }

    f->computeToPositions(this->units);

    const std::vector<BWAPI::Position>& to = f->end_positions;

    std::vector<unsigned int> alignment; // alignment[from_pos] = to_pos 
    // align(from, to, alignment);// TODO min crossing || fastest
    // simple_align(from, alignment);
    mid_based_align(from, to, alignment);
    for(unsigned int i = 0; i < this->units.size(); i++)
    {
        (this->units)[i]->target = to[alignment[i]];
        //(this->units)[i]->attackMove(to[alignment[i]]);
    }

}

void UnitsGroup::setGoals(std::list<pGoal>& goals)
{
    this->goals = goals;
	if(!this->goals.empty())
		this->goals.front()->achieve(this);
}

void UnitsGroup::addGoal(pGoal goal)
{
    this->goals.push_back(goal);
    if (goals.size() == 1 && !this->units.empty())
    this->goals.front()->achieve(this);
}


void UnitsGroup::onUnitDestroy(Unit* u)
{
    if (u->getPlayer() != Broodwar->self())
        for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
            (*it)->onUnitDestroy(u);
    else
    {
        for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
            if ( (*it)->unit == u ) 
            {
                units.erase(it);
                return;
            }
    }
}

void UnitsGroup::onUnitShow(Unit* u)
{
    for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
        (*it)->onUnitShow(u);
}

void UnitsGroup::onUnitHide(Unit* u)
{    
    for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
        (*it)->onUnitHide(u);
}

void UnitsGroup::takeControl(Unit* u)
{//TOCHECK FOR THE GOAL MANAGEMENT
    pBayesianUnit tmp;
    if (u->getType() == BWAPI::UnitTypes::Protoss_Arbiter)
        tmp = pBayesianUnit(new ArbiterUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Archon)
        tmp = pBayesianUnit(new ArchonUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Carrier)
        tmp = pBayesianUnit(new CarrierUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Corsair)
        tmp = pBayesianUnit(new CorsairUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Dark_Archon)
        tmp = pBayesianUnit(new DarkArchonUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Dark_Templar)
        tmp = pBayesianUnit(new DarkTemplarUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Dragoon)
        tmp = pBayesianUnit(new DragoonUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_High_Templar)
        tmp = pBayesianUnit(new HighTemplarUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Observer)
        tmp = pBayesianUnit(new ObserverUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Probe)
        tmp = pBayesianUnit(new ProbeUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Reaver)
        tmp = pBayesianUnit(new ReaverUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Scout)
        tmp = pBayesianUnit(new ScoutUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Shuttle)
        tmp = pBayesianUnit(new ShuttleUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Protoss_Zealot)
        tmp = pBayesianUnit(new ZealotUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Zerg_Mutalisk)
        tmp = pBayesianUnit(new MutaliskUnit(u, this));
    else if (u->getType() == BWAPI::UnitTypes::Zerg_Scourge)
        tmp = pBayesianUnit(new ScourgeUnit(u, this));
    else
        Broodwar->printf("Cette race n'est pas correctement gérée par l'IA pour le moment !");

    if (tmp != NULL)
        this->units.push_back(tmp);
	if(this->goals.size()==1){
		if(this->goals.front()->getStatus()==GS_ACHIEVED){
			goals.front()->achieve(this);
		}
	}
}

void UnitsGroup::giveUpControl(Unit* u)
{
    for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
        if ((*it)->unit == u)
        {
            units.erase(it);
            break;
        }
}

void UnitsGroup::keepDistance()
{
    if (units.empty()) return;

    // List all enemy units in a range < 500
    std::set<Unit*>& allUnits = Broodwar->getAllUnits(); 
    std::set<Unit*> enemies;
    for each (Unit* itAllUnits in allUnits)
    {
        Unit* u = itAllUnits;
        if( u->getType().isBuilding() || u->getType().isWorker()) continue;
        if (u->getPlayer()->isEnemy(Broodwar->self()) && u->getPosition().getDistance(center) < 500)
        {
            enemies.insert( u);
            Broodwar->drawTextMap( u->getPosition().x(), u->getPosition().y(), "KD");
        }
    }

    // keep distance
    for each (pBayesianUnit itUnit in units)
    {
        Unit* unit = itUnit->unit;
        if (itUnit->timeIdle >= 0) 
            itUnit->timeIdle++;
        if (itUnit->timeIdle > 75)
        {
            itUnit->timeIdle = -1;
        }

        if (enemies.empty()) continue;

        // Get the closest unit
        Unit* closestUnit = *enemies.begin();
        for each (Unit* itEnemies in enemies)
        {
            if (closestUnit->getPosition().getDistance(unit->getPosition()) > 
                itEnemies->getPosition().getDistance(unit->getPosition()))
                closestUnit = itEnemies;
        }

        int attackRange = unit->getType().groundWeapon().maxRange();
        if (closestUnit->getType().groundWeapon().maxRange() < attackRange &&
            closestUnit->getDistance(unit) < closestUnit->getType().groundWeapon().maxRange())
        {
            //(unit)->rightClick( Broodwar->self()->getStartLocation());
            itUnit->timeIdle = 0;
        }
    }
}

bool UnitsGroup::emptyUnits()
{
    return units.empty();
}
bool UnitsGroup::emptyGoals()
{
    return goals.empty();
}


unsigned int UnitsGroup::getNbUnits() const
{
    return units.size();
}

int UnitsGroup::getTotalHP() const
{
    return totalHP;
}

std::vector<pBayesianUnit>* UnitsGroup::getUnits()
{
    return &units;
}

std::map<BWAPI::Unit*, std::list<pBayesianUnit> >& UnitsGroup::getAttackersEnemy()
{
    return attackersEnemy;
}

const BayesianUnit& UnitsGroup::operator[](int i)
{
    return *(units[i]);
}

void UnitsGroup::display()
{
	
    Broodwar->drawCircle(CoordinateType::Map, center.x(), center.y(), 8, Colors::Green);
    for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); it++)
    {
        Unit* unit = (*it)->unit;
        Position target = (*it)->target;

        // Attack range
        //Broodwar->drawCircle( CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), unit->getType().groundWeapon()->maxRange(), Colors::Red);

        // Target
        if( true)//unit->getPosition().getDistance( target) < 200)
        {
            Broodwar->drawCircle( CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), 5, Colors::Cyan, true);
            Broodwar->drawLine( CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), target.x(), target.y(), Colors::Cyan);
            Broodwar->drawCircle( CoordinateType::Map, target.x(), target.y(), 5, Colors::Cyan, true);
        }

        // Goal status
        if( goals.empty() || goals.front()->getStatus() == GS_ACHIEVED)
            Broodwar->drawTextMap( unit->getPosition().x(), unit->getPosition().y(), "W");
        if( !goals.empty() && goals.front()->getStatus() == GS_IN_PROGRESS)
            Broodwar->drawTextMap( unit->getPosition().x(), unit->getPosition().y(), "P");
    }

	//TOUNCOMMENT when formation for all units group accomplished
//    for (std::list<pGoal>::iterator it = goals.begin(); it != goals.end(); it++)
//    {
//        Broodwar->drawCircle( CoordinateType::Map, (int)((*it)->formation->center.x), (int)((*it)->formation->center.y), 5, Colors::White, true);
//    }
	
}

int UnitsGroup::size()
{
    return units.size();
}

void UnitsGroup::updateCenter()
{
    center = Position(0, 0);
    for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
    {
        center += (*it)->unit->getPosition();
    }
    if( units.size() != 0)
    {
        center.x() /= units.size();
        center.y() /= units.size();
    }
}

Position UnitsGroup::getCenter() const
{
    return center;
}

#ifdef _UNITS_DEBUG
void UnitsGroup::selectedUnits(std::set<pBayesianUnit>& u)
{
    for (std::vector<pBayesianUnit>::iterator i=this->units.begin(); i!=this->units.end(); ++i)
    {
        if ((*i)->unit->isSelected())
            u.insert(*i);
    }
}
#endif


void UnitsGroup::accomplishGoal(){
	
	//TOCHECK Potential Memory Leak with accomplished goals => no thanks to smart pointers
	
	if(goals.size() > 0){
		if (!goals.front()->getStatus() == GS_ACHIEVED) {
			goals.front()->achieve(this);
		} else {
			if(goals.size() > 1 ){
				goals.pop_front();
			}
		}
	}
}