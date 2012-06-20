#include <PrecompiledHeader.h>
#include "Utils/Util.h"
#include "MicroProject.h"
#include "Micro/Goals/Goal.h"
#include "Micro/Goals/FormationSubgoal.h"
#include "Micro/Goals/AttackGoal.h"
#include "Micro/Tactician.h"
#include "Macro/BWSAL.h"

#ifdef __LEARNING_PROB_TABLES__
#include "ZealotUnit.h"
#endif

using namespace BWAPI;
using namespace std;

// TODO change all the mapWidth/Height - mp.x()/y() (+4) by eStartLocation.x()/y()

void MicroAIModule::onStart()
{
#ifdef __LEARNING_PROB_TABLES__
	ZealotUnit::initProbTables();
#endif
	///Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
	// Enable some cheat flags
    //Broodwar->printf("ON START !!\n");
#ifdef __DEBUG__
    Broodwar->enableFlag(Flag::UserInput);
    //Broodwar->setLocalSpeed(0);
#endif
	//Broodwar->enableFlag(Flag::CompleteMapInformation);
	BWTA::readMap();
	BWTA::analyze();
    this->eUnitsFilter = & EUnitsFilter::Instance();
    this->mapManager = & MapManager::Instance();
	this->unitGroupManager = UnitGroupManager::create();
	goalManager = & GoalManager::Instance();
	TheArbitrator = & arbitrator;

    std::string mapName = Broodwar->mapPathName();
    mapName = mapName.substr(mapName.find_last_of("\\") + 1, mapName.length() - mapName.find_last_of("\\") - 1);
    Broodwar->sendText("map name: %s", mapName.c_str());

	std::set<Unit*> allUnits = Broodwar->getAllUnits();

	BWAPI::TilePosition mp = Broodwar->self()->getStartLocation();
    for (std::set<BWAPI::TilePosition>::const_iterator it = Broodwar->getStartLocations().begin();
        it != Broodwar->getStartLocations().end(); ++it)
    {
        if (*it == mp)
            continue;
        else
            eStartLocation = *it;
    }
	Position p;
	for(std::set<BWAPI::TilePosition>::iterator l=Broodwar->getStartLocations().begin(); l!=Broodwar->getStartLocations().end(); l++)
	{
		if (!((*l) == mp))
			p = BWAPI::Position(*l);
    }

    int sign = mp.x() < Broodwar->mapWidth()/2*32 ? 1 : -1;

	pFormation tmp_form;
	pSubgoal tmp_subgoal;
	pGoal tmp_goal;

	list<pGoal> goals;
		 
    /*if (mapName == std::string("muta.scm") || mapName == std::string("mutaCOMPUTER.scm"))
    {
        tmp_form = pFormation(new ArcFormation(
            Position(Broodwar->mapWidth()/2*32 + sign*350,(Broodwar->mapHeight()/2 + 4)*32), 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
		tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, NULL, tmp_form));
        tmp_goal = pGoal(new AttackGoal(tmp_subgoal, 90));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal( 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32),
			80)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()*32 - 128, Broodwar->mapHeight()*32 - 128),
			70)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()*32, 128),
			60)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(128, Broodwar->mapHeight()*32),
			50)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(128, 128),
			45)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()*32 - 128, Broodwar->mapHeight()*32 - 128),
			40)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()*32, 128),
			35)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(128, Broodwar->mapHeight()*32),
			30)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(128, 128),
			25)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()/2*3, 22*32),
			20)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()/2*32, 50*32),
			15)));
    }
    else if (mapName == std::string("zealots.scm") || mapName == std::string("zealotsCOMPUTER.scm"))
    {
        tmp_form = pFormation(new ArcFormation(
            Position(Broodwar->mapWidth()/2*32 + sign*350,(Broodwar->mapHeight()/2 + 4)*32), 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, NULL, tmp_form));
        tmp_goal = pGoal(new AttackGoal(tmp_subgoal, 90));
        goals.push_back(tmp_goal);
        tmp_form = pFormation(new ArcFormation(
            Position(Broodwar->mapWidth()/2*32 + sign*450,(Broodwar->mapHeight()/2 + 4)*32), 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, NULL, tmp_form));
        tmp_goal = pGoal(new AttackGoal(tmp_subgoal, 80));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal(
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32), 70)));
        tmp_form = pFormation(new LineFormation(
            Position(Broodwar->mapWidth()/2*32,(Broodwar->mapHeight()/2 + 4)*32), Vec(1,0)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, NULL, tmp_form));
        tmp_goal = pGoal(new AttackGoal(tmp_subgoal, 60));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal( 
            Position((Broodwar->mapWidth() - (Broodwar->mapWidth() - mp.x()))*32, (Broodwar->mapHeight()/2 + 4)*32), 50)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()/2*32, 22*32), 40)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()/2*32, 50*32), 30)));
    }
	else if (mapName == std::string("formation.scm"))
	{
		tmp_form = pFormation(new SquareFormation(
			Position(Broodwar->mapWidth()/2*32, (Broodwar->mapHeight()/2 + 4)*32)));
		tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, NULL, tmp_form));
		tmp_goal = pGoal(new AttackGoal(tmp_subgoal));
		goals.push_back(tmp_goal);
	}
    else*/
    {
        tmp_form = pFormation(new ArcFormation(
            Position(Broodwar->mapWidth()/2*32 + sign*350,(Broodwar->mapHeight()/2 + 4)*32), 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, NULL, tmp_form));
        tmp_goal = pGoal(new AttackGoal(tmp_subgoal, 90));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal(
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32), 80)));
        tmp_form = pFormation(new LineFormation(
            Position(Broodwar->mapWidth()/2*32,(Broodwar->mapHeight()/2 + 4)*32), Vec(1,0)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, NULL, tmp_form));
        tmp_goal = pGoal(new AttackGoal(tmp_subgoal, 70));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal( 
            Position((Broodwar->mapWidth() - (Broodwar->mapWidth() - mp.x()))*32, (Broodwar->mapHeight()/2 + 4)*32), 60)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()/2*32, 22*32), 50)));
        goals.push_back(pGoal(new AttackGoal( 
            Position(Broodwar->mapWidth()/2*32, 50*32), 40)));
    }

	goalManager->addGoals(goals);

}

