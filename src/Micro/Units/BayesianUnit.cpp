
#include "BayesianUnit.h"
#include "Rainbow.h"
#include <utility>
#include <Defines.h>
#include <time.h>
#include <UnitsGroup.h>
#include <set>
#ifdef PROBT
#include <pl.h>
#else
#include <boost/math/distributions/normal.hpp>
using boost::math::normal;
#endif

//#define _OUR_PATHFINDER_

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

#define _PROB_NO_FLOCK_MOVE 0.19 // TODO change for const and set specific values depending on the unit type
#define _PROB_NO_EUNIT_MOVE 0.9
#define _PROB_NO_UNIT_MOVE 0.8
#define _PROB_NO_WALL_MOVE 0.9999999
#define _PROB_NO_BUILDING_MOVE 0.99999
#define _PROB_GO_OBJ 0.9

#define _PROB_CENTER_ATTRACT 0.4
#define _PROB_CENTER_V1_ATTRACT 0.3
#define _PROB_CENTER_V2_ATTRACT 0.2
#define _PROB_CENTER_V3_ATTRACT 0.09

// TODO optimize by removing the dirv inside the unit (but one, the center, when it's better not to move at all)

BayesianUnit::BayesianUnit(Unit* u, UnitsGroup* ug)
: BattleUnit(u)
, dir(Vec(unit->getVelocityX(), unit->getVelocityY()))
, _mode(MODE_FLOCK)
, _unitsGroup(ug)
, _ground_unit(!unit->getType().isFlyer())
{
    updateDirV();
    mapManager = & MapManager::Instance();
    switchMode(_mode);
    updateAttractors();
    initDefaultProb();
    if (_mode == MODE_FLOCK)
    {
        _flockProb.push_back(_PROB_NO_FLOCK_MOVE);  //FLOCK_NO
        _flockProb.push_back(0.01);                  //FLOCK_CONTACT
        _flockProb.push_back(0.20);                 //FLOCK_CLOSE
        _flockProb.push_back(0.30);                  //FLOCK_MEDIUM
        _flockProb.push_back(0.25);                 //FLOCK_FAR


        _inPosProb.push_back(0.49);                // INPOS_OK
        _inPosProb.push_back(0.01);                // INPOS_CONTACT
        _inPosProb.push_back(0.1);                // INPOS_CLOSE
        _inPosProb.push_back(0.15);                // INPOS_MEDIUM
        _inPosProb.push_back(0.3);                // INPOS_FAR
        // old (~~safe) params
//        _flockProb.push_back(0.05);                  //FLOCK_CONTACT
//        _flockProb.push_back(0.25);                 //FLOCK_CLOSE
//        _flockProb.push_back(0.38);                  //FLOCK_MEDIUM
//        _flockProb.push_back(0.22);                 //FLOCK_FAR
    }
    else if (_mode == MODE_FIGHT_G)
    {
        _fightGProb.push_back(0.9); // FIGHTG_NO
        _fightGProb.push_back(0.7); // FIGHTG_LIGHT
        _fightGProb.push_back(0.5); // FIGHTG_MEDIUM
        _fightGProb.push_back(0.3); // FIGHTG_HEAVY
        _fightGProb.push_back(0.1); // FIGHTG_DEAD
    }
}

BayesianUnit::~BayesianUnit()
{
}

void BayesianUnit::initDefaultProb()
{
    _defaultProb.insert(make_pair(OCCUP_EUNIT, _PROB_NO_EUNIT_MOVE));         // P(enemy_unit_in_this_case=false | we_go_in_this_case=true)
    _defaultProb.insert(make_pair(OCCUP_UNIT, _PROB_NO_UNIT_MOVE));           // P(allied_unit_in_this_case=false | we_go_in_this_case=true)
    _defaultProb.insert(make_pair(OCCUP_BLOCKING, _PROB_NO_WALL_MOVE));       // P(this_case_is_blocking=false | we_go_in_this_case=true)
    _defaultProb.insert(make_pair(OCCUP_BUILDING, _PROB_NO_BUILDING_MOVE));   // P(there_is_a_building_in_this_case=false | we_go_in_this_case=true)
    //_defaultProb.insert(make_pair(OCCUP_FLOCK, _PROB_NO_FLOCK_MOVE));       // P(there_is_flocking_attraction=false | we_go_in_this_case=true)
}


