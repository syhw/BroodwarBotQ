#include "BayesianUnit.h"
#include "Rainbow.h"
#include <utility>
#include <Defines.h>
#include <time.h>
#include <UnitsGroup.h>
#include <set>
#include <stdio.h>

//#include <boost/math/distributions/normal.hpp>
//using boost::math::normal;

//#define __OUR_PATHFINDER__
#define __EXACT_OBJ__


using namespace std;
using namespace BWAPI;

/** Attraction/Repulsion factors
- attraction fire range
- répulsion foe fire range
- follow lead in flocking mode
- group (friends) unit interaction:
  • in_position: 1) 2)
  • flocking: 1) 2)
  • fight: 1) 2) 
- repulsion area of effect damage
*/

#define _PROB_NO_UNIT_MOVE 0.9
#define _PROB_NO_FLOCK_MOVE 0.19 // TODO change for const and set specific values depending on the unit type
#define _PROB_NO_DAMAGE_MOVE 0.9
#define _PROB_NO_REPULSE_MOVE 0.7
#define _PROB_NO_WALL_MOVE 0.9999999
#define _PROB_NO_BUILDING_MOVE 0.99999
#define _PROB_GO_OBJ 0.9
#define _PROB_GO_NOT_VISIBLE 0.4

BayesianUnit::BayesianUnit(Unit* u, UnitsGroup* ug)
: BattleUnit(u)
, _pathMutex(CreateMutex( 
        NULL,                  // default security attributes
        FALSE,                 // initially not owned
        NULL)                  // unnamed mutex
)
, dir(Vec(unit->getVelocityX(), unit->getVelocityY()))
, _mode(MODE_FLOCK)
, _unitsGroup(ug)
, _ground_unit(!unit->getType().isFlyer())
, _maxDimension(max(_slarge, _sheight))
, _minDimension(min(_slarge, _sheight))
, _maxDiag(sqrt((double)(_slarge*_slarge + _sheight*_sheight)))
, _lastRightClick(unit->getPosition())
, _lastAttackOrder(0)
, _lastClickFrame(0)
, _posAtMost13FramesAgo(Position(unit->getPosition().x() + 1, unit->getPosition().y() + 1)) // we don't want posAtMost13FramesAgo  
, _posAtMost23FramesAgo(unit->getPosition())                                                // and posAtMost23FramesAgo to be equal
, _iThinkImBlocked(false)
, _lastTotalHP(unit->getHitPoints() + unit->getShields())
, _refreshPathFramerate(12)
, _maxDistWhileRefreshingPath(max(_refreshPathFramerate * _topSpeed, 45.26)) // 45.26 = sqrt(BWAPI::TILE_SIZE^2 + BWAPI::TILE_SIZE^2)
, _newPath(false)
, _attackDuration(Broodwar->getLatency())
, _inPos(Position(0, 0))
, _fleeing(false)
{
    updateDirV();
    mapManager = & MapManager::Instance();
    switchMode(_mode);
    updateAttractors();
    initDefaultProb();

    _flockProb.push_back(_PROB_NO_FLOCK_MOVE);  //FLOCK_NO
    _flockProb.push_back(0.01);                  //FLOCK_CONTACT
    _flockProb.push_back(0.22);                 //FLOCK_CLOSE
    _flockProb.push_back(0.30);                  //FLOCK_MEDIUM
    _flockProb.push_back(0.23);                 //FLOCK_FAR

    _damageProb.push_back(_PROB_NO_DAMAGE_MOVE); //DAMAGE__NO
    _damageProb.push_back(0.08);                 //DAMAGE_LOW
    _damageProb.push_back(0.015);                 //DAMAGE_MED
    _damageProb.push_back(0.005);                 //DAMAGE_HIGH

    _repulseProb.push_back(_PROB_NO_REPULSE_MOVE); // REPULSE_NO
    _repulseProb.push_back(0.2);                  // REPULSE_LOW
    _repulseProb.push_back(0.1);                  // REPULSE_HIGH

    _heightProb.push_back(0.2);                  // NORMAL_GROUND
    _heightProb.push_back(0.3);                  // HIGH_GROUND
    _heightProb.push_back(0.45);                 // VERY_HIGH_GROUND
    _heightProb.push_back(0.05);                 // UNKOWN_HEIGHT

    // old (~~safe) params
//        _flockProb.push_back(0.1);
//        _flockProb.push_back(0.05);                  //FLOCK_CONTACT
//        _flockProb.push_back(0.25);                 //FLOCK_CLOSE
//        _flockProb.push_back(0.38);                  //FLOCK_MEDIUM
//        _flockProb.push_back(0.22);                 //FLOCK_FAR

}

BayesianUnit::~BayesianUnit()
{
    CloseHandle(_pathMutex);
}

void BayesianUnit::initDefaultProb()
{
    _defaultProb.insert(make_pair(OCCUP_UNIT, _PROB_NO_UNIT_MOVE));           // P(allied_unit_in_this_case=false | we_go_in_this_case=true)
    _defaultProb.insert(make_pair(OCCUP_BLOCKING, _PROB_NO_WALL_MOVE));       // P(this_case_is_blocking=false | we_go_in_this_case=true)
    _defaultProb.insert(make_pair(OCCUP_BUILDING, _PROB_NO_BUILDING_MOVE));   // P(there_is_a_building_in_this_case=false | we_go_in_this_case=true)
    //_defaultProb.insert(make_pair(OCCUP_FLOCK, _PROB_NO_FLOCK_MOVE));       // P(there_is_flocking_attraction=false | we_go_in_this_case=true)
}

void BayesianUnit::computeDamageValues()
{
    _damageValues.clear();
    int* tab;
    for (unsigned int i = 0; i <_dirv.size(); ++i)
    {
        Position tmp = _dirv[i].translate(this->_unitPos);
        if (unit->getType().isFlyer())
            tab = mapManager->airDamages;
        else
            tab = mapManager->groundDamages;

        if (!tab[tmp.x()/32 + tmp.y()/32 * Broodwar->mapWidth()])
            _damageValues.push_back(DAMAGE_NO);
        else if (tab[tmp.x()/32 + tmp.y()/32 * Broodwar->mapWidth()] < unit->getShields())
            _damageValues.push_back(DAMAGE_LOW);
        else if (tab[tmp.x()/32 + tmp.y()/32 * Broodwar->mapWidth()] < (unit->getHitPoints()+unit->getShields()))
            _damageValues.push_back(DAMAGE_MED);
        else 
            _damageValues.push_back(DAMAGE_HIGH);
    }
}