void MicroAIModule::onFrame()
{
    mapManager->update();
	TheArbitrator->update();
	goalManager->update();
    if (Broodwar->getLastError() != BWAPI::Errors::None)
        Broodwar->printf("LAST ERROR: %s", Broodwar->getLastError().toString().c_str());
    
#ifdef BW_POS_MOUSE
    char mousePos[100];
    sprintf_s(mousePos, "%d, %d", Broodwar->getScreenPosition().x()+Broodwar->getMousePosition().x(), Broodwar->getScreenPosition().y()+Broodwar->getMousePosition().y());
    Broodwar->drawTextMouse(12, 0, mousePos);
#endif

#ifdef __DEBUG__
    //we will iterate through all the regions and draw the polygon outline of it in green.
	for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
	{
		BWTA::Polygon p=(*r)->getPolygon();
		for(int j=0;j<(int)p.size();j++)
		{
			Position point1=p[j];
			Position point2=p[(j+1) % p.size()];
			Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Green);
		}
	}
#endif

    //Broodwar->printf("weapons ground upgrade level %d", Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Weapons));
    //Broodwar->printf("weapons ground upgrade level %d", eUnitsFilter->getViewedUnits().begin()->first->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Weapons));
    //Broodwar->printf("%s", Broodwar->getLastError().toString().c_str());
}

MicroAIModule::~MicroAIModule()
{
}

MicroAIModule::MicroAIModule()
{
}

void MicroAIModule::onEnd(bool isWinner)
{
    MapManager::Destroy();
	GoalManager::Destroy();
#ifdef __LEARNING_PROB_TABLES__ ////////// Auto restart games
	myRestartGame();
#endif
	logScore(isWinner, Broodwar->mapPathName());
}

void MicroAIModule::onSendText(std::string text)
{
	if (text=="/show stats")
	{
		showStats();
		return;
	} else if (text=="/show players")
	{
		showPlayers();
		return;
	} else if (text=="/show forces")
	{
		showForces();
		return;
	} else if (text=="/analyze")
	{
		if (analyzed == false)
		{
			Broodwar->printf("Analyzing map... this may take a minute");
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
		}
		return;
    } else if (text=="/euf") // eUnitsFilter
    {
#ifdef __DEBUG__
        eUnitsFilter->bwOutput();
#endif
	} else
	{
		Broodwar->printf("You typed '%s'!",text.c_str());
	}
	return;
}

void MicroAIModule::onUnitCreate(Unit* unit)
{
    mapManager->onUnitCreate(unit);
	goalManager->onUnitCreate(unit);
}

void MicroAIModule::onUnitDestroy(Unit* unit)
{
	TheArbitrator->onRemoveObject(unit);
    goalManager->onUnitDestroy(unit);
    eUnitsFilter->onUnitDestroy(unit);
    mapManager->onUnitDestroy(unit);
}

void MicroAIModule::onUnitShow(Unit* unit)
{
    eUnitsFilter->onUnitShow(unit);
    mapManager->onUnitShow(unit);
    unitGroupManager->onUnitDiscover(unit);
}

void MicroAIModule::onUnitHide(Unit* unit)
{
    eUnitsFilter->onUnitHide(unit);
    mapManager->onUnitHide(unit);
    unitGroupManager->onUnitEvade(unit);
}

DWORD WINAPI AnalyzeThread()
{
	BWAPI::Broodwar->printf("just entered AnalyseThread()");
	int a = 69;
	for(int i = 0; i < 50000000; i++) a = 42;
	int b = a;
	BWTA::analyze();
	analyzed   = true;
	analysis_just_finished = true;
	home       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
	enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
	return 0;
}

void MicroAIModule::showStats()
{
	std::set<Unit*> myUnits = Broodwar->self()->getUnits();
	Broodwar->printf("I have %d units:",myUnits.size());
	std::map<UnitType, int> unitTypeCounts;
	for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
	{
		if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
		{
			unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
		}
		unitTypeCounts.find((*i)->getType())->second++;
	}
	for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
	{
		Broodwar->printf("- %d %ss",(*i).second, (*i).first.getName().c_str());
	}
}

void MicroAIModule::showPlayers()
{
	std::set<Player*> players=Broodwar->getPlayers();
	for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
	{
		Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
	}
}
void MicroAIModule::showForces()
{
	std::set<Force*> forces=Broodwar->getForces();
	for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
	{
		std::set<Player*> players=(*i)->getPlayers();
		Broodwar->printf("Force %s has the following players:",(*i)->getName().c_str());
		for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
		{
			Broodwar->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
		}
	}
}