void BayesianUnit::computeFightGValues()
{
    _fightGValues.clear();
    // TODO
}

void BayesianUnit::computeInPosValues()
{
    _inPosValues.clear();
    for (unsigned int i = 0; i < _dirv.size(); ++i)
    {
        vector<inPos_value> tmpv;
        for (std::set<Unit*>::const_iterator it = 
                Broodwar->self()->getUnits().begin(); 
            it != Broodwar->self()->getUnits().end(); ++it)
        {
            if ( (*it) == this->unit) continue; 

            bool appartient = false; //*** [A CHANGER]
            for (vector<pBayesianUnit>::const_iterator it2 = 
                _unitsGroup->getUnits()->begin(); 
            it2 != _unitsGroup->getUnits()->end() && !appartient; ++it2)
                    appartient = (*it2)->unit == (*it);
            if (appartient) continue; //*** [FIN A CHANGER]

            Position tmp = _dirv[i].translate(this->_unitPos);
            Vec tmpvit((*it)->getVelocityX(), 
                    (*it)->getVelocityY());

            inPos_value value = (inPos_value)(1 + (int)tmp.getDistance(
                    tmpvit.translate((*it)->getTargetPosition())) / 32);

            if (value <= INPOS_FAR)
                tmpv.push_back(value);
            else
            {
                 /*value = (inPos_value)(1 + (int)tmp.getDistance(
                                tmpvit.translate((*it)->getPosition())) / 32);
                if (value <= INPOS_FAR)
                    tmpv.push_back(value);
                else*/
                    tmpv.push_back(INPOS_OK);
            }
        }
        _inPosValues.push_back(tmpv);
    }
}

void BayesianUnit::computeFlockValues()
{
    _flockValues.clear();
    for (unsigned int i = 0; i < _dirv.size(); ++i)
    {
        vector<flock_value> tmpv;
        for (vector<pBayesianUnit>::const_iterator it = 
                _unitsGroup->getUnits()->begin(); 
            it != _unitsGroup->getUnits()->end(); ++it)
        {
            if ((*it)->unit == this->unit) continue; 
            // we don't flock with ourselves!
            
            //CODE flock_value value = (flock_value)
            //CODE (1 + (int)((*it)->unit->getDistance(
            //CODE      _dirv[i].translate(this->_unitPos()))/32));
            Position tmp = _dirv[i].translate(this->_unitPos);
            Vec tmpvit((*it)->unit->getVelocityX(), 
                    (*it)->unit->getVelocityY()); 
            // we flock with the interpolated next position of other units
            //tmpvit *= 8;
            //TEST BWAPI::Broodwar->printf("X: %f, Y: %f \n", (*it)->unit->getVelocityX(), (*it)->unit->getVelocityY());
            flock_value value = (flock_value)(1 + (int)tmp.getDistance(
                        tmpvit.translate((*it)->_unitPos)) / 32);
            //CODE if (value == FLOCK_FAR + 1) --value; 
            // some kind of hysteresis for FAR
            if (value <= FLOCK_FAR)
                tmpv.push_back(value);
            else
                tmpv.push_back(FLOCK_NO);
            //TEST Broodwar->printf("distance int: %d, double %f\n", (int)(*it)->unit->getDistance(this->unit), (*it)->unit->getDistance(this->unit));
            //TEST Broodwar->printf("Flock value %d\n", value);
        }
        _flockValues.push_back(tmpv);
    }
    // TODO: <OR> Heat dissipation model
}