void BayesianUnit::computeFlockValues()
{
    _flockValues.clear();
    for (unsigned int i = 0; i < _dirv.size(); ++i)
    {
        vector<flock_value> tmpv;
        Position tmp = _dirv[i].translate(this->_unitPos);
        for (vector<pBayesianUnit>::const_iterator it = 
                _unitsGroup->units.begin(); 
            it != _unitsGroup->units.end(); ++it)
        {
            if ((*it)->unit == this->unit) continue; // we don't flock with ourselves!

            Vec tmpvit((*it)->unit->getVelocityX(), 
                    (*it)->unit->getVelocityY());
            tmpvit *= tmp.getDistance((*it)->unit->getPosition()) / _topSpeed;
            // we flock with the interpolated next position of other units
            flock_value value = (flock_value)(1 + (int)tmp.getDistance(
                        tmpvit.translate((*it)->_unitPos)) / 32); // 32 pixels for a build tile
            
            if (value == FLOCK_FAR + 1) value = FLOCK_FAR; // some kind of hysteresis for FAR
            
            if (value <= FLOCK_FAR)
                tmpv.push_back(value);
            else
                tmpv.push_back(FLOCK_NO);
        }
        _flockValues.push_back(tmpv);
    }
}

void BayesianUnit::computeRepulseValues()
{
    _repulseValues.clear();
    for (unsigned int i = 0; i < _dirv.size(); ++i)
    {
        repulse_value value = REPULSE_NO;
        Position tmp = _dirv[i].translate(this->_unitPos);
        double v = 100000.0;
        for (std::set<Unit*>::const_iterator it = Broodwar->self()->getUnits().begin();
            it != Broodwar->self()->getUnits().end(); ++it)
        {
            if (*it == unit)
                continue;
            Vec tmpvit((*it)->getVelocityX(), 
                    (*it)->getVelocityY());
            tmpvit *= tmp.getDistance((*it)->getPosition()) / (*it)->getType().topSpeed();
            Position interp_pos = tmpvit.translate((*it)->getPosition());
            double otherMaxDiag = 1.4143 * max((*it)->getType().dimensionDown() - (*it)->getType().dimensionUp(),
                (*it)->getType().dimensionRight() - (*it)->getType().dimensionLeft());
            if (tmp.getDistance(interp_pos) < max(_maxDiag, otherMaxDiag)) // collision route
            {
                if (value == REPULSE_NO && (*it)->getType().size() == BWAPI::UnitSizeTypes::Small)
                    value = REPULSE_LOW;
                else if (value < REPULSE_HIGH 
                    && (*it)->getType().size() != BWAPI::UnitSizeTypes::Small)
                    value = REPULSE_HIGH;
            }
            if (!value && tmp.getDistance((*it)->getPosition()) < max(_maxDiag, otherMaxDiag))
                value = REPULSE_LOW;
        }
        _repulseValues.push_back(value);
    }
}

void BayesianUnit::switchMode(unit_mode um)
{
    _mode = um;
    switch (um) 
    {
        case MODE_FLOCK:
#ifdef __DEBUG_GABRIEL__
            Broodwar->printf("Switch FLOCK!");
#endif
            _defaultProb[OCCUP_UNIT] = 0.6;
            break;
        case MODE_INPOS:
#ifdef __DEBUG_GABRIEL__
            //Broodwar->printf("Switch INPOS!");
#endif
            _inPos = _unitPos;
            break;
        case MODE_FIGHT_G:
#ifdef __DEBUG_GABRIEL__
            Broodwar->printf("Switch FIGHT_G!");
#endif
            //unit->holdPosition();
            break;
        case MODE_SCOUT:
#ifdef __DEBUG_GABRIEL__
            //Broodwar->printf("Switch SCOUT!");
#endif
            break;
        case MODE_MOVE:
#ifdef __DEBUG_GABRIEL__
            //Broodwar->printf("Switch MOVE!");
#endif
            break;
        default:
            break;
    }
}

int BayesianUnit::getMaxDimension()
{
    return _maxDimension;
}

void BayesianUnit::straightLine(vector<Position>& ppath, 
        const Position& p_start, const Position& p_end, bool quick)
{
    ppath.clear();
    if (p_start.getDistance(p_end) < 8) return; // <=> if (p_start == p_end) return;
    // not anti-aliased :)
    vector<Vec> dirvnorm;
    for (unsigned int i = 0; i < _dirv.size(); ++i)
        dirvnorm.push_back(_dirv[i] / _dirv[i].norm());
    Position p_current = p_start;
    ppath.push_back(p_current);
    if (quick) 
    {
        for (int id = 0; p_current != p_end && id < 2; ++id)
        {
            Vec line = Vec(p_end.x() - p_current.x(), 
                p_end.y() - p_current.y()); 
            Vec objnorm = line.normalize();
            double maxv = -10.;
            unsigned int imax = 0;
            for (unsigned int i = 0; i < _dirv.size(); ++i)
            {
                double tmp = dirvnorm[i].dot(objnorm);
                if (tmp > maxv || 
                        (tmp >= maxv && _dirv[i].norm() > _dirv[imax].norm()))
                {
                    maxv = tmp;
                    imax = i;
                }
            }
            p_current = _dirv[imax].translate(p_current);
            ppath.push_back(p_current);
        }
    } 
    else // TODO: Test it, untested (bad bad bad)
    {
        while (p_current.getDistance(p_end) > 31) // 31 for BuildTile Resolution
        {
            Vec line = Vec(p_end.x() - p_current.x(), 
                p_end.y() - p_current.y()); 
            Vec objnorm = line.normalize();
            double maxv = -10.;
            unsigned int imax = 0;
            for (unsigned int i = 0; i < _dirv.size(); ++i)
            {
                double tmp = dirvnorm[i].dot(objnorm);
                if (tmp > maxv || 
                        (tmp >= maxv && _dirv[i].norm() > _dirv[imax].norm()))
                {
                    maxv = tmp;
                    imax = i;
                }
            }
            p_current = _dirv[imax].translate(p_current);
            ppath.push_back(p_current);
        }
    }
}

void BayesianUnit::updateAttractors()
{
    _occupation.clear();
    computeDamageValues();
    // flocking attractions
    if (_mode == MODE_FLOCK)
        computeFlockValues();
    else
    {
        computeRepulseValues();
        //drawRepulseValues();
    }

    // building and blocking attraction (repulsion)
    const int width = Broodwar->mapWidth();
    Position up = _unitPos;
    for (unsigned int i = 0; i < _dirv.size(); ++i)
    {
        Position tmp = _dirv[i].translate(up);
        
        // TODO optimize/clean this
        Position hG = Position(tmp.x() - this->unit->getType().dimensionUp(), tmp.y() - this->unit->getType().dimensionLeft());
        Position hD = Position(tmp.x() - this->unit->getType().dimensionUp(), tmp.y() + this->unit->getType().dimensionRight());
        Position bG = Position(tmp.x() + this->unit->getType().dimensionDown(), tmp.y() - this->unit->getType().dimensionLeft());
        Position bD = Position(tmp.x() + this->unit->getType().dimensionDown(), tmp.y() + this->unit->getType().dimensionRight());
        
        if (mapManager->buildings_wt[tmp.x()/8 + (tmp.y()/8)*4*width])
            _occupation.push_back(OCCUP_BUILDING);
		else if (!mapManager->walkability[hG.x()/8 + (hG.y()/8)*4*width])
            _occupation.push_back(OCCUP_BLOCKING);
        else if (!mapManager->walkability[bG.x()/8 + (bG.y()/8)*4*width])
            _occupation.push_back(OCCUP_BLOCKING);
        else if (!mapManager->walkability[hD.x()/8 + (hD.y()/8)*4*width])
            _occupation.push_back(OCCUP_BLOCKING);
        else if (!mapManager->walkability[bD.x()/8 + (bD.y()/8)*4*width])
            _occupation.push_back(OCCUP_BLOCKING);

        //else if (0/*TODO TEST SI Y A UNE UNITE QUI BLOQUE en TMP*/)
        //    _occupation.push_back(OCCUP_UNIT);

        else // TODO UNIT/EUNIT
            _occupation.push_back(OCCUP_NO);
    }
}

