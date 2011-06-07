#include <PrecompiledHeader.h>
#include <Micro/WarManager.h>
#include "Macro/UnitGroupManager.h"
#include "Micro/UnitsGroup.h"
#include "Micro/Goals/AttackGoal.h"
#include "Intelligence/ScoutController.h"
#include "Macro/BWSAL.h"

using std::map;
using std::set;
using std::vector;
using std::list;
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
	//Broodwar->printf("INOUT WarManager::~WarManager()");
}

void WarManager::update()
{
	/*
	int count = 0;
	for (std::list<UnitsGroup *>::const_iterator it = this->unitsGroups.begin();
		it != unitsGroups.end(); ++it)
		count += (*it)->getUnits()->size();
	Broodwar->printf("I have: %d units groups and %d units total", unitsGroups.size(), count);
	*/

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
	/*std::set<BWAPI::Unit*> myUnits = BWAPI::Broodwar->self()->getUnits();
	for(std::set<BWAPI::Unit*>::iterator it = myUnits.begin(); it != myUnits.end(); ++it){
		if (!(*it)->getType().isBuilding() && !(*it)->getType().isWorker())
        {
			TheArbitrator->setBid(this, (*it), 20);
		}
	}*/
	//std::set<BWAPI::Unit*> usefulUnits=SelectAll()(isCompleted).not(isWorker,isBuilding);
	//TheArbitrator->setBid(this,usefulUnits,20);


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