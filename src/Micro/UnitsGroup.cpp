#include <PrecompiledHeader.h>
#include <Defines.h>
#include "Micro/UnitsGroup.h"
#include "Utils/Util.h"
#include <stack>
#include <typeinfo>

int round(double a)
{
	return int(a + 0.5);
}

using namespace BWAPI;

//#define __LEADING_UNIT_BY_SIZE_HP__
//#define __WITH_RETREAT__
#define __MAX_DISTANCE_TO_GROUP__ 13*TILE_SIZE

UnitsGroup::UnitsGroup()
: defaultTargetEnemy(NULL)
, groupTargetPosition(Positions::None)
, distToTarget(-1.0)
, _totalHP(0)
, _totalMinPrice(0)
, _totalGasPrice(0)
, _totalSupply(0)
, _backFrame(-200)
, _offensiveFrame(-200)
, nonFlyers(0)
, groupMode(MODE_MOVE)
, _hasDetection(0)
, centerSpeed(0,0)
, center(0,0)
, groupAltitude(0)
, stdDevRadius(0.0)
, maxRadius(0.0)
, enemiesCenter(0,0)
, enemyStatic(false)
, enemiesAltitude(0)
, suicide(false)
, readyToAttack(false)
, nearestChoke(NULL)
, force(1.0)
, enemiesDefinedByGoal(false)
{
	_eUnitsFilter = & EUnitsFilter::Instance();
	enemies.clear();
}

UnitsGroup::~UnitsGroup()
{
	for each (pBayesianUnit bu in units)
		bu->setUnitsGroup(NULL);
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
	if (from.empty())
		return;
	assert(from.size() != 0);
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
			double max = -DBL_MAX+1;
			unsigned int max_i = 0;
			unsigned int max_j = 0;
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
			double max = -DBL_MAX+1;
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
			temp.push_back(i_dist(ind_to, (*f).getApproxDistance(to[ind_to])));
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
			// TODO
			// TODO
			// TODO
			// TODO
			// TODO
		}
	}
}

#ifdef __MICRO_DEBUG__
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
			if (u->oorTargetEnemy && u->oorTargetEnemy->exists() && u->oorTargetEnemy->isVisible())
			{
				ex = u->oorTargetEnemy->getPosition().x(); int ey = u->oorTargetEnemy->getPosition().y();
				BWAPI::Broodwar->drawLineMap(ux,uy,ex,ey,Colors::Purple);
			}
		}
	}
}
#endif

double UnitsGroup::evaluateForces()
{
	if (enemies.empty())
		return 10000.0;
	bool onlyInvisibles = true;
	int theirMinPrice = 0;
	int theirGasPrice = 0;
	int theirSupply = 0; // this is double supply: 1 zergling = 1 supply
	for (std::map<Unit*, Position>::const_iterator it = enemies.begin();
		it != enemies.end(); ++it)
	{
		UnitType ut = _eUnitsFilter->getViewedUnit(it->first).type;
		if (ut.isBuilding() && it->first->isCompleted())
		{
			// consider ground defenses only because we have only ground atm
			if (ut == UnitTypes::Protoss_Photon_Cannon || ut == UnitTypes::Terran_Bunker)
			{
				theirMinPrice += 4*ut.mineralPrice();
			} else if (ut == UnitTypes::Zerg_Sunken_Colony)
			{
				theirMinPrice += 4*(ut.mineralPrice() + 50);
			} else if (ut == UnitTypes::Protoss_Shield_Battery)
			{
				theirMinPrice += ut.mineralPrice();
			}
			continue;
		}
		if (ut == UnitTypes::Zerg_Overlord)
			continue;
		if (it->first && it->first->exists() && it->first->isVisible() && it->first->isDetected()
			|| (ut != UnitTypes::Protoss_Dark_Templar && ut != UnitTypes::Zerg_Lurker))
		    onlyInvisibles = false;
		theirMinPrice += ut.mineralPrice();
		theirGasPrice += ut.gasPrice();
		theirSupply += ut.supplyRequired();
		if (ut == UnitTypes::Terran_Siege_Tank_Siege_Mode) // a small boost for sieged tanks
			theirSupply += ut.supplyRequired();
		if (_eUnitsFilter->getInvisibleUnits().count(it->first)) // invisibles not detected count double
		{
			theirMinPrice += ut.mineralPrice();
			theirGasPrice += ut.gasPrice();
			theirSupply += ut.supplyRequired();
		}
	}
	// trying a simple rule: 100 minerals == 4 pop == 75 gaz == 100 pts
	double ourScore = _totalMinPrice + (4/3)*_totalGasPrice + 25*_totalSupply;
	double theirScore = theirMinPrice + (4/3)*theirGasPrice + 25*theirSupply;
	if (theirScore < 50.0)
		theirScore = 50.0;
	if (enemiesAltitude > groupAltitude)
		theirScore *= 1.5;
	if (onlyInvisibles && !_hasDetection)
	    return 0.01;
	else
		return ourScore/theirScore;
}