void BayesianUnit::drawAttractors()
{
    Position up = _unitPos;
    for (unsigned int i = 0; i < _dirv.size(); ++i)
    {
        Position p = _dirv[i].translate(up);
        if (!_occupation[i])
        {
            continue;
        }
        if (_occupation[i] == OCCUP_BUILDING)
            Broodwar->drawBox(CoordinateType::Map, p.x() - 2, p.y() -2, p.x() + 2, p.y() + 2, Colors::Orange, true);
        else if (_occupation[i] == OCCUP_BLOCKING)
            Broodwar->drawBox(CoordinateType::Map, p.x() - 2, p.y() - 2, p.x() + 2, p.y() + 2, Colors::Red, true);
        if (_occupation[i] == OCCUP_UNIT)
            Broodwar->drawBox(CoordinateType::Map, p.x() - 2, p.y() - 2, p.x() + 2, p.y() + 2, Colors::Green, true);
        else if (_occupation[i] == OCCUP_EUNIT)
            Broodwar->drawBox(CoordinateType::Map, p.x() - 2, p.y() - 2, p.x() + 2, p.y() + 2, Colors::Purple, true);
        //if (_occupation[i].count(OCCUP_FLOCK))
        //    Broodwar->drawBox(CoordinateType::Map, p.x() - 2, p.y() - 2, p.x() + 2, p.y() + 2, Colors::Blue, true);
    }
}

void BayesianUnit::drawFlockValues()
{
    Rainbow colors = Rainbow(Color(12, 12, 255), 51);
    for (unsigned int i = 0; i < _flockValues.size(); ++i)
    {
        Position p = _dirv[i].translate(this->_unitPos);
        for (unsigned int j = 0; j < _flockValues[i].size(); ++j)
            Broodwar->drawBox(CoordinateType::Map, p.x() - 3 + j , p.y() - 3 + j, p.x() + 1 + j, p.y() + 1 + j, colors.rainbow[(_flockValues[i][j]+4) % 5], true);
    }
}

void BayesianUnit::drawRepulseValues()
{
    Rainbow colors = Rainbow(Color(12, 12, 255), 51);
    for (unsigned int i = 0; i < _repulseValues.size(); ++i)
    {
        Position p = _dirv[i].translate(this->_unitPos);
        Broodwar->drawBox(CoordinateType::Map, p.x() - 3, p.y() - 3, p.x() + 3, p.y() + 3, colors.rainbow[(_repulseValues[i]) % 3], true);
    }
}

double BayesianUnit::computeProb(unsigned int i)
{
    double val = 1.;
    
    val *= _damageProb[_damageValues[i]];
    TilePosition tmpTilePos = TilePosition(_dirv[i].translate(_unitPos));

    if (_unitsGroup->getUnits()->size() > 1 && _mode == MODE_FLOCK)
    {
        /// FLOCKING INFLUENCE
        // one j for each attractor
        for (unsigned int j = 0; j < _flockValues[i].size(); ++j)
            val *= _flockProb[_flockValues[i][j]];

        //for (unsigned int j = 0; j < _dodgeValues[i].size(); ++j)
        //    val *= _dodgeProb[_dodgeValues[i][j]];
       
        /// OBJECTIVE (pathfinder) INFLUENCE
        double prob_obj = _PROB_GO_OBJ / (_unitsGroup->getUnits()->size() - 1);

        if (_dirv[i] == obj)
            val *= prob_obj;
        else
        {
            Vec dirvtmp = _dirv[i];
            dirvtmp.normalize();
            Vec objnorm = obj;
            objnorm.normalize();
            const double tmp = dirvtmp.dot(objnorm);
            val *= (tmp > 0 ? prob_obj*tmp : 0.01);
            // TOCHANGE 0.01 magic number (uniform prob to go in the half-quadrant opposite to obj)
        }
    }

    if (_mode != MODE_FLOCK)
    {
        if (_mode != MODE_FIGHT_G)
        {
            double prob_obj = _PROB_GO_OBJ;
            if (_dirv[i] != obj)
            {
                Position tmp = _dirv[i].translate(_unitPos);
                Position tmp_obj = obj.translate(_unitPos);
                double mult = (1.0 / (tmp.getDistance(tmp_obj)/11.32) < 1.0) ? (1.0 / (tmp.getDistance(tmp_obj)/11.32)) : 1.0;
                val *= mult;
            }
        }

        val *= _repulseProb[_repulseValues[i]];


        int height = Broodwar->getGroundHeight(tmpTilePos);
        if (height >= 0 && height <= 3)
            val *= _heightProb[height];
        else
            val *= _heightProb[_heightProb.size() - 1];
    }

    if (_occupation[i] == OCCUP_BUILDING) /// NON-WALKABLE (BUILDING) INFLUENCE
    {	
        val *= 1.0-_PROB_NO_BUILDING_MOVE;
    }
    else if (_occupation[i] == OCCUP_BLOCKING) /// NON-WALKABLE INFLUENCE
    {
        val *= 1.0-_PROB_NO_WALL_MOVE;
    }
    else if (_occupation[i] == OCCUP_UNIT)
    {
        val *= 1.0-_PROB_NO_UNIT_MOVE;
    }

    if (!Broodwar->isVisible(tmpTilePos))
        val *= _PROB_GO_NOT_VISIBLE;
    else
        val *= 1.0 - _PROB_GO_NOT_VISIBLE;
    //Broodwar->printf("val is %d \n", val);
    return val;
}