void BayesianUnit::switchMode(unit_mode um)
{
    // Comportement attendu ?
    _mode = um;
    switch (um) 
    {
        case MODE_FLOCK:
            _defaultProb[OCCUP_UNIT] = 0.6;
            //_defaultProb[OCCUP_FLOCK] = 0.1;
            break;
        case MODE_FIGHT_G:
            break;
        default:
            break;
    }
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
    // flocking attractions
    if (_mode == MODE_FLOCK)
    {
        computeFlockValues();
    }
    // inposition attractions
    else if (_mode == MODE_INPOS)
    {
        computeInPosValues();
    }
    // fight attractions
    else if (_mode == MODE_FIGHT_G)
    {
        computeFightGValues();
    }

    // building and blocking attraction (repulsion)
    const int width = Broodwar->mapWidth();
    Position up = _unitPos;
    for (unsigned int i = 0; i < _dirv.size(); ++i)
    {
        Position tmp = _dirv[i].translate(up);
        
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


        //if (mapManager->buildings[tmp.x()/32 + (tmp.y()/32)*width])
        //    _occupation.push_back(OCCUP_BUILDING);




/*
        if (mapManager->buildings_wt[tmp.x()/8 + (tmp.y()/8)*4*width])
            _occupation.push_back(OCCUP_BUILDING);
        else if (!mapManager->walkability[tmp.x()/8 + (tmp.y()/8)*4*width])
                                       // tmp.x()/8 + (tmp.y()/2)*width
            _occupation.push_back(OCCUP_BLOCKING);
            */

        else if (0/*TODO TEST SI Y A UNE UNITE QUI BLOQUE en TMP*/)
            _occupation.push_back(OCCUP_UNIT);


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
        UnitType ut = this->unit->getType();
        string str = ut.getName();
        Position p = _dirv[i].translate(this->_unitPos);
        for (unsigned int j = 0; j < _flockValues[i].size(); ++j)
            Broodwar->drawBox(CoordinateType::Map, p.x() - 3 + j , p.y() - 3 + j, p.x() + 1 + j, p.y() + 1 + j, colors.rainbow[(_flockValues[i][j]+4) % 5], true);
    }
}

