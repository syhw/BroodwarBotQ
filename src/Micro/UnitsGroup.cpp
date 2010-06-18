#include <UnitsGroup.h>
#include <Position.h>
#include <algorithm>
#include <util.h>
#include <cmath>
#include <assert.h>
using namespace BWAPI;

UnitsGroup::UnitsGroup()
:lastGoal(NULL)
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
	for(std::vector<pBayesianUnit>::iterator it = this->units.begin();
		it != this->units.end(); ++it)
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

void UnitsGroup::formation(Formation* f)
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

void UnitsGroup::setGoals(std::list<Goal*>& goals)
{
	this->goals = goals;
	if (!lastGoal) lastGoal = goals.front();
	this->goals.front()->achieve(this);
}

void UnitsGroup::addGoal(Goal* goal)
{
	this->goals.push_back(goal);
	if (!lastGoal) lastGoal = goal;
	if (goals.size() == 1 && !this->units.empty())
		this->goals.front()->achieve(this);
}

const Goal* UnitsGroup::getLastGoal() const
{
	return lastGoal;
}

void UnitsGroup::onUnitDestroy(Unit* u)
{
    for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
    {
        if ((*it)->unit == u)
            units.erase(it);
        (*it)->onUnitDestroy(u);
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
    pBayesianUnit tmp(new BayesianUnit(u, &units));
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
	for (std::set< Unit* >::const_iterator itAllUnits = allUnits.begin(); itAllUnits != allUnits.end(); itAllUnits++)
	{
		Unit* u = *itAllUnits;
		if( u->getType().isBuilding() || u->getType().isWorker()) continue;
		if (u->getPlayer()->isEnemy(Broodwar->self()) && u->getPosition().getDistance(center) < 500)
		{
			enemies.insert( u);
			Broodwar->drawTextMap( u->getPosition().x(), u->getPosition().y(), "KD");
		}
	}

	// keep distance
	for (std::vector<pBayesianUnit>::iterator itUnit = units.begin(); itUnit != units.end(); itUnit++)
	{
		Unit* unit = (*itUnit)->unit;
		if ((*itUnit)->timeIdle >= 0) (*itUnit)->timeIdle++;
		if ((*itUnit)->timeIdle > 75)
		{
			(*itUnit)->timeIdle = -1;
			if (!goals.empty()) (*itUnit)->attackMove(goals.front()->formation->center.toPosition());
		}

		if (enemies.empty()) continue;

		// Get the closest unit
		Unit* closestUnit = *enemies.begin();
		for (std::set<Unit*>::iterator itEnemies = enemies.begin(); itEnemies != enemies.end(); itEnemies++)
		{
			if (closestUnit->getPosition().getDistance(unit->getPosition()) > 
				(*itEnemies)->getPosition().getDistance(unit->getPosition()))
				closestUnit = (*itEnemies);
		}

		int attackRange = unit->getType().groundWeapon().maxRange();
		if (closestUnit->getType().groundWeapon().maxRange() < attackRange &&
			closestUnit->getDistance(unit) < closestUnit->getType().groundWeapon().maxRange())
		{
			(unit)->rightClick( Broodwar->self()->getStartLocation());
			(*itUnit)->timeIdle = 0;
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

	for (std::list<Goal*>::iterator it = goals.begin(); it != goals.end(); it++)
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