void BayesianUnit::drawProbs(multimap<double, Vec>& probs, int number)
{
    //Broodwar->printf("Size: %d\n", probs.size());
    Position up = _unitPos;
    Rainbow colors = Rainbow(Color(12, 12, 255), 51); // 5 colors
    multimap<double, Vec>::const_iterator itmax = probs.end();
    --itmax;
    double max = itmax->first;
    for (multimap<double, Vec>::const_iterator it = probs.begin(); it != probs.end(); ++it)
    {
        //Broodwar->printf("proba: %f, Vec: %f, %f\n", it->first, it->second.x, it->second.y);
        if (number == 1)
        {
            const int middle = 64;
            if (it->first < 0.2*max)
                Broodwar->drawBoxScreen(middle - 4 + (int)it->second.x, middle - 4 + (int)it->second.y,
                middle + 4 + (int)it->second.x, middle + 4 + (int)it->second.y, colors.rainbow[0], true);
            else if (it->first >= 0.2*max && it->first < 0.4*max)
                Broodwar->drawBoxScreen(middle - 4 + (int)it->second.x, middle - 4 + (int)it->second.y,
                middle + 4 + (int)it->second.x, middle + 4 + (int)it->second.y, colors.rainbow[1], true);
            else if (it->first >= 0.4*max && it->first < 0.6*max)
                Broodwar->drawBoxScreen(middle - 4 + (int)it->second.x, middle - 4 + (int)it->second.y,
                middle + 4 + (int)it->second.x, middle + 4 + (int)it->second.y, colors.rainbow[2], true);
            else if (it->first >= 0.6*max && it->first < 0.8*max)
                Broodwar->drawBoxScreen(middle - 4 + (int)it->second.x, middle - 4 + (int)it->second.y,
                middle + 4 + (int)it->second.x, middle + 4 + (int)it->second.y, colors.rainbow[3], true);
            else if (it->first >= 0.8*max)
                Broodwar->drawBoxScreen(middle - 4 + (int)it->second.x, middle - 4 + (int)it->second.y,
                middle + 4 + (int)it->second.x, middle + 4 + (int)it->second.y, colors.rainbow[4], true);
        } else
        {
            if (it->first < 0.2*max)
                Broodwar->drawBoxMap(up.x() - 4 + (int)it->second.x, up.y() - 4 + (int)it->second.y,
                up.x() + 4 + (int)it->second.x, up.y() + 4 + (int)it->second.y, colors.rainbow[0], true);
            else if (it->first >= 0.2*max && it->first < 0.4*max)
                Broodwar->drawBoxMap(up.x() - 4 + (int)it->second.x, up.y() - 4 + (int)it->second.y,
                up.x() + 4 + (int)it->second.x, up.y() + 4 + (int)it->second.y, colors.rainbow[1], true);
            else if (it->first >= 0.4*max && it->first < 0.6*max)
                Broodwar->drawBoxMap(up.x() - 4 + (int)it->second.x, up.y() - 4 + (int)it->second.y,
                up.x() + 4 + (int)it->second.x, up.y() + 4 + (int)it->second.y, colors.rainbow[2], true);
            else if (it->first >= 0.6*max && it->first < 0.8*max)
                Broodwar->drawBoxMap(up.x() - 4 + (int)it->second.x, up.y() - 4 + (int)it->second.y,
                up.x() + 4 + (int)it->second.x, up.y() + 4 + (int)it->second.y, colors.rainbow[3], true);
            else if (it->first >= 0.8*max)
                Broodwar->drawBoxMap(up.x() - 4 + (int)it->second.x, up.y() - 4 + (int)it->second.y,
                up.x() + 4 + (int)it->second.x, up.y() + 4 + (int)it->second.y, colors.rainbow[4], true);
        }
    }
}

DWORD WINAPI BayesianUnit::StaticLaunchPathfinding(void* obj)
{
    BayesianUnit* This = (BayesianUnit*) obj;
    int buf = This->LaunchPathfinding();
    ExitThread(buf);
    return buf;
}

DWORD BayesianUnit::LaunchPathfinding()
{
    DWORD waitResult = WaitForSingleObject( 
        _pathMutex,    // handle to mutex
        100);          // 100 ms time-out interval, < next pathfinding

    switch (waitResult) 
    {
        // The thread got ownership of the mutex
    case WAIT_OBJECT_0: 
        _btpath = BWTA::getShortestPath(TilePosition(_unitPos), TilePosition(target));
        _newPath = true;
        ReleaseMutex(_pathMutex);
        break; 

        // The thread got ownership of an abandoned mutex
        // The database is in an indeterminate state
    case WAIT_ABANDONED:
        _newPath = false;
        ReleaseMutex(_pathMutex);
        return -1;
    }
    return 0;
}

