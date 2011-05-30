#include <PrecompiledHeader.h>
#include <WarManager.h>
#include <util.h>
#include <UnitsGroup.h>
#include "AttackGoal.h"
#include "ScoutController.h"
#include "BWSAL.h"

using std::map;
using std::set;
using std::vector;
using std::list;
using namespace BWAPI;
using namespace BWTA;

WarManager::WarManager() 
{
	arbitrator = NULL;
    unitsGroups.push_front(new UnitsGroup()); // to garbage collect idle units
    informationManager = TheInformationManager;
    home = BWTA::getStartLocation(Broodwar->self());
}

WarManager::~WarManager() 
{
	//Broodwar->printf("INOUT WarManager::~WarManager()");
}

void WarManager::setDependencies()
{
	this->arbitrator = TheArbitrator;
}

void WarManager::update()
{
    /*if (!ScoutController::Instance().enemyFound && Broodwar->getFrameCount() > 4320)
    {
        for (std::list<UnitsGroup*>::iterator it = unitsGroups.begin();
            it != unitsGroups.end(); ++it)
        {
            pGoal goal = pGoal(new AttackGoal(*it, Scout
        }
        }*/

    //Suppress the list prompted to suppress 
    if (!promptedRemove.empty())
    {
        for (std::list<UnitsGroup *>::iterator ug = this->promptedRemove.begin(); ug != this->promptedRemove.end(); ++ug)
        {
            if (*ug == NULL)
                continue;
            this->remove(*ug);
            (*ug)->idle();	//Set target of units to their position so that they are now idling
            delete (*ug); // @merge
            // @merge ug->~UnitsGroup();
        }
        promptedRemove.clear();
    }


	//Set bid on appropriate units (not workers and not building)
	std::set<BWAPI::Unit *> myUnits = BWAPI::Broodwar->self()->getUnits();

	for(std::set<BWAPI::Unit *>::iterator it = myUnits.begin(); it != myUnits.end(); ++it){
		if (!(*it)->getType().isBuilding() && !(*it)->getType().isWorker())
        {
			this->arbitrator->setBid(this, (*it), 20);
		}
	}

	//Update unitsgroup;
	if (unitsGroups.empty()) return;
	for (std::list<UnitsGroup*>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); it++)
	{
        if ((*it)->size() > 2 && (*it)->emptyGoals())
            sendGroupToAttack(*it);
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
			arbitrator->accept(this, *u);
            unitsGroups.front()->takeControl(*u);
			//Broodwar->printf("New %s added to the micro manager", (*u)->getType().getName().c_str());
		}
		else
		{
			arbitrator->decline(this, *u, 0);
		}
	}
}

void WarManager::onRevoke(BWAPI::Unit* unit, double bid)
{
	// @merge this->onUnitDestroy(unit);
}

std::string WarManager::getName() const
{
	return "War Manager";
}

void WarManager::onUnitCreate(BWAPI::Unit* unit)
{
	/*
	if (!unit->getType().isWorker() && unit->getPlayer()==Broodwar->self() && !unit->getType().isBuilding() && unit->getType().canAttack())
		arbitrator->setBid(this, unit, 100);
	*/
}

void WarManager::onUnitDestroy(BWAPI::Unit* unit)
{
	for (std::list<UnitsGroup*>::iterator it = unitsGroups.begin(); it != unitsGroups.end(); ++it)
	{
		(*it)->giveUpControl(unit);
		if( (*it)->emptyUnits() )
		{
			this->promptRemove(*it);
		}
	}
}

void WarManager::sendGroupToAttack( UnitsGroup* ug)
{
	if (ScoutController::Instance().enemyFound)
    {
        ug->addGoal(pGoal(new AttackGoal(ug, ScoutController::Instance().enemyStartLocation)));
	}
}

void WarManager::sendGroupToDefense( UnitsGroup* ug)
{

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