double BayesianUnit::computeProb(unsigned int i)
{
#ifdef PROBT
    // plSymbol A("A", PL_BINARY_TYPE);
#else
    double val = 1.;
    //for (multimap<Position, attractor_type>::const_iterator it = _prox.begin(); it != _prox.end(); ++it)

    if (_unitsGroup->getUnits()->size() > 1 && _mode == MODE_FLOCK)
    {
        /// FLOCKING INFLUENCE
        // one j for each attractor
        for (unsigned int j = 0; j < _flockValues[i].size(); ++j)
            val *= _flockProb[_flockValues[i][j]];
       
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
            // TODO 0.01 magic number (uniform prob to go in the half-quadrant opposite to obj)
        }
    }
    else if (_mode == MODE_INPOS)
    {
        /// INPOS INFLUENCE
        // one j for each attractor
        for (unsigned int j = 0; j < _inPosValues[i].size(); ++j)
            val *= _inPosProb[_inPosValues[i][j]];

        if (_dirv[i] == obj)
            val *= _PROB_CENTER_ATTRACT;
        else
        {
            std::set<Vec> center_v1;
            for(int ii = -8; ii <= 8 ; ii+=8)
                for(int jj = -8; jj <= 8 ; jj+=8)
                {
                    if (ii == jj == 0) continue;
                    Vec tmp(0,0);
                    tmp += obj; tmp += Vec(ii,jj);
                    center_v1.insert(tmp);
                }
            std::set<Vec> center_v2;
            std::set<Vec> center_v3;
            for(int ii = -16; ii <= 16 ; ii+=8)
                for(int jj = -8; jj <= 8 ; jj+=8)
                {
                    if (ii == jj == 0) continue;
                    Vec tmp(0,0);
                    tmp += obj; tmp += Vec(ii,jj);
                    center_v2.insert(tmp);
                }
            for(int ii = -32; ii <= 32 ; ii+=8)
                for(int jj = -8; jj <= 8 ; jj+=8)
                {
                    if (ii == jj == 0) continue;
                    Vec tmp(0,0);
                    tmp += obj; tmp += Vec(ii,jj);
                    center_v3.insert(tmp);
                }

            if (center_v1.count(_dirv[i]))
                val *= _PROB_CENTER_V1_ATTRACT;
            else if(center_v2.count(_dirv[i]))
                val *= _PROB_CENTER_V2_ATTRACT;
            else if(center_v3.count(_dirv[i]))
                val *= _PROB_CENTER_V3_ATTRACT;
            else 
                val *= (1.0 - _PROB_CENTER_V1_ATTRACT -  _PROB_CENTER_ATTRACT -  _PROB_CENTER_V3_ATTRACT - _PROB_CENTER_V2_ATTRACT );
        }
    }
    else if (_mode == MODE_FIGHT_G)
    {
        // TODO
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
    //Broodwar->printf("val is %d \n", val);
#endif
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

void BayesianUnit::updateObj()
{
#ifndef _OUR_PATHFINDER_
    Position p;
    if (Broodwar->getFrameCount()%10 == 0 || !_path.size()) // hack to remove with the introduction of TimeManager
    {
        //TIMINGclock_t start = clock();
        _btpath = BWTA::getShortestPath(TilePosition(_unitPos), TilePosition(target));
        _path.clear();
        for (std::vector<TilePosition>::const_iterator it = _btpath.begin(); it != _btpath.end(); ++it)
            _path.push_back(*it);
        //TIMINGclock_t end = clock();
        //TIMINGBroodwar->printf("Iterations took %f", (double)(end-start));
    } else
    {
        // remove path points we passed
        if (_path.size() > 1 && _path[1].getPosition().getDistance(_unitPos) < 35.0) // 35 pixels, TODO to change perhaps
            _path.erase(_path.begin());
        // I'm not drunk, do it twice! (path[2] if possible)        
        if (_path.size() > 1 && _path[1].getPosition().getDistance(_unitPos) < 35.0) // 35 pixels, TODO to change perhaps
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
    obj = Vec(p.x() - _unitPos.x() + 12, p.y() - _unitPos.y() + 12); // to center the Tile, change 12 for 16 - top/left sizes
    drawPath();
#else
    if (_unitPos.getDistance(target) < WALK_TILES_SIZE*2)

    {
        obj = Vec(0, 0);
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
            if (_unitsGroup->_path.size() > 8)
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
        if (_path.size() > 1 && _path[1].getPosition().getDistance(_unitPos) < 35.0) // 35 pixels, TODO to change perhaps
            _path.erase(_path.begin());
        // I'm not drunk, do it twice! (path[2] if possible)        
        if (_path.size() > 1 && _path[1].getPosition().getDistance(_unitPos) < 35.0) // 35 pixels, TODO to change perhaps
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
        obj = Vec(p.x() - _unitPos.x() + 12, p.y() - _unitPos.y() + 12); // to center the Tile, change 12 for 16 - top/left sizes
    }        


#ifdef __DEBUG_GABRIEL__
    drawBTPath();
    MapManager* mapm = & MapManager::Instance();
    mapm->drawBuildingsStrict();
    mapm->drawWalkability();
    //drawPath();
#endif
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
    int accel = unit->getType().acceleration();
    double speed = unit->getType().topSpeed();
    //Broodwar->printf("unittype: %s, accel: %d, top speed: %f", unit->getType(). , accel, speed);
    Position p = _unitPos;
    WalkTilePosition wtp(p);
//    const int minx = 0;
//    const int maxx = 32*Broodwar->mapWidth();
//    const int miny = 0;
//    const int maxy = 32*Broodwar->mapHeight();
    const int minx = max(p.x() - 2*_slarge, 0);
    const int maxx = min(p.x() + 2*_slarge, 32*Broodwar->mapWidth());
    const int miny = max(p.y() - 2*_sheight, 0);
    const int maxy = min(p.y() + 2*_sheight, 32*Broodwar->mapHeight());
    for (int x = -4; x <= 4; ++x) // -4..4 overkill
        for (int y = -4; y <= 4; ++y)
        {
            Vec v(x*_slarge/2, y*_sheight/2);
            Position tmp = v.translate(p);
            if (tmp.x() <= maxx && tmp.y() <= maxy 
                    && tmp.x() >= minx && tmp.y() >= miny)
            {
                _dirv.push_back(v);
            }
        }  
    /*
    const int sh = 1 + _sheight/8;
    const int sl = 1 + _slarge/8;
    //Broodwar->printf("unit size : %d, %d\n", _sheight, _slarge);
    //Broodwar->printf("sh : %d, sl : %d\n", sh, sl);
    const int width = Broodwar->mapWidth();
    Position p = _unitPos();
    const int minx = max(p.x() - sl*8, 0);
    const int maxx = min(p.x() + sl*8, 32*width);
    const int miny = max(p.y() - sh*8, 0);
    const int maxy = min(p.y() + sh*8, 32*Broodwar->mapHeight());
    _dirv.clear();
    for (int x = - sl; x <= sl; ++x)
        for (int y = -sh; y <= sh; ++y)
        {
            Vec v(x, y);
            v *= 8;
            Position tmp = v.translate(p);
            if (tmp.x() <= maxx && tmp.y() <= maxy && tmp.x() >= minx && tmp.y() >= miny)
                _dirv.push_back(v);
        }
    */
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
    //pathFind(_path, _unitPos(), target);
    /*std::vector<TilePosition> path;
    pathFind(path, _unitPos(), target);
    path = std::vector<TilePosition>(getShortestPath(
        unit->getTilePosition(), target));
    if (path.size() == 0) 
        Broodwar->printf("Scandale path.size()==0");*/
    //unit->rightClick(path.pop....
	unit->attackMove(p);
}

void BayesianUnit::updateDir()
{
    Position p = this->_unitPos;

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
    //drawObj(_unitsGroup->size());
    //drawOccupation(_unitsGroup->size());

    // compute the probability to go in each dirv(ector)
    multimap<double, Vec> dirvProb;
    for (unsigned int i = 0; i < _dirv.size(); ++i)
        dirvProb.insert(make_pair(computeProb(i), _dirv[i]));
    multimap<double, Vec>::const_iterator last = dirvProb.end(); 
    // I want the right probabilities and not 1-prob in the multimap
    --last;
    // select the most probable
    if (dirvProb.count(dirvProb.begin()->first) == 1) 
    {
        dir = last->second;
    }
    // or the equally most probable and most in the direction of obj
    else
    {
        pair<multimap<double, Vec>::const_iterator, multimap<double, Vec>::const_iterator> possible_dirs = dirvProb.equal_range(last->first);
        double max = -100000.0;
        double max2 = -100000.0;
        for (multimap<double, Vec>::const_iterator it = possible_dirs.first; it != possible_dirs.second; ++it)
        {
            Vec tmpTest = it->second;
            Vec tmpVec = it->second;
            double tmp = obj.dot(tmpVec.normalize());

            if (tmp >= max && max2 < obj.dot(tmpTest))
            {
                max = tmp;
                max2 = obj.dot(tmpTest);
                dir = it->second;
            }
        }
    }

    //drawProbs(dirvProb, _unitsGroup->size());
}

void BayesianUnit::drawDir()
{
    Position up = _unitPos;
    if (dir.translate(up).y() <= Broodwar->mapHeight()*BWAPI::TILE_SIZE && dir.translate(up).x() <= Broodwar->mapWidth()*BWAPI::TILE_SIZE)
        Broodwar->drawLineMap(up.x(), up.y(), dir.translate(up).x(), dir.translate(up).y(), Colors::Red);
}

void BayesianUnit::clickDir()
{
    dir += _unitPos;
    if (_unitPos.getDistance(dir.toPosition()) >= 1.0) 
        unit->rightClick(dir.toPosition());
    else
        unit->stop();
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

void BayesianUnit::deleteRangeEnemiesElem(Unit* u)
{
    // TODO change with Boost's Multi-Index or BiMap to avoid the O(n) search in the map
    // http://www.boost.org/doc/libs/1_42_0/libs/multi_index/doc/examples.html#example4
    // http://www.boost.org/doc/libs/1_42_0/libs/bimap/doc/html/boost_bimap/one_minute_tutorial.html
    for (std::multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin(); it != _rangeEnemies.end(); ++it)
        if (it->second == u)
        {
            _rangeEnemies.erase(it);
            return;
        }
}

void BayesianUnit::updateRangeEnemiesWith(Unit* u)
{
    // TODO change with Boost's Multi-Index or BiMap to avoid the O(n) search in the map
    // http://www.boost.org/doc/libs/1_42_0/libs/multi_index/doc/examples.html#example4
    // http://www.boost.org/doc/libs/1_42_0/libs/bimap/doc/html/boost_bimap/one_minute_tutorial.html
    for (std::multimap<double, Unit*>::const_iterator it = _rangeEnemies.begin(); it != _rangeEnemies.end(); ++it)
        if (it->second == u)
        {
            std::pair<double, Unit*> temp(u->getDistance(_unitPos), u);
            _rangeEnemies.erase(it);
            _rangeEnemies.insert(temp);
            return;
        }
    std::pair<double, Unit*> temp(u->getDistance(_unitPos), u);
    _rangeEnemies.insert(temp);
}

void BayesianUnit::onUnitDestroy(Unit* u)
{
    deleteRangeEnemiesElem(u);
}

void BayesianUnit::onUnitShow(Unit* u)
{
    updateRangeEnemiesWith(u);
}

void BayesianUnit::onUnitHide(Unit* u)
{
    updateRangeEnemiesWith(u);
}

void BayesianUnit::update()
{
    if (!unit->exists()) return;
    _unitPos = unit->getPosition();

    //if (_sheight > 32 || _slarge > 32)
    //    Broodwar->printf("height: %d, large: %d", _sheight, _slarge);

    if (targetEnemy != NULL)// && withinRange(targetEnemy) /* && Qu'elle est capable de tirer .. */)
    {
        _mode = MODE_FIGHT_G;
        attackEnemy(targetEnemy, BWAPI::Colors::Red);
        return;
    }

    if (_mode == MODE_FLOCK || _mode == MODE_INPOS) 
    {
        updateDir();
        drawObj(0); // green
        drawDir(); // red
        clickDir();
        if (_mode != MODE_INPOS && _unitPos.getDistance(target) < WALK_TILES_SIZE*2)
        {
            Broodwar->printf("Switch INPOS!");
            this->switchMode(MODE_INPOS);
        }
        else if (_mode == MODE_INPOS && _unitPos.getDistance(target) > WALK_TILES_SIZE*18)
        {
            Broodwar->printf("Switch FLOCK!");
            this->switchMode(MODE_FLOCK);
        }
        //drawFlockValues();
    }

    drawTarget();
    return;
    Position p = _unitPos;
    if ((_mode == MODE_FLOCK && _mode == MODE_FLOCKFORM)
        && (p.getDistance(target) < 4 
            || (_ground_unit && BWTA::isConnected(TilePosition(p), TilePosition(target)))))
        switchMode(MODE_INPOS);

#ifdef __DEBUG_NICOLAS__
    this->drawTarget();
#endif
    
    if (_mode == MODE_FIGHT_G || 1) {
        // TODO not every update()s, perhaps even asynchronously
        // TODO inline function!
      /*
      if (!unit->getGroundWeaponCooldown()) {
            std::multimap<double, Unit*>::const_iterator rangeEnemyUnit;
            rangeEnemyUnit = _rangeEnemies.begin();
            unsigned int i = 0;
            unsigned int end = _rangeEnemies.size();
            while (i < end && rangeEnemyUnit != _rangeEnemies.end())
            {
                if (!rangeEnemyUnit->second->exists()) {
                    _rangeEnemies.erase(rangeEnemyUnit);
                    rangeEnemyUnit = _rangeEnemies.begin();
                    --end;
                    continue;
                }
                double enemyDistance = rangeEnemyUnit->second->getDistance(_unitPos());
                if (enemyDistance < unit->getType().groundWeapon().maxRange()) { // attack former closer if in range
                    // unit->rightClick(rangeEnemyUnit->second->getPosition());
                    break;
                } else { // replace former close that is now out of range in the right position
                    if (enemyDistance > unit->getType().groundWeapon().maxRange() + rangeEnemyUnit->second->getType().groundWeapon().maxRange()) {
                        _rangeEnemies.erase(rangeEnemyUnit);
                    } else {
                        std::pair<double, Unit*> temp = *rangeEnemyUnit;
                        _rangeEnemies.erase(rangeEnemyUnit);
                        _rangeEnemies.insert(temp);
                    }
                    ++rangeEnemyUnit;
                    ++i;
                }
            }
            if (++i == end) 
            {
                // NOT IMPL TODO
                // perhaps fill _rangeEnemies in the UnitsGroup (higher level)
                //Broodwar->printf("me think I have no enemy unit in range, me perhaps stoodpid!\n");
            }
        }

        */
        if (targetEnemy != NULL && unit->getGroundWeaponCooldown() == getTimeToAttack())//getTimeToAttack())
            attackEnemy(targetEnemy, BWAPI::Colors::Red);
        else if ( unit->isAttacking() && unit->getGroundWeaponCooldown() < 20)
            unit->rightClick(Position(_unitPos.x()-100, _unitPos.y()));
        /*else if (targetEnemy != NULL && unit->getGroundWeaponCooldown() > getTimeToAttack())
            unit->rightClick(targetEnemy);
        else if (targetEnemyInRange != NULL && unit->getGroundWeaponCooldown() <= getTimeToAttack())*/
        /*if (targetEnemyInRange != NULL && targetEnemyInRange->exists())
            attackEnemy(targetEnemyInRange, BWAPI::Colors::Red);*/

    } else if (_mode == MODE_FLOCK) {
        //if (tick())
        {
            drawAttractors();
            drawTarget();
            updateDir();
            drawDir();
            clickDir();
            drawFlockValues();
        }
        //Broodwar->drawLine(CoordinateType::Map, _unitPos().x(), _unitPos().y(), target.x(), target.y(), BWAPI::Color(92, 92, 92));
    }
    /*if (tick())
    {
        _path = BWTA::getShortestPath(unit->getTilePosition(), target);
        // pathFind(_path, _unitPos(), target);
        updateDir();
    }*/
    // _path = BWTA::getShortestPath(unit->getTilePosition(), target);
    //if (tick()) 
    //      pathFind(_path, _unitPos(), target);
    //      drawWalkability();
    //drawPath();
    // double velocity = sqrt(unit->getVelocityX()*unit->getVelocityX() + unit->getVelocityY()*unit->getVelocityY());
    // if (velocity > 0)
    //    Broodwar->printf("Velocity: %d\n", velocity);
    
    //drawVelocityArrow();
    //drawArrow(obj);
    //drawArrow(dir);
    //drawEnclosingBox();
    //drawPath();
}

std::multimap<double, BWAPI::Unit*>& BayesianUnit::getRangeEnemies()
{
    return _rangeEnemies;
}

void BayesianUnit::attackEnemy(BWAPI::Unit* u, BWAPI::Color col)
{
#ifdef __DEBUG_NICOLAS__
    int ux = _unitPos().x(); int uy = _unitPos().y();
    int ex = u->getPosition().x(); int ey = u->getPosition().y();

    Broodwar->drawLineMap(ux, uy, ex, ey, col);
#endif
    
    if (unit->getOrderTarget() != u)
    {
        unit->rightClick(u);
    }
}