void BayesianUnit::updateObj()
{
    double targetDistance = _unitPos.getDistance(target);
    if (_mode == MODE_INPOS)
    {
        if (_unitPos.getDistance(_inPos) < 1.0) // TOCHANGE 1.0
            obj = Vec(0, 0);
        else
            obj = Vec(_inPos.x() - _unitPos.x(), _inPos.y() - _unitPos.y());
        return;
    }
    // if (_mode == MODE_SCOUT) // TODO pathfinder with dmgmap
#ifndef __OUR_PATHFINDER__
    Position p;
    TilePosition tptarget;
    /* TODO TODO TODO sharing one path for the whole UnitsGroup doesn't work
    if (_unitPos.getDistance(target) > 32 * _unitsGroup->size())
    {
        Vec posMeet(_unitsGroup->center.x(), _unitsGroup->center.y());
        if (_unitsGroup->btpath.size() > 2)
        {
            posMeet += Vec(_unitsGroup->btpath[2].x() + 15 - _unitsGroup->leadingUnit->_unitPos.x(), _unitsGroup->btpath[2].y() + 15 - _unitsGroup->leadingUnit->_unitPos.y());
            tptarget = TilePosition(posMeet.x, posMeet.y);
        }
        else if (_unitsGroup->btpath.size() > 1)
        {
            posMeet += Vec(_unitsGroup->btpath[1].x() + 15 - _unitsGroup->leadingUnit->_unitPos.x(), _unitsGroup->btpath[1].y() + 15 - _unitsGroup->leadingUnit->_unitPos.y());
            tptarget = TilePosition(posMeet.x, posMeet.y);
        }
        else 
            tptarget = target;
    }
    else */
        tptarget = target;

    if (targetDistance <= 45.26) // sqrt(BWAPI::TILE_SIZE^2 + BWAPI::TILE_SIZE^2)
    {
        _ppath.clear();
        if (targetDistance <= _maxDimension) // dirty hack to click farther than the unit size when really near
        {
            obj = Vec(target.x() - _unitPos.x(), target.y() - _unitPos.y());
            obj *= 2;
        }
        else
            obj = Vec(target.x() - _unitPos.x(), target.y() - _unitPos.y());
    }
    else 
    {
        if (WaitForSingleObject(_pathMutex, 0) == WAIT_OBJECT_0 && _newPath)
        {
            if (_btpath.size())                
            {
                _ppath.clear();
                for (std::vector<TilePosition>::const_iterator it = _btpath.begin(); it != _btpath.end(); ++it)
                    _ppath.push_back(*it);
            }
            _newPath = false;
        }
        ReleaseMutex(_pathMutex);
        if (!(Broodwar->getFrameCount() % _refreshPathFramerate))
        {
            //BayesianUnit::StaticLaunchPathfinding(this);
            DWORD threadId;
            //Broodwar->printf("creating a thread");
            // Create the thread to begin execution on its own.
            HANDLE thread = CreateThread( 
                NULL,                   // default security attributes
                0,                      // use default stack size  
                &BayesianUnit::StaticLaunchPathfinding,      // thread function name
                (void*) this,                   // argument to thread function 
                0,                      // use default creation flags 
                &threadId);             // returns the thread identifier 
            if (thread == NULL) 
            {
                // leave?
            }
            CloseHandle(thread);
        }

        // remove path points we passed
        unsigned int count = 0;
        for (unsigned int i = 0; i < _ppath.size(); ++i) 
        {
            if (_ppath[i].getDistance(_unitPos) < _maxDistWhileRefreshingPath) //45.26) // sqrt(BWAPI::TILE_SIZE^2 + BWAPI::TILE_SIZE^2)
                count = i;
        }
        for (; count > 0; --count) 
            _ppath.erase(_ppath.begin());

        if (_ppath.size() > 1)   // _ppath[0] is the current unit position
        {
            if (_ppath.size() > 3)
                p = _ppath[3];
            else if (_ppath.size() > 2) 
                p = _ppath[2];
            else
                p = _ppath[1];
            obj = Vec(p.x() - _unitPos.x() + 15, p.y() - _unitPos.y() + 15); // top left Position in a Build Tile -> middle Position = +15
        }
        else if (targetDistance < 0.9)
        {
            obj = Vec(0, 0);
        }
        else 
        {
#ifdef __DEBUG_GABRIEL
            Broodwar->printf("bug in updateObj()?");
#endif
            obj = Vec(target.x() - _unitPos.x(), target.y() - _unitPos.y());
        }
    }
#else
    /*if (_unitPos.getDistance(target) < WALK_TILES_SIZE/2)
    {
        obj = Vec(0, 0);
    }
    else if (_unitPos.getDistance(target) >= WALK_TILES_SIZE/2 && _unitPos.getDistance(target) < BWAPI::TILE_SIZE)
    {*/
    if (_unitPos.getDistance(target) < BWAPI::TILE_SIZE)
    {
        obj = Vec(target.x() - _unitPos.x(), target.y() - _unitPos.y());
    }
    else
    {
        Position p;
        if (Broodwar->getFrameCount()%10 == 0 || !_path.size()) // hack to remove with the introduction of TimeManager
        {
            // clock_t start = clock();
            /*
            Bug avec le pathfind.. Si 6 unités ou moins calculent leurs pathfind, le temps est de 9 ms / unité.
            Si ça passe à plus de 6 unités, le temps est de 65 ms / unité et ça lag ...
            */
            if (_unitsGroup->_path.size() > 10)
                quickPathFind(_btpath, TilePosition(_unitPos), TilePosition(this->_unitsGroup->_path[10].getPosition()));
            else
                quickPathFind(_btpath, TilePosition(_unitPos), TilePosition(target));
            _path.clear();
            for (std::vector<TilePosition>::const_iterator it = _btpath.begin(); it != _btpath.end(); ++it)
                _path.push_back(*it);
            //clock_t end = clock();
            //Broodwar->printf("Iterations took %f", (double)(end-start));
        } else
        {
        // remove path points we passed
        if (_path.size() > 1 && _path[1].getPosition().getDistance(_unitPos) < 35.0) // 35 pixels, TOCHANGE
            _path.erase(_path.begin());
        // I'm not drunk, do it twice! (path[2] if possible)        
        if (_path.size() > 1 && _path[1].getPosition().getDistance(_unitPos) < 35.0) // 35 pixels, TOCHANGE
            _path.erase(_path.begin());
        }

        if (_path.size() > 1)   // _ppath[0] is the current unit position
        {
            if (_path.size() > 2) 
                p = _path[2].getPosition();
            else
                p = _path[1].getPosition();
        }
        else
            p = _unitPos;
        obj = Vec(p.x() - _unitPos.x() + 15, p.y() - _unitPos.y() + 15); // top left Positoin in a Build Tile -> middle Position = +15
    }     

#endif

#ifdef __DEBUG_GABRIEL__
    //drawBTPath();
    //drawPath();
    //drawPPath();
#endif
#ifdef __EXACT_OBJ__
    double norm = obj.norm();
    obj.normalize();

    double max = -1000000.0;
    unsigned int ind_max;
    for(unsigned int i = 0; i < _dirv.size(); ++i)
    { 
        Vec tmp = _dirv[i];
        tmp.normalize();
        double val = tmp.dot(obj);
        if ((val > max && norm >= _dirv[i].norm())) // most in the direction of obj AND not past obj
        {
            max = val;
            ind_max = i;
        }
    }
    obj = _dirv[ind_max];
#endif
}

void BayesianUnit::drawObj(int number)
{
    Position up = _unitPos;
    Broodwar->drawLine(CoordinateType::Map, up.x(), up.y(), up.x() + (int)obj.x, up.y() + (int)obj.y, Colors::Green);
    if (number == 0) return;
    if (number == 1)
    {
        const int middle = 64;
        for (unsigned int i = 0; i < this->_dirv.size(); ++i)
        {
            //Broodwar->printf("obj.x: %f, obj.y: %f, _dirv[i].x: %f, _dirv[i].y: %f\n", obj.x, obj.y, _dirv[i].x, _dirv[i].y);
            if (obj == _dirv[i])
                Broodwar->drawBoxScreen(middle - 4 + (int)_dirv[i].x, middle - 4 + (int)_dirv[i].y,
                middle + 4 + (int)_dirv[i].x, middle + 4 + (int)_dirv[i].y, Colors::Green, true);
            else
                Broodwar->drawBoxScreen(middle - 4 + (int)(int)_dirv[i].x, middle - 4 + (int)(int)_dirv[i].y,
                middle + 4 + (int)(int)_dirv[i].x, middle + 4 + (int)(int)_dirv[i].y, Colors::Orange);
        }
    } else
    {
        for (unsigned int i = 0; i < this->_dirv.size(); ++i)
        {
            //Broodwar->printf("obj.x: %f, obj.y: %f, _dirv[i].x: %f, _dirv[i].y: %f\n", obj.x, obj.y, _dirv[i].x, _dirv[i].y);
            if (obj == _dirv[i])
                Broodwar->drawBoxMap(up.x() - 4 + (int)_dirv[i].x, up.y() - 4 + (int)_dirv[i].y,
                up.x() + 4 + (int)_dirv[i].x, up.y() + 4 + (int)_dirv[i].y, Colors::Green, true);
            else
                Broodwar->drawBoxMap(up.x() - 4 + (int)(int)_dirv[i].x, up.y() - 4 + (int)(int)_dirv[i].y,
                up.x() + 4 + (int)(int)_dirv[i].x, up.y() + 4 + (int)(int)_dirv[i].y, Colors::Orange);
        }
    }
}