/// completely hacky
/// first throw
std::vector<Position> UnitsGroup::findRangePositions()
{
	std::vector<Position> ret;
	Vec dir1(center.x() - enemiesCenter.x(), center.y() - enemiesCenter.y());
	dir1.normalize();
	Vec tmp(dir1);
	dir1 *= 6*32;
	double ssin = sin(3.14159/6);
	double ccos = cos(3.14159/6);
	Vec dir2(ccos*dir1.x - ssin*dir1.y, ssin*dir1.x + ccos*dir1.y); // 30deg rot
	Vec dir3(ccos*dir1.x + ssin*dir1.y, -ssin*dir1.x + ccos*dir1.y); // -30deg rot
	Position p1 = dir1.translate(enemiesCenter);
	Position p2 = dir2.translate(enemiesCenter);
	Position p3 = dir3.translate(enemiesCenter);
	int iter = 0;
	while (iter < 50 &&
		(!MapManager::Instance().groundDamages[p1.x()/TILE_SIZE + Broodwar->mapWidth()*p1.y()/TILE_SIZE]
	&& !MapManager::Instance().groundDamages[p2.x()/TILE_SIZE + Broodwar->mapWidth()*p2.y()/TILE_SIZE]
	&& !MapManager::Instance().groundDamages[p3.x()/TILE_SIZE + Broodwar->mapWidth()*p3.y()/TILE_SIZE]))
	{
		dir1.normalize();
		dir3.normalize();
		dir2.normalize();
		dir1 *= 51; // > sqrt(2*TILE_SIZE^2) and < 2*TILE_SIZE
		dir2 *= 51;
		dir3 *= 51;
		p1 = dir1.translate(p1);
		p2 = dir2.translate(p2);
		p3 = dir3.translate(p3);
		++iter;
	}
	if (iter < 50)
	{
		tmp *= 3*32;
		p1 = tmp.translate(p1);
		p2 = tmp.translate(p2);
		p3 = tmp.translate(p3);
		TilePosition tp1(p1);
		TilePosition tp2(p2);
		TilePosition tp3(p3);
		if (!MapManager::Instance().isBTWalkable(tp1))
			tp1 = MapManager::Instance().closestWalkableSameRegionOrConnected(tp1);
		if (!MapManager::Instance().isBTWalkable(tp2))
			tp2 = MapManager::Instance().closestWalkableSameRegionOrConnected(tp2);
		if (!MapManager::Instance().isBTWalkable(tp3))
			tp3 = MapManager::Instance().closestWalkableSameRegionOrConnected(tp3);
		if (tp1 != TilePositions::None)
			ret.push_back(Position(tp1));
		if (tp2 != TilePositions::None)
			ret.push_back(Position(tp2));
		if (tp3 != TilePositions::None)
			ret.push_back(Position(tp3));
		Vec test(0, 0);
		for each (Position p in ret)
			test += p;
		test /= ret.size();
		if (test.toPosition().getApproxDistance(center) <= sqrt((double)units.size()*units.size()*16))
			readyToAttack = true;
	}
	else
	{
		if (Broodwar->enemy()->getRace() == Races::Zerg)
			tmp *= (8+4)*32;
		else if (Broodwar->enemy()->getRace() == Races::Terran)
			tmp *= (13+4)*32;
		else
			tmp *= (10+4)*32;
		Position seed = tmp.translate(enemiesCenter);
		if (Broodwar->isWalkable(seed.x()/8, seed.y()/8))
			seed = Position(MapManager::Instance().closestWalkableSameRegionOrConnected(TilePosition(seed)));
		ret.push_back(seed);
		if (seed.getApproxDistance(center) <= sqrt((double)units.size()*units.size()*16))
			readyToAttack = true;
	}
#ifdef __MICRO_DEBUG__
	for each (Position p in ret)
		Broodwar->drawBoxMap(p.x()-8,p.y()-8,p.x()+8,p.y()+8,Colors::Green,false);
#endif
	return ret;
}

