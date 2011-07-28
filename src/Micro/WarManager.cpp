#include <PrecompiledHeader.h>
#include <Micro/WarManager.h>
#include "Macro/UnitGroupManager.h"
#include "Micro/UnitsGroup.h"
#include "Micro/Goals/AttackGoal.h"
#include "Intelligence/ScoutController.h"
#include "Macro/BWSAL.h"

using namespace std;
using namespace BWAPI;
using namespace BWTA;

WarManager::WarManager() 
: informationManager(TheInformationManager)
, home(BWTA::getStartLocation(Broodwar->self()))
{
    unitsGroups.push_front(new UnitsGroup()); // to garbage collect idle units
}

WarManager::~WarManager() 
{
	TheArbitrator->removeAllBids(this);
}

void WarManager::update()
{
//    if (!ScoutController::Instance().enemyFound && Broodwar->getFrameCount() > 4320)
//    {
//        for (std::list<UnitsGroup*>::iterator it = unitsGroups.begin();
//            it != unitsGroups.end(); ++it)
//        {
//			pGoal goal = pGoal(new AttackGoal(*it, ScoutController::Instance().enemyStartLocation));
//        }
//	}

    //Suppress the list prompted to suppress 
    if (!promptedRemove.empty())
    {
        for (std::list<UnitsGroup *>::iterator ug = this->promptedRemove.begin(); ug != this->promptedRemove.end(); ++ug)
        {
            if (*ug == NULL)
                continue;
            this->remove(*ug);
            (*ug)->idle();	//Set target of units to their position so that they are now idling
            // @merge ug->~UnitsGroup();
        }
        promptedRemove.clear();
    }


	//Set bid on appropriate units (not workers and not building)
	std::set<Unit*> usefulUnits = 
		SelectAll().not(isWorker, isBuilding);
	for (std::set<Unit*>::const_iterator it = usefulUnits.begin();
		it != usefulUnits.end(); ++it)
	{
		if (!(TheArbitrator->hasBid(*it)))
			TheArbitrator->setBid(this, *it, 20);
	}

	//Update unitsgroup;
	if (unitsGroups.empty()) return;
	for (std::list<UnitsGroup*>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); it++)
	{
        if ((*it)->size() > 2 && (*it)->emptyGoals())
            sendGroupToAttack(*it);
		else if ((*it)->emptyGoals())
			sendGroupToDefense(*it);
		(*it)->update();
	}
}


void WarManager::onOffer(std::set<BWAPI::Unit*> units)
{ 
    if (unitsGroups.empty()) // because each UnitsGroup gets destroyed when it becomes empty
    {
        unitsGroups.push_back(new UnitsGroup());
    }
	for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
	{
		if (!(*u)->getType().isWorker() && !(*u)->getType().isBuilding())
		{
			TheArbitrator->accept(this, *u);
			TheArbitrator->setBid(this, *u, 100);
            unitsGroups.front()->takeControl(*u);
			//Broodwar->printf("New %s added to the micro manager", (*u)->getType().getName().c_str());
		}
		else
		{
			TheArbitrator->decline(this, *u, 0);
		}
	}
}

void WarManager::onRevoke(BWAPI::Unit* unit, double bid)
{
	this->onUnitDestroy(unit);
}

std::string WarManager::getName() const
{
	return "War Manager";
}

void WarManager::onUnitCreate(BWAPI::Unit* unit)
{
	if (!unit->getType().isWorker() && unit->getPlayer()==Broodwar->self() && !unit->getType().isBuilding() && unit->getType().canAttack())
		TheArbitrator->setBid(this, unit, 20);
}

void WarManager::onUnitDestroy(BWAPI::Unit* unit)
{
	for (std::list<UnitsGroup*>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); )
	{
		(*it)->giveUpControl(unit);
		if ((*it)->emptyUnits())
		{
			delete (*it);
			unitsGroups.erase(it++);
		}
		else
			++it;
	}
}

void WarManager::sendGroupToAttack(UnitsGroup* ug)
{
	if (ScoutController::Instance().enemyFound)
    {
        ug->addGoal(pGoal(new AttackGoal(ug, ScoutController::Instance().enemyStartLocation)));
	} 
	else 
	{
		for (std::set<BWTA::BaseLocation*>::const_iterator it = BWTA::getStartLocations().begin();
			it != BWTA::getStartLocations().end(); ++it)
		{
			if (*it != BWTA::getStartLocation(Broodwar->self()))
				ug->addGoal(pGoal(new AttackGoal(ug, (*it)->getPosition())));
		}
	}
}

void WarManager::sendGroupToDefense(UnitsGroup* ug)
{
	if (home->getRegion()->getChokepoints().size() == 1)
	{
		ug->addGoal(pGoal(new AttackGoal(ug, (*(home->getRegion()->getChokepoints().begin()))->getCenter())));
	}
	else 
	{
		// TODO
	}
}


bool WarManager::remove(UnitsGroup* u)
{
	for(std::list<UnitsGroup *>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); )
    {
		if ((*it) == u)
        {
			unitsGroups.erase(it++);
			return true;
		}
        else
            ++it;
	}
	return false;
}

void WarManager::promptRemove(UnitsGroup* ug)
{
	this->promptedRemove.push_back(ug);
}