void BayesianUnit::drawOccupation(int number)
{
    Position up = _unitPos;
    if (number == 0) return;
    if (number == 1)
    {
        const int middleX = 64;
        const int middleY = 150;
        for (unsigned int i = 0; i < this->_dirv.size(); ++i)
        {
            //Broodwar->printf("obj.x: %f, obj.y: %f, _dirv[i].x: %f, _dirv[i].y: %f\n", obj.x, obj.y, _dirv[i].x, _dirv[i].y);
            if (_occupation[i] == OCCUP_BLOCKING)
                Broodwar->drawBoxScreen(middleX - 4 + (int)_dirv[i].x, middleY - 4 + (int)_dirv[i].y,
                middleX + 4 + (int)_dirv[i].x, middleY + 4 + (int)_dirv[i].y, Colors::Red, true);
            else if (_occupation[i] == OCCUP_BUILDING)
                Broodwar->drawBoxScreen(middleX - 4 + (int)(int)_dirv[i].x, middleY - 4 + (int)(int)_dirv[i].y,
                middleX + 4 + (int)(int)_dirv[i].x, middleY + 4 + (int)(int)_dirv[i].y, Colors::Orange, true);
        }
    } else
    {
        for (unsigned int i = 0; i < this->_dirv.size(); ++i)
        {
            //Broodwar->printf("obj.x: %f, obj.y: %f, _dirv[i].x: %f, _dirv[i].y: %f\n", obj.x, obj.y, _dirv[i].x, _dirv[i].y);
            if (_occupation[i] == OCCUP_BLOCKING)
                Broodwar->drawBoxMap(up.x() - 4 + (int)_dirv[i].x, up.y() - 4 + (int)_dirv[i].y,
                up.x() + 4 + (int)_dirv[i].x, up.y() + 4 + (int)_dirv[i].y, Colors::Red, true);
            else if (_occupation[i] == OCCUP_BUILDING)
                Broodwar->drawBoxMap(up.x() - 4 + (int)(int)_dirv[i].x, up.y() - 4 + (int)(int)_dirv[i].y,
                up.x() + 4 + (int)(int)_dirv[i].x, up.y() + 4 + (int)(int)_dirv[i].y, Colors::Orange, true);
        }
    }
}

void BayesianUnit::updateDirV()
{
    _dirv.clear();

    Position p = _unitPos;
    WalkTilePosition wtp(p);
 
    int pixs = min(_slarge, _sheight) + 1; // TOCHANGE
    int pixs_far = 0; // outer layer of dirvectors
    if (pixs < 32) // 3*pixs < 96
    {
        pixs_far = 32;
    }
    else
        pixs_far = pixs;
    for (int x = -3; x <= 3; ++x)
        for (int y = -3; y <= 3; ++y)
        {
            int xx, yy;
            if (x == -3 || x == 3 || y == -3 || y == 3)
            {
                xx = x*pixs_far;
                yy = y*pixs_far;
            }
            else
            {
                xx = x*pixs;
                yy = y*pixs;
            }
            Vec v(xx, yy);

            Position tmp = v.translate(p);
            if (tmp.x() <= 32*Broodwar->mapWidth() && tmp.y() <= 32*Broodwar->mapHeight() 
                    && tmp.x() >= 0 && tmp.y() >= 0)
            {
                _dirv.push_back(v);
            }
        }  
}

void BayesianUnit::testIfBlocked()
{
    if (!(Broodwar->getFrameCount() % 13))
        _posAtMost13FramesAgo = _unitPos;
    if (!(Broodwar->getFrameCount() % 23))
        _posAtMost23FramesAgo = _unitPos;
    if (!(Broodwar->getFrameCount() % 11))
    {
        if (!(_iThinkImBlocked && Broodwar->getFrameCount() - _lastClickFrame < 12))
            _iThinkImBlocked = (_posAtMost13FramesAgo == _unitPos && _posAtMost23FramesAgo == _unitPos) ? true : false;    
    }
    if (!target.isValid())
        target.makeValid();
}

void BayesianUnit::resumeFromBlocked()
{
    if ((Broodwar->getFrameCount() - _lastClickFrame) > 24)
    {
#ifdef __DEBUG_GABRIEL__
        //Broodwar->printf("resuming from blocked");
#endif
        if (_lastRightClick != target)
        {
            unit->rightClick(target);
            _lastRightClick = target;
        }
        else if (_lastRightClick == target)
        {
            Vec tmpv = Vec(target.x() - _unitPos.x(), target.y() - _unitPos.y()) * 2;
            Position tmpp = tmpv.translate(_unitPos);
            if (!tmpp.isValid())
                tmpp.makeValid();
            unit->rightClick(tmpp);
            _lastRightClick = tmpp;
        }
        _lastClickFrame = Broodwar->getFrameCount();
    }
}

void BayesianUnit::updateRangeEnemies()
{
    _rangeEnemies.clear();
    for (std::set<Unit*>::const_iterator it = _unitsGroup->enemies.begin(); 
        it != _unitsGroup->enemies.end(); ++it)
    {
        _rangeEnemies.insert(std::make_pair<double, Unit*>(_unitPos.getDistance((*it)->getPosition()), *it));
    }
}

void BayesianUnit::updateTargetEnemy()
{
    // clear old damage
    if (_unitsGroup->unitDamages.left.count(targetEnemy))
    {
        UnitDmgBimap::left_iterator it = _unitsGroup->unitDamages.left.find(targetEnemy);
        int tmp_dmg = it->second.dmg - computeDmg(targetEnemy);
        Unit* tmp_unit = it->first;
        _unitsGroup->unitDamages.left.erase(it);
        _unitsGroup->unitDamages.insert(UnitDmg(tmp_unit, Dmg(tmp_dmg, tmp_unit)));
    }
    
    // choose new targetEnemy
    /// Focus fire
    UnitDmgBimap::right_iterator it;
    for (it = _unitsGroup->unitDamages.right.begin();
        it != _unitsGroup->unitDamages.right.end(); ++it)
    {
        if (it->first.dmg == 0) // iterate on the interesting part only
            break;
        if (it->first.dmg < it->second->getHitPoints() + it->second->getShields()
            && (
            (!it->second->getType().isFlyer() && _unitPos.getDistance(it->second->getPosition())
            < (double)unit->getType().groundWeapon().maxRange() + addRangeGround()) 
            || (it->second->getType().isFlyer() && _unitPos.getDistance(it->second->getPosition())
            < (double)unit->getType().groundWeapon().maxRange() + addRangeAir())
            ))
        {
            setTargetEnemy(it->second);
            return;
        }
    }
    /// Keep old target if in range
    if (targetEnemy 
        && (
        (!it->second->getType().isFlyer() && _unitPos.getDistance(targetEnemy->getPosition())
        < (double)unit->getType().groundWeapon().maxRange() + addRangeGround())
        || (it->second->getType().isFlyer() && _unitPos.getDistance(it->second->getPosition())
        < (double)unit->getType().groundWeapon().maxRange() + addRangeAir())
        ))
    {
        setTargetEnemy(targetEnemy);
        return;
    }
    /// Take a new one in range
    if (_rangeEnemies.size())
        setTargetEnemy(_rangeEnemies.begin()->second);
    /// No target in range, follow the old one
    else if (targetEnemy)
        setTargetEnemy(targetEnemy);
    /// Take the default target of the UnitGroup
    else if (_unitsGroup->defaultTargetEnemy != NULL)
        setTargetEnemy(_unitsGroup->defaultTargetEnemy);
    /// WTF??? 
    else
        Broodwar->printf("WTF? Mode fight, no enemy");
}