void BasicUnitsGroup::update()
{
	for (std::vector<pBayesianUnit>::const_iterator it = units.begin();
		it != units.end(); ++it)
		(*it)->update();
}

void UnitsGroup::updateArrivingUnits()
{
	if (!arrivingUnits.empty())
	{
		if (units.size() <= 3)
		{
			for (std::list<pBayesianUnit>::iterator it = arrivingUnits.begin();
				it != arrivingUnits.end(); )
			{
				units.push_back(*it);
				arrivingUnits.erase(it++);
			}
		}
		else
		{
			for (std::list<pBayesianUnit>::iterator it = arrivingUnits.begin();
				it != arrivingUnits.end(); )
			{
				if ((*it)->unit->getPosition().getApproxDistance(center) < __MAX_DISTANCE_TO_GROUP__)
				{
					activeUnit(*it);
					arrivingUnits.erase(it++);
				}
				else
				{
					// we want them to merge with us, not to go alone to the groupTargetPosition
					// but they can interpolate our position a little before tracking
					// very dumb small heuristic (could be a real interpolation)
					if (groupTargetPosition != Positions::None)
					{
						if ((*it)->unit->getType().isFlyer())
						{
							if (center.getApproxDistance(groupTargetPosition) > (*it)->unit->getDistance(groupTargetPosition)) // do not go before the group
								(*it)->target = center;
							else // fully interpolate (without even taking any speed into account, because at worst we will arrive at the case above)
								(*it)->target = groupTargetPosition;
						}
						else // by ground, a little more complicated
						{
							BWTA::Region* r1 = BWTA::getRegion((*it)->unit->getTilePosition());
							BWTA::Region* r2 = BWTA::getRegion(TilePosition(groupTargetPosition));
							if (r1 && r2)
							{
								double tmpDist = MapManager::Instance().distRegions(r1, r2);
								if (tmpDist <= distToTarget) // we want to come from behind the group)
									(*it)->target = center;
								else
								{
									// see the max lookup we can do in the path of the unitsgroup (to come from behind him, path-wise)
									// without taking the geometry of the path into account
									double diff = tmpDist - distToTarget;
									size_t lookup = (size_t)(diff / 32);
									while (!(ppath.size() > lookup) && lookup > 0)
										--lookup;
									if (!lookup)
										(*it)->target = center;
									else
										(*it)->target = ppath[lookup - 1];
								}
							}
							else
								(*it)->target = center;
						}
					}
					else
						(*it)->target = center;
					(*it)->update();
					++it;
				}
			}
		}
	}
}

void UnitsGroup::chooseLeadingUnit()
{
	leadingUnit = units.front();
	for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
	{ 
		UnitType ut = (*it)->unit->getType();
		if ((nonFlyers && ut.isFlyer()) || ut == UnitTypes::Protoss_High_Templar || (*it)->unit->isLoaded())
			continue;
#ifdef __LEADING_UNIT_BY_SIZE_HP__
		if ((leadingUnit->unit->getType().size() < ut.size() 
			|| ( leadingUnit->unit->getType().size() == ut.size() && 
			leadingUnit->unit->getDistance(center) > (*it)->unit->getDistance(center) )) && (leadingUnit->unit->getHitPoints() + leadingUnit->unit->getShields() > 100)
			)
			leadingUnit = *it;
#else
		/// take the closest unit to the target which has the closest attack range
		if (((ppath.size() > 2 && (*it)->unit->getDistance(ppath[2]) < leadingUnit->unit->getDistance(ppath[2]))
			|| (ppath.size() > 1 && (*it)->unit->getDistance(ppath[1]) < leadingUnit->unit->getDistance(ppath[1]))
			|| (!ppath.empty() && (*it)->unit->getDistance(ppath[0]) < leadingUnit->unit->getDistance(ppath[0]))
			|| (ppath.empty() && (*it)->unit->getDistance(groupTargetPosition) < leadingUnit->unit->getDistance(groupTargetPosition)))
			&& ut.groundWeapon().maxRange() <= leadingUnit->unit->getType().groundWeapon().maxRange())
			leadingUnit = *it;
#endif
	}
	if (leadingUnit != NULL && leadingUnit->unit->exists()) // defensive prog
	{
		leadingUnit->updatePPath();
		if (!leadingUnit->getPPath().empty())
			ppath = leadingUnit->getPPath();
	}
}

