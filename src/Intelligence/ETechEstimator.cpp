#include "ETechEstimator.h"	

using namespace BWAPI;

Building::Building(UnitType t, double p)
: type(t)
, probToHave(p)
{
}

ETechEstimator::ETechEstimator()
{
    //Select the types 
    std::set<UnitType> aut = BWAPI::UnitTypes::allUnitTypes();

    //Select the enemy Player
    std::set<Player*> players = BWAPI::Broodwar->getPlayers();
    Player * opponent;
    for (std::set<Player*>::iterator pl = players.begin(); pl != players.end(); ++ pl)
    {
        if ((*pl)->isEnemy(Broodwar->self())) 
        {
            opponent = (*pl);
            break;
        }
    }
    Race eRace = opponent->getRace();
    if (eRace == BWAPI::Races::Random) {
        BWAPI::Broodwar->printf(" My opponent is random, ETechEstimator feels bad and cannot foresee anything");
    }

    //Set all buildings
    for (std::set<UnitType>::iterator it = aut.begin(); it != aut.end(); ++it)
    {
        if (it->getRace() == eRace && it->isBuilding() && (*it)==eRace.getCenter())
        {
            //Case of the Command center/ Nexus / Hatch
            Building building((*it),1.0);
            buildings.push_back(building);

        }
        else if (it->getRace() == eRace && it->isBuilding() && (*it)!=eRace.getCenter())
        {
            //Any other building
            Building building((*it),0.0);
            buildings.push_back(building);
        }
    }
    //Set dependencies (needed field)
    for (std::list<Building>::iterator itb = buildings.begin(); itb != buildings.end(); ++itb)
    {
        std::map< UnitType, int > needs = itb->type.requiredUnits();

        for (std::map< UnitType, int >::iterator itneeds = needs.begin(); itneeds != needs.end(); ++itneeds)
        {
            itb->needed.push_back(this->getBuilding(itneeds->first));
        }
    }


}

Building ETechEstimator::getBuilding(BWAPI::UnitType ut)
{
    for (std::list<Building>::iterator it = buildings.begin(); it != buildings.end(); ++it)
    {
        if ( (*it).type == ut )
        {
            return (*it);
        }
    }
    return Building(ut, -1.0);
}

ETechEstimator::~ETechEstimator()
{
}

double ETechEstimator::canProduce(BWAPI::UnitType ut)
{
    double res = 1.0;
    for (std::map< UnitType, int >::const_iterator itneeds = ut.requiredUnits().begin(); itneeds != ut.requiredUnits().end(); ++itneeds)
    {
        res *= getBuilding(itneeds->first).probToHave;
    }
    return res;
}

double ETechEstimator::hasTech(BWAPI::TechType tt)
{
    //Probability to have


    //Probability to research


    //Result
}

double ETechEstimator::hasUpgrade(BWAPI::UpgradeType upt)
{
}

void ETechEstimator::updateProbs()
{
    //Update probs of buildings


    //Probs to 1 must not change
    for (std::list<Building>::iterator it = buildings.begin(); it != buildings.end(); ++it)
    {
        if(it->probToHave == 1.0)
        {

        } else {

        }

    }
    //Linearly increase probs between 
}

void ETechEstimator::sayProbs()
{
    //Useful only for debug
    for (std::list<Building>::iterator it = buildings.begin(); it != buildings.end(); ++it)
    {

    }
}