void BayesianUnit::setTargetEnemy(Unit* u)
{
    targetEnemy = u;
    if (_unitsGroup->unitDamages.left.count(u))
    { /// <=> _unitsGroup->unitDamages.left[u] += computeDmg(u);
        UnitDmgBimap::left_iterator it = _unitsGroup->unitDamages.left.find(u);
        int tmp_dmg = it->second.dmg + computeDmg(u);
        _unitsGroup->unitDamages.left.erase(it);
        _unitsGroup->unitDamages.insert(UnitDmg(u, Dmg(tmp_dmg, u)));
    }
    else
        _unitsGroup->unitDamages.insert(UnitDmg(u, Dmg(computeDmg(u), u)));
}

int BayesianUnit::computeDmg(Unit* u)
{
    // TODO complete: armors, upgrades, shields, spells (matrix...)

    if (u->getType().isFlyer())
    {
        if (u->getShields() 
            >= (unit->getType().airWeapon().damageAmount() /* - shield armor */) * unit->getType().maxAirHits())
        {
            return (unit->getType().airWeapon().damageAmount() /* - shield armor */)
                * unit->getType().maxAirHits();
        }
        else
        {
            double factor = 1.0;
            if (unit->getType().airWeapon().damageType() == BWAPI::DamageTypes::Explosive)
            {
                if (u->getType().size() == BWAPI::UnitSizeTypes::Small)
                    factor = 0.25;
                else if (u->getType().size() == BWAPI::UnitSizeTypes::Medium)
                    factor = 0.5;
            } else if (unit->getType().airWeapon().damageType() == BWAPI::DamageTypes::Concussive)
            {
                if (u->getType().size() == BWAPI::UnitSizeTypes::Medium)
                    factor = 0.75;
                else if (u->getType().size() == BWAPI::UnitSizeTypes::Large)
                    factor = 0.5;
            }
            return (unit->getType().airWeapon().damageAmount() * factor - u->getType().armor())
                * unit->getType().maxAirHits();
        }
    }
    else
    {        
        if (u->getShields() 
            >= (unit->getType().groundWeapon().damageAmount() /* - shield armor */) * unit->getType().maxGroundHits())
        {
            return (unit->getType().groundWeapon().damageAmount() /* - shield armor */)
                * unit->getType().maxGroundHits();
        }
        else
        {
            double factor = 1.0;
            if (unit->getType().groundWeapon().damageType() == BWAPI::DamageTypes::Explosive)
            {
                if (u->getType().size() == BWAPI::UnitSizeTypes::Small)
                    factor = 0.25;
                else if (u->getType().size() == BWAPI::UnitSizeTypes::Medium)
                    factor = 0.5;
            } else if (unit->getType().groundWeapon().damageType() == BWAPI::DamageTypes::Concussive)
            {
                if (u->getType().size() == BWAPI::UnitSizeTypes::Medium)
                    factor = 0.75;
                else if (u->getType().size() == BWAPI::UnitSizeTypes::Large)
                    factor = 0.5;
            }
            return (unit->getType().groundWeapon().damageAmount() * factor - u->getType().armor())
                * unit->getType().maxGroundHits();
        }
    }
}

void BayesianUnit::drawDirV()
{
    Position up = _unitPos;
    for (unsigned int i = 0; i < _dirv.size(); ++i)
    {
        //Broodwar->printf("_dirv[i].x: %f, _dirv[i].y: %f \n", _dirv[i].x, _dirv[i].y);
        Broodwar->drawLine(CoordinateType::Map, up.x(), up.y(), _dirv[i].translate(up).x(), _dirv[i].translate(up).y(), Colors::Grey);
    }
    for (unsigned int i = 0; i < _dirv.size(); ++i)
        if (_dirv[i].norm() < 15.5)
            Broodwar->drawLine(CoordinateType::Map, up.x(), up.y(), _dirv[i].translate(up).x(), _dirv[i].translate(up).y(), Colors::Black);
}

void BayesianUnit::attackMove(const Position& p)
{
    target = p;
	unit->attackMove(p);
}

void BayesianUnit::computeProbs()
{
    _dirvProb.clear();
    for (unsigned int i = 0; i < _dirv.size(); ++i)
        _dirvProb.insert(make_pair(computeProb(i), _dirv[i]));
}

void BayesianUnit::selectDir(const Vec& criterium)
{
    multimap<double, Vec>::const_iterator last = _dirvProb.end(); 
    // I want the right probabilities and not 1-prob in the multimap
    --last;
    if (_dirvProb.count(_dirvProb.begin()->first) == 1) 
    {
        dir = last->second;
    }
    // or the equally most probable and most in the direction of damage gradient
    else
    {
        pair<multimap<double, Vec>::const_iterator, multimap<double, Vec>::const_iterator> possible_dirs = _dirvProb.equal_range(last->first);
        if (_mode == MODE_INPOS && obj == Vec(0, 0))
        {
            // sum on all the possible_directions
            for (multimap<double, Vec>::const_iterator it = possible_dirs.first; it != possible_dirs.second; ++it)
            {
                obj += it->second;
            }
        }
        double max_dir = -100000.0;
        double max_norm = -100000.0;
        double objnorm = obj.norm();
        for (multimap<double, Vec>::const_iterator it = possible_dirs.first; it != possible_dirs.second; ++it)
        {
            Vec tmpVec = it->second;
            double direction = criterium.dot(tmpVec.normalize());
            double norm = tmpVec.norm();
            if ((direction > max_dir || (direction == max_dir && norm > max_norm)) && norm <= objnorm)
            {
                // test if directly (right line) attainable
                Vec tmpHalfDir = it->second;
                tmpHalfDir /= 2;
                Position tmpPosDir = tmpHalfDir.translate(_unitPos);
                if (Broodwar->isWalkable(tmpPosDir.x() / 8, tmpPosDir.y() / 8))
                {
                    max_dir = direction;
                    max_norm = norm;
                    dir = it->second;
                }
            }
        }
    }
}

int BayesianUnit::addRangeGround()
{
    return 0;
}

int BayesianUnit::addRangeAir()
{
    return 0;
}

void BayesianUnit::updateDir()
{
    // update possible directions vector
    updateDirV();
    //Affiche les différents axes de directions de l'unité
    //drawDirV();

    // update attractions
    updateAttractors();
    //Affiche les différents objets et la probabilité de pouvoir y aller
    //drawAttractors();
    
    // update objectives
    updateObj();
#ifdef __DEBUG_GABRIEL__
    //drawObj(_unitsGroup->size());
    //drawOccupation(_unitsGroup->size());
#endif
    
    // compute the probability to go in each dirv(ector)
    computeProbs();
#ifdef __DEBUG_GABRIEL__
    drawProbs(_dirvProb, _unitsGroup->size());
#endif

    // select the most probable, most in the direction of obj if equally probables
    selectDir(obj);
}