void UnitsGroup::updateOurStats()
{
	int oldTotalHP = _totalHP;

	isFighting = false;
	_totalHP = 0;
	_totalMinPrice = 0;
	_totalGasPrice = 0;
	_totalSupply = 0;
	_maxRange = -1.0;
	_hasDetection = false;
	for (std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
	{
		if ((*it)->getType() == UnitTypes::Protoss_Observer)
			_hasDetection = true;
		if ((*it)->isFighting())
			isFighting = true;
		_totalHP += (*it)->unit->getHitPoints();
		_totalHP += (*it)->unit->getShields();
		UnitType ut = (*it)->unit->getType();
		_totalMinPrice += ut.mineralPrice();
		_totalGasPrice += ut.gasPrice();
		_totalSupply += ut.supplyRequired();
		double tmp_max = max(max(ut.groundWeapon().maxRange(), ut.airWeapon().maxRange()), 
			ut.sightRange()); // TODO: upgrades
		if (tmp_max > _maxRange)
			_maxRange = tmp_max;
	}

	/// Hack/Experimental++
	if (oldTotalHP - _totalHP > 150)
		readyToAttack = true;
	if (suicide || oldTotalHP - _totalHP > 300)
		suicide = true;
}

void UnitsGroup::update()
{
#ifdef __MICRO_DEBUG__
	clock_t start = clock();
#endif
	if (units.empty())
		return;

	/// Update arriving units
	updateArrivingUnits();

	/// Update the center of the group
	updateCenter(); // the center will drift towards the arriving units TOFIX TODO
#ifdef __MICRO_DEBUG__
	Broodwar->drawLineMap(center.x(), center.y(), groupTargetPosition.x(), groupTargetPosition.y(), Colors::Orange);
	Broodwar->drawLineMap(center.x()+1, center.y()+1, groupTargetPosition.x()+1, groupTargetPosition.y()+1, Colors::Orange);
#endif

	/// Choose a non flying leadingUnit
	chooseLeadingUnit();

	//// Update maxRange and _totalHP
	if (Broodwar->getFrameCount() % 13)
		updateOurStats();
	/*** TODO BUG IN SquareFormation TODO TODO TODO
	if (contactUnits)
	_alignFormation = false;
	else
	_alignFormation = true;*/

	if (!enemiesDefinedByGoal)
	{
		/// Update nearby enemy units from eUnitsFilter (SLOW)
		updateNearbyEnemyUnitsFromFilter(center, maxRadius + _maxRange + 92); // possibly hidden units, could be taken from onUnitsShow/View asynchronously for more efficiency
	}

	/// Update enemiesCenter / enemiesAltitude
	updateEnemiesCenter();
	if (enemies.empty())
	{
		readyToAttack = false;
		suicide = false;
	}

	if (groupMode == MODE_SCOUT)
	{
		for (std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
			(*it)->target = groupTargetPosition;
	}

	/// All the things to do when we have to fight
	if ((!enemies.empty() && groupMode != MODE_SCOUT)
		|| Broodwar->getFrameCount() - _backFrame < 240
		|| Broodwar->getFrameCount() - _offensiveFrame < 240)
	{
		force = evaluateForces();
		//if ((Broodwar->getFrameCount() - _backFrame < 240 && Broodwar->getFrameCount() - _offensiveFrame >= 240) || force < 0.8 && !suicide) // take decisions for 10 seconds, renewable
		if (Broodwar->getFrameCount() - _backFrame < 240 || force < 0.8 && !suicide) // take decisions for 10 seconds, renewable
		{
			if (Broodwar->getFrameCount() - _backFrame >= 240)
				_backFrame = Broodwar->getFrameCount();
			/// strategic withdrawal
			for (std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
			{
				(*it)->target = Position(Broodwar->self()->getStartLocation());
#ifdef __WITH_RETREAT__
				if ((*it)->unit->getDistance((*it)->target) >= 10*TILE_SIZE)
					(*it)->switchMode(MODE_MOVE);
				else
#endif
				{
					if ((*it)->getType().isFlyer())
						(*it)->switchMode(MODE_FIGHT_A);
					else
						(*it)->switchMode(MODE_FIGHT_G);
#ifdef __MICRO_DEBUG__
					Position displayp = (*it)->unit->getPosition();
					Broodwar->drawTextMap(displayp.x() + 8, displayp.y() + 8, "\x07 Back");
#endif
				}
			}
		}
		else
		{
			//if ((Broodwar->getFrameCount() - _offensiveFrame < 240 && Broodwar->getFrameCount() - _backFrame >= 240) || force > 1.5)
			if (Broodwar->getFrameCount() - _offensiveFrame < 240 || force > 1.5)
			{
				if (Broodwar->getFrameCount() - _offensiveFrame >= 240)
					_offensiveFrame = Broodwar->getFrameCount();
				/// we can be offensive, use our goal target and do what we want
				for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
				{
#ifdef __MICRO_DEBUG__
					Position displayp = (*it)->unit->getPosition();
					Broodwar->drawTextMap(displayp.x() + 8, displayp.y() + 8, "\x07 Offensive");
#endif
					/// target fixed by the subgoal
					if ((*it)->getType().isFlyer())
						(*it)->switchMode(MODE_FIGHT_A);
					else
						(*it)->switchMode(MODE_FIGHT_G);
				}
			}
			else /// position and go
			{
				if (enemyStatic && !readyToAttack)
				{
					/// find where to place our units around them
					std::vector<Position> bestPositions = findRangePositions();
					if (!bestPositions.empty()) // we found where to place ourselves
					{
						for (unsigned int i = 0; i < units.size(); ++i)
						{
							units[i]->target = bestPositions[i % bestPositions.size()];
							units[i]->switchMode(MODE_MOVE);
#ifdef __MICRO_DEBUG__
							Position displayp = units[i]->unit->getPosition();
							Broodwar->drawTextMap(displayp.x() + 8, displayp.y() + 8, "\x07 Place");
#endif
						}
					}
					else /// we didn't found where to place ourselves, stand our ground
					{
						for (std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
						{
							(*it)->target = (*it)->unit->getPosition();
							if ((*it)->getType().isFlyer())
								(*it)->switchMode(MODE_FIGHT_A);
							else
								(*it)->switchMode(MODE_FIGHT_G);
#ifdef __MICRO_DEBUG__
							Position displayp = (*it)->unit->getPosition();
							Broodwar->drawTextMap(displayp.x() + 8, displayp.y() + 8, "\x07 Attack");
#endif
						}
					}
				}
				else
				{
					/// ATTACK!!
					for (std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
					{
						if (enemiesAltitude > groupAltitude 
							&& nearestChoke
							&& (*it)->unit->getDistance(nearestChoke->getCenter()) < 6*TILE_SIZE)
						{
							const std::pair<BWTA::Region*, BWTA::Region*> regions = nearestChoke->getRegions();
							BWTA::Region* higherRegion = 
								(Broodwar->getGroundHeight(TilePosition(regions.first->getCenter())) > Broodwar->getGroundHeight(TilePosition(regions.second->getCenter())))
								? regions.first : regions.second;
							(*it)->target = (MapManager::Instance().regionsPFCenters(higherRegion));
							(*it)->switchMode(MODE_FIGHT_G);
#ifdef __MICRO_DEBUG__
							Position displayp = (*it)->unit->getPosition();
							Broodwar->drawTextMap(displayp.x() + 8, displayp.y() + 8, "\x07 Ramp");
#endif
						}
						else
						{	
							(*it)->target = (*it)->unit->getPosition();
							if ((*it)->getType().isFlyer())
								(*it)->switchMode(MODE_FIGHT_A);
							else
								(*it)->switchMode(MODE_FIGHT_G);
#ifdef __MICRO_DEBUG__
							Position displayp = (*it)->unit->getPosition();
							Broodwar->drawTextMap(displayp.x() + 8, displayp.y() + 8, "\x07 Attack");
#endif
						}
					}
				}
			}
		}
	}

#ifdef __MICRO_DEBUG__
	displayTargets();
	clock_t finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	if (duration > 0.040) 
		Broodwar->printf( "UnitsGroup::update() took %2.5f seconds\n", duration);
	Broodwar->drawCircleMap((int)center.x(), (int)center.y(), (int)maxRadius + (int)_maxRange +  32, Colors::Yellow);
	Broodwar->drawCircleMap((int)center.x(), (int)center.y(), (int)stdDevRadius, Colors::Orange);
#endif

	/// Update BayesianUnits
	for (std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); ++it)
		(*it)->update();

	/// Merge Templars if needed
	templarMergingStuff();
}

void UnitsGroup::move(BWAPI::Position& p)
{
	groupTargetPosition = p;
	for(std::vector<pBayesianUnit>::iterator it = this->units.begin(); it != this->units.end(); it++)
		(*it)->target = p;
}

void UnitsGroup::formation(pFormation f)
{
	if (units.empty())
		return;
	groupTargetPosition = f->center.toPosition();
	std::vector<BWAPI::Position> from;
	for(std::vector<pBayesianUnit>::iterator it = units.begin(); it != units.end(); it++)
	{
		from.push_back((*it)->unit->getPosition());
	}

	f->computeToPositions(units);

	/*** TODO BUG IN SquareFormation TODO TODO TODOif (_alignFormation)
	{*/
	const std::vector<BWAPI::Position>& to = f->end_positions;
	if (to.size() < units.size())
		return;
	std::vector<unsigned int> alignment; // alignment[from_pos] = to_pos 
	// align(from, to, alignment);// TODO min crossing || fastest
	// simple_align(from, alignment);
	mid_based_align(from, to, alignment);
	for (unsigned int i = 0; i < units.size(); i++)
	{
#ifdef __MICRO_DEBUG__
		assert (to[alignment[i]] != Positions::None);
		assert (to[alignment[i]] != Positions::Invalid);
		assert (to[alignment[i]] != Positions::Unknown);
		assert (to[alignment[i]] != Position(0, 0));
#endif
		units[i]->target = to[alignment[i]];
	}
	/*** TODO BUG IN SquareFormation TODO TODO TODO}
	else
	{
	for (unsigned int i = 0; i < units.size(); i++)
	{
	units[i]->target = f->end_positions[i];
	}
	}*/

}

void UnitsGroup::onUnitDestroy(Unit* u)
{
	if (units.empty())
		return;
	if (u->getPlayer() == Broodwar->self())
		giveUpControl(u);
	else
		unitDamages.left.erase(u); // remove from the focus fire bimap
}

void UnitsGroup::onUnitShow(Unit* u)
{
	if (u->getPlayer() == Broodwar->enemy() && u->isDetected()) //(!u->getType().isBuilding())
		unitDamages.insert(UnitDmg(u, Dmg(0, u, u->getHitPoints() + u->getShields())));
}

void UnitsGroup::onUnitHide(Unit* u)
{    
}

double UnitsGroup::getDistance(BWAPI::Unit* u) const
{
	return u->getDistance(center);
}

double UnitsGroup::getDistance(BWAPI::Position p) const
{
	return center.getApproxDistance(p);
}

void UnitsGroup::activeUnit(pBayesianUnit bu)
{
	bu->switchMode(groupMode);
	units.push_back(bu);
}

void UnitsGroup::dispatchCompleteUnit(pBayesianUnit bu)
{
	if (bu == NULL || bu->unit == NULL || !bu->unit->exists())
		return;
	bu->setUnitsGroup(this);
	if (!bu->getType().isFlyer()) // ugly
		++nonFlyers;
	if (bu->unit->getPosition().getApproxDistance(center) < __MAX_DISTANCE_TO_GROUP__ || units.empty())
		activeUnit(bu);
	else
	{
		if (bu->getType().canAttack())
			bu->unit->attack(center);
		else
			bu->unit->move(center);
		arrivingUnits.push_back(bu);
	}
}

bool UnitsGroup::removeUnit(Unit* u)
{
	for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
		if ((*it)->unit == u)
		{
			(*it)->dettachGroup();
			units.erase(it);
			if (!u->getType().isFlyer()) // ugly
				--nonFlyers;
			return true;
		}
		return false;
}

bool UnitsGroup::removeArrivingUnit(Unit* u)
{
	for (std::list<pBayesianUnit>::const_iterator it = arrivingUnits.begin();
		it != arrivingUnits.end(); ++it)
		if ((*it)->unit == u)
		{
			arrivingUnits.erase(it);
			if (!u->getType().isFlyer()) // ugly
				++nonFlyers;
			return true;
		}
		return false;
}

void UnitsGroup::giveUpControl(Unit* u)
{
	removeArrivingUnit(u);
	removeUnit(u);
	if (u->getType() == UnitTypes::Protoss_Observer)
	{
		_hasDetection = false;
		for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
			if ((*it)->unit->getType() == UnitTypes::Protoss_Observer)
			{
				_hasDetection = true;
				break;
			}
	}
}

bool BasicUnitsGroup::emptyUnits()
{
	return units.empty();
}

int UnitsGroup::getTotalHP() const
{
	return _totalHP;
}

std::vector<pBayesianUnit>* UnitsGroup::getUnits()
{
	return &units;
}

void UnitsGroup::updateNearbyEnemyUnitsFromFilter(BWAPI::Position p, double radius)
{
	// TODO Use a Quadtree (or use BWAPI's getUnitsInRadius, which uses a Quadtree)
	enemies.clear();
	// have units that have been seek like units on cliffs or lurkers before burrowing (for instance)
	for (std::map<BWAPI::Unit*, EViewedUnit>::const_iterator it = _eUnitsFilter->getViewedUnits().begin();
		it != _eUnitsFilter->getViewedUnits().end(); ++it)
	{
		if (it->second.position.getApproxDistance(p) <= radius)
			enemies.insert(std::make_pair<Unit*, Position>(it->first, it->second.position));
	}
}

const BayesianUnit& UnitsGroup::operator[](ptrdiff_t i)
{
	return *(units[i]);
}

#ifdef __MICRO_DEBUG__
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
		if( true)//unit->getPosition().getApproxDistance( target) < 200)
		{
			Broodwar->drawCircle( CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), 5, Colors::Cyan, true);
			Broodwar->drawLine( CoordinateType::Map, unit->getPosition().x(), unit->getPosition().y(), target.x(), target.y(), Colors::Cyan);
			Broodwar->drawCircle( CoordinateType::Map, target.x(), target.y(), 5, Colors::Cyan, true);
		}
	}

	//TOUNCOMMENT when formation for all units group accomplished
	//    for (std::list<pGoal>::iterator it = goals.begin(); it != goals.end(); it++)
	//    {
	//        Broodwar->drawCircle( CoordinateType::Map, (int)((*it)->formation->center.x), (int)((*it)->formation->center.y), 5, Colors::White, true);
	//    }

}
#endif

int BasicUnitsGroup::size() const
{
	return units.size();
}

void UnitsGroup::updateCenter()
{
#ifdef __MICRO_DEBUG__
	assert(this != NULL);
#endif
	if (units.empty())
		return;
	// update center
	center = Position(0, 0);
	groupAltitude = 0;
	for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
	{
		if (groupMode == MODE_FIGHT_G || groupMode == MODE_FIGHT_A)
		{
			if ((*it)->isFighting() || center == Position(0, 0))
			{
				center += (*it)->unit->getPosition();
				if (!(*it)->unit->getType().isFlyer())
					groupAltitude += Broodwar->getGroundHeight(TilePosition((*it)->unit->getPosition()));
			}
		}
		else
		{
			center += (*it)->unit->getPosition();
			if (!(*it)->unit->getType().isFlyer())
				groupAltitude += Broodwar->getGroundHeight(TilePosition((*it)->unit->getPosition()));
		}
	}
	center.x() /= units.size();
	center.y() /= units.size();
#ifdef __MICRO_DEBUG__
	///assert(center.isValid()); /// TODO that shit actually happens! BUG WHY?
#endif
	if (!center.isValid())
	{
		center = leadingUnit->unit->getPosition();
		center.makeValid();
	}

	groupAltitude = round((double)groupAltitude / units.size());
	if (nearestChoke != NULL)
		distToNearestChoke = nearestChoke->getCenter().getApproxDistance(center);
	if ((!nearestChoke || distToNearestChoke > 8*TILE_SIZE))
		nearestChoke = BWTA::getNearestChokepoint(center);

	// update stdDevRadius and maxRadius
	maxRadius = -1.0;
	double sum = 0.0;
	for (std::vector<pBayesianUnit>::const_iterator it = units.begin(); it != units.end(); ++it)
	{
		double dist = center.getApproxDistance((*it)->unit->getPosition());
		if (dist > maxRadius)
			maxRadius = dist;
		sum += (dist * dist);
	}
	stdDevRadius = sqrt((1/units.size()) * sum); // 1/(units.size() - 1) for the sample std dev

	/// update the distToTarget with the new center by using precomputer pathfinding distances
	if (groupTargetPosition == TilePositions::None)
		return;
	if (!nonFlyers)
		distToTarget = center.getApproxDistance(groupTargetPosition);
	else
	{
		BWTA::Region* r1 = BWTA::getRegion(TilePosition(center));
		BWTA::Region* r2 = BWTA::getRegion(TilePosition(groupTargetPosition));
		if (r1 && r2 && r1 != r2)
			distToTarget = MapManager::Instance().distRegions(r1, r2); // Note: distToTarget = -1 if we can't go by group from r1 to r2
		else
			distToTarget = center.getApproxDistance(groupTargetPosition); // same regions, or regions fucked up
	}
}

void UnitsGroup::updateEnemiesCenter()
{
	if (enemies.size () != 0)
	{
		int staticUnits = 0;
		int nonStaticUnits = 0;
		enemiesCenter = Position(0, 0);
		enemiesAltitude = 0;
		for (std::map<Unit*, Position>::const_iterator it = enemies.begin();
			it != enemies.end(); ++it)
		{
			enemiesCenter += it->second;
			if (!(it->first->getType().isFlyer()))
				enemiesAltitude += BWAPI::Broodwar->getGroundHeight(TilePosition(it->second));
			//if (it->first->getType().isBuilding())
			//	++staticUnits;
			//else 
			if (it->first->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
				++staticUnits;
			else if (it->first->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode)
				++nonStaticUnits;
			else if (it->first->getType() == UnitTypes::Zerg_Lurker)
			{
				if (it->first->isBurrowed())
					++staticUnits;
				else
					++nonStaticUnits;
			}
		}
		enemiesCenter.x() /= enemies.size();
		enemiesCenter.y() /= enemies.size();
		if (!enemiesCenter.isValid())
			enemiesCenter.makeValid();
		enemiesAltitude = round((double)enemiesAltitude / enemies.size());
		if (staticUnits > 1 && nonStaticUnits > 2)
			enemyStatic = staticUnits > 2*nonStaticUnits;
		else
			enemyStatic = false; // if there are only static units, we don't want to place but attack right now
	}
}

Position UnitsGroup::getCenter() const
{
	return center;
}

#ifdef __MICRO_DEBUG__
void UnitsGroup::selectedUnits(std::set<pBayesianUnit>& u)
{
	for (std::vector<pBayesianUnit>::iterator i=this->units.begin(); i!=this->units.end(); ++i)
	{
		if ((*i)->unit->isSelected())
			u.insert(*i);
	}
}
#endif

void UnitsGroup::switchMode(unit_mode um)
{
	groupMode = um;
	for(std::vector<pBayesianUnit>::iterator it = getUnits()->begin(); it != getUnits()->end(); ++it)
		(*it)->switchMode(um);
}

void UnitsGroup::idle()
{
	for(std::vector<pBayesianUnit>::iterator it = getUnits()->begin(); it != getUnits()->end(); ++it)
		(*it)->target = (*it)->unit->getPosition();
}

void UnitsGroup::signalMerge(Unit* u)
{
	if (u->getType() == UnitTypes::Protoss_High_Templar)
	{
		_mergersHT.insert(u);
	}
}

void UnitsGroup::templarMergingStuff()
{
	if (_mergersHT.size() < 2)
		return;
	Unit* tomerge = NULL;
	Unit* closer = NULL;
	double distance = DBL_MAX; 
	// stupid, suboptimal, heuristic
	for (std::set<Unit*>::iterator it = _mergersHT.begin();
		it != _mergersHT.end(); )
	{
		if (!(*it)->exists())
		{
			_mergersHT.erase(it++);
			continue;
		}
		else if (!tomerge)
			tomerge = *it;
		else
		{
			double tmp = (*it)->getDistance(tomerge);
			if (tmp < distance)
			{
				closer = *it;
				distance = tmp;
			}
		}
		++it;
	}
	if (tomerge != NULL)
	{
		if (tomerge->exists())
		{
			if (closer != NULL)
			{
				if (closer->exists())
					tomerge->useTech(TechTypes::Archon_Warp, closer);
			}
			_mergersHT.erase(closer);
		}
		_mergersHT.erase(tomerge);
	}
}
