
#include <UnitsGroup.h>
#include <Position.h>
#include <algorithm>
#include <util.h>
#include <cmath>
#include <assert.h>
#include <fstream>
#include <iostream>
using namespace BWAPI;

UnitsGroup::UnitsGroup()
:lastGoal(boost::shared_ptr<Goal>())
{
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

void UnitsGroup::update()
{
    this->totalHP = 0;

    for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
    {
        (*it)->update();
        this->totalHP += (*it)->unit->getHitPoints();
        this->totalPower += (*it)->unit->getType().groundWeapon().damageAmount();
    }

    updateCenter();

    keepDistance(); // Temporary call to test micro tech

    if (!goals.empty())
    {
        if (goals.front()->status == GS_ACHIEVED) 
        {
            if (goals.size() == 1) lastGoal = goals.front();
            goals.pop_front();
            if (!goals.empty()) goals.front()->achieve(this);
        }
        else
        {
            goals.front()->checkAchievement(this);
        }
        //debug_goals(goals);
    }    

    std::set<Unit*> enemies;
    std::set<Unit*> enemies_in_range;

    for each(Unit* u in Broodwar->getAllUnits())
    {
        if ( (u->getPlayer() != Broodwar->self()) )
        {
            enemies.insert(u);
        }
    }

    double maxRangeGoon = 0.0;
    double maxRangeGoonEnemy = 0.0;
    for each(pBayesianUnit u in units)
    {
        // find enemies in range of our current dragoon
        for each(Unit* enemy in enemies)
        {
            if (maxRangeGoon == 0.0) 
            {
                maxRangeGoon = u->unit->getType().groundWeapon().maxRange();
                for each (UpgradeType upgrade in u->unit->getType().upgrades()) 
                {
                    if (upgrade == BWAPI::UpgradeTypes::Singularity_Charge)
                    {
                        maxRangeGoon *= 1.5;
                        break;
                    }
                }
            }
            if (maxRangeGoonEnemy == 0.0)
            {
                maxRangeGoonEnemy = enemy->getType().groundWeapon().maxRange();
                for each (UpgradeType upgrade in enemy->getType().upgrades()) 
                {
                    if (upgrade == BWAPI::UpgradeTypes::Singularity_Charge)
                    {
                        maxRangeGoonEnemy *= 1.5;
                        break;
                    }
                }
            }

            if (u->unit->getDistance(enemy) < maxRangeGoon) 
            {
                enemies_in_range.insert(enemy);
            }
        }
        // find weakest
        Unit* weakestenemy = NULL;
        for each(Unit* enemy in enemies_in_range)
        {
            if (weakestenemy)
            {
                int weakesthp = weakestenemy->getHitPoints() + weakestenemy->getShields();
                int enemyhp = enemy->getHitPoints() + enemy->getShields();
                if (weakesthp > enemyhp)
                {
                    weakestenemy = enemy;
                }
            }
            else
            {
                weakestenemy = enemy;
            }
        }
        // Enemy found
        if (weakestenemy)
        {
            int ux = u->unit->getPosition().x(); int uy = u->unit->getPosition().y();
            int ex = weakestenemy->getPosition().x(); int ey = weakestenemy->getPosition().y();

            Broodwar->drawLineMap(ux,uy,ex,ey,Colors::Red);
            if (u->unit->getOrderTarget() != weakestenemy && !u->unit->isMoving())
            {
                u->unit->rightClick(weakestenemy);
            }
        }
        // Enemy not found
        else
        {
            // Find the closest enemy
            Unit* closest_enemy = NULL;
            for each(Unit* enemy in enemies)
            {
                if (closest_enemy)
                {
                    if (u->unit->getDistance(closest_enemy) > u->unit->getDistance(enemy))
                    {
                        closest_enemy = enemy;
                    }
                }
                else
                {
                    closest_enemy = enemy;
                }
            }
            // Enemy found
            if (closest_enemy)
            {
                int ux = u->unit->getPosition().x(); int uy = u->unit->getPosition().y();
                int ex = closest_enemy->getPosition().x(); int ey = closest_enemy->getPosition().y();

                Broodwar->drawLineMap(ux,uy,ex,ey,Colors::Yellow);

                if (u->unit->getOrderTarget() != closest_enemy && !u->unit->isMoving())
                {
                    u->unit->rightClick(closest_enemy);
                }
            }
            // No enemy
            else
            {
                int ux = u->unit->getPosition().x(); int uy = u->unit->getPosition().y();
                int ex = u->unit->getTargetPosition().x(); int ey = u->unit->getTargetPosition().y();

                Broodwar->drawLineMap(ux,uy,ex,ey,Colors::White);

                if (static_cast<Goal>(*lastGoal).status == GS_ACHIEVED) 
                    u->unit->rightClick(Position(u->unit->getPosition().x()+10, u->unit->getPosition().y()));
            }
        }
        enemies_in_range.clear();
    }  
    // IA improve
    for each(Unit* enemy in enemies)
    {
        if (enemy->getOrderTarget() != NULL && enemy->isAttacking()) 
        {
            Unit* myUnit = enemy->getOrderTarget();
            if (myUnit->getPlayer() == Broodwar->self() && !myUnit->isMoving()) 
            {
                Vec dep(myUnit->getPosition().x() - enemy->getPosition().x(), myUnit->getPosition().y() - enemy->getPosition().y()); 
                dep = dep.normalize();
                Position newPos = Position(myUnit->getPosition().x(), myUnit->getPosition().y());;
                double distanceFromEnemy = dep.norm();
                double calc = (maxRangeGoonEnemy + 25 - enemy->getDistance(newPos))/distanceFromEnemy;
                newPos += Position(static_cast<int>(dep.x * calc), static_cast<int>(dep.y * calc));
                bool test = true;
                for each (Unit* uunit in Broodwar->unitsOnTile(newPos.x(), newPos.y()))
                {
                    if (uunit != myUnit){
                        test = false;
                        break;
                    }
                }
                double actualLife = myUnit->getHitPoints() + myUnit->getShields();
                double totalLife = myUnit->getType().maxHitPoints() + myUnit->getType().maxShields();
                double probaFuite = 0.2 + 0.8*(1-(distanceFromEnemy/maxRangeGoonEnemy));
                probaFuite = (actualLife/totalHP < 0.1 ? 0 : probaFuite);
                if (test && (actualLife/totalLife) < probaFuite) 
                {
                    myUnit->rightClick(newPos);
                    int ux = myUnit->getPosition().x(); int uy = myUnit->getPosition().y();
                    int ex = myUnit->getTargetPosition().x(); int ey = myUnit->getTargetPosition().y();
                    Broodwar->drawLineMap(ux,uy,ex,ey,Colors::Blue);
                }
            }
        }
    }
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
    if (lastGoal == boost::shared_ptr<Goal>()) lastGoal = goals.front();
    this->goals.front()->achieve(this);
}

void UnitsGroup::addGoal(pGoal goal)
{
    this->goals.push_back(goal);
    if (lastGoal == boost::shared_ptr<Goal>()) lastGoal = goal;
    if (goals.size() == 1 && !this->units.empty())
        this->goals.front()->achieve(this);
}

const pGoal UnitsGroup::getLastGoal() const
{
    return lastGoal;
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
{
    pBayesianUnit tmp(new BayesianUnit(u, this));
    this->units.push_back(tmp);
    if (this->goals.empty()) goals.push_back(lastGoal);
    if (this->goals.front() != NULL) this->goals.front()->achieve(this);
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
            //if (!goals.empty()) 
            //   itUnit->attackMove(goals.front()->formation->center.toPosition());
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

bool UnitsGroup::empty()
{
    return units.empty();
}

unsigned int UnitsGroup::getNbUnits() const
{
    return units.size();
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
        if( goals.empty() || goals.front()->status == GS_ACHIEVED)
            Broodwar->drawTextMap( unit->getPosition().x(), unit->getPosition().y(), "W");
        if( !goals.empty() && goals.front()->status == GS_IN_PROGRESS)
            Broodwar->drawTextMap( unit->getPosition().x(), unit->getPosition().y(), "P");
    }

    for (std::list<pGoal>::iterator it = goals.begin(); it != goals.end(); it++)
    {
        Broodwar->drawCircle( CoordinateType::Map, (int)((*it)->formation->center.x), (int)((*it)->formation->center.y), 5, Colors::White, true);
    }
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