void BayesianUnit::drawDir()
{
    Position up = _unitPos;
    if (dir.translate(up).y() <= Broodwar->mapHeight()*BWAPI::TILE_SIZE && dir.translate(up).x() <= Broodwar->mapWidth()*BWAPI::TILE_SIZE)
        Broodwar->drawLineMap(up.x(), up.y(), dir.translate(up).x(), dir.translate(up).y(), Colors::Red);
}

void BayesianUnit::clickDir()
{
    double dist = _unitPos.getDistance(target);
    if (_mode != MODE_INPOS && (dir == Vec(0,0) || dist < 0.9)) // 0.9 arbitrary
    {
        return;
    } 
    else if (_mode == MODE_INPOS && dir != Vec(0, 0))
    {
        dir += _unitPos;
        unit->rightClick(dir.toPosition());
        _lastRightClick = dir.toPosition();
        ///_lastClickFrame = Broodwar->getFrameCount();
        return;
    }
    else if (_mode == MODE_INPOS)
        return;
    //if (dist > 11.32) // sqrt(8^2 + 8^2), one walk tile
    if (dist > _maxDimension && !_iThinkImBlocked) //45.26) // sqrt(32^2 + 32^2), one build tile
    {
        dir += _unitPos;
        unit->rightClick(dir.toPosition());
        _lastRightClick = dir.toPosition();
        ///_lastClickFrame = Broodwar->getFrameCount();
    }
    else if (_lastRightClick!=target)
    {
        unit->rightClick(target);
        _lastRightClick = target;
        ///_lastClickFrame = Broodwar->getFrameCount();
    } 
    else
    {
        // DO NOTHING
    }
}

void BayesianUnit::flee()
{
    _fleeing = true;
    Vec dmgGrad = this->mapManager->groundDamagesGrad[_unitPos.x()/32 + _unitPos.y()/32*Broodwar->mapWidth()];
    if (dmgGrad.norm() > 0.9)
    {
        updateDirV();
        updateAttractors();
        // do not update objectives
        computeProbs();
        selectDir(dmgGrad);
        clickDir();
    }
    if (!this->mapManager->groundDamages[_unitPos.x()/32 + _unitPos.y()/32*Broodwar->mapWidth()])
        _fleeing = false;
}

void BayesianUnit::fightMove()
{
    updateDirV();
    updateAttractors();

    /*int maxHeight = Broodwar->getGroundHeight(TilePosition(_unitPos));
    double max = -10000000.0;
    double max_norm = -10000000.0;
    unsigned int ind_max;
    for(unsigned int i = 0; i < _dirv.size(); ++i)
    { 
        Vec tmp = _dirv[i];
        TilePosition tmpTilePos = TilePosition(_dirv[i].translate(_unitPos));
        int tmpHeight = Broodwar->getGroundHeight(tmpTilePos);
        tmp.normalize();
        double val = tmp.dot(current);
        if (tmpHeight > maxHeight) // most in the direction of height
        {
            max = val;
            max_norm = _dirv[i].norm();
            ind_max = i;
        }
    }
    obj = _dirv[ind_max];*/
    Vec maxHeightDir = Vec(0, 0);

    computeProbs();
    selectDir(maxHeightDir);
    clickDir();
}

void BayesianUnit::drawArrow(Vec& v)
{
    int xfrom = _unitPos.x();
    int yfrom = _unitPos.y();
    double xto = xfrom + 20*v.x; // 20, magic number
    double yto = yfrom + 20*v.y;
    double v_x = xto-xfrom; 
    double v_y = yto-yfrom;
    Broodwar->drawLine(CoordinateType::Map, xfrom, yfrom, (int)xto, (int)yto, Colors::Orange);
    Broodwar->drawTriangle(CoordinateType::Map, (int)(xto - 0.1*v_y), (int)(yto + 0.1*v_x), (int)(xto + 0.1*v_y), (int)(yto - 0.1*v_x), (int)(xto + 0.1*v_x), (int)(yto + 0.1*v_y), Colors::Orange); // 0.1, magic number
}

void BayesianUnit::onUnitDestroy(Unit* u)
{
}

void BayesianUnit::onUnitShow(Unit* u)
{
}

void BayesianUnit::onUnitHide(Unit* u)
{
}

void BayesianUnit::update()
{
    if (!unit->exists()) return;
    check();
    _unitPos = unit->getPosition();
    //if (unit->isAttacking())
    //    Broodwar->printf("frame %d, damage cooldown %d", Broodwar->getFrameCount(), unit->getType().groundWeapon().damageCooldown());

    if (_mode == MODE_FLOCK && this->_unitsGroup->size() > 21)
    {
        switchMode(MODE_MOVE);
    }

    if (_mode != MODE_FIGHT_G && _mode != MODE_SCOUT 
        && !_unitsGroup->enemies.empty()
        && unit->getGroundWeaponCooldown() <= Broodwar->getLatency())
    {
#ifdef __DEBUG_GABRIEL__
        //Broodwar->setLocalSpeed(51);
#endif
        this->switchMode(MODE_FIGHT_G);
    }

    if (_mode == MODE_SCOUT || _mode == MODE_FLOCK)
    {
        testIfBlocked();
        if (_iThinkImBlocked)
        {
            resumeFromBlocked();
            return;
        }
    }

//if (_mode == MODE_FLOCK)
//Broodwar->printf("MODE FLOCK");
    //if (_mode == MODE_SCOUT)
    //    Broodwar->printf("MODE SCOUT");

    switch (_mode)
    {
    case MODE_FLOCK:
        if (_unitPos.getDistance(target) < 3.9)
        {
            this->switchMode(MODE_INPOS);
            return;
        }
        updateDir();
#ifdef __DEBUG_GABRIEL__
        //drawObj(0); // green
        //drawDir(); // red
#endif
        clickDir();
        break;

    case MODE_SCOUT:
        updateDir();
        clickDir();
        break;

    case MODE_INPOS:       
        if (_unitPos.getDistance(target) > 96) // OCHANGE should be sqrt(96^2+96^2)
        {
            this->switchMode(MODE_FLOCK);
            return;
        }
        updateDir();
        clickDir();
        break;

    case MODE_FIGHT_G:
        micro();
        break;

    case MODE_MOVE:
        if ((Broodwar->getFrameCount() - _lastClickFrame) > _attackDuration)
        {
            unit->rightClick(target);
            _lastRightClick = target;
            _lastClickFrame = Broodwar->getFrameCount();
        }
        break;
        
    default:
        break;
    }

#ifdef __DEBUG_GABRIEL__
    drawTarget();
#endif
    _lastTotalHP = unit->getShields() + unit->getHitPoints();
    return;
}

void BayesianUnit::attackEnemy(BWAPI::Unit* u, BWAPI::Color col)
{
#ifdef __DEBUG_NICOLAS__
    int ux = _unitPos.x(); int uy = _unitPos.y();
    int ex = u->getPosition().x(); int ey = u->getPosition().y();

    Broodwar->drawLineMap(ux, uy, ex, ey, col);
#endif
    if (unit->getOrderTarget() != u)
    {
        unit->rightClick(u);
    }
}
