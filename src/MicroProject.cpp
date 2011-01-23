#include "MicroProject.h"
#include "Goal.h"
#include "FormationSubgoal.h"
#include "AttackGoal.h"
#include <stdio.h>
#include <string>

using namespace BWAPI;
using namespace std;

// TODO change all the mapWidth/Height - mp.x()/y() (+4) by eStartLocation.x()/y()

void MicroAIModule::onStart()
{
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
    this->eTechEstimator = & ETechEstimator::Instance();
    this->mapManager = & MapManager::Instance();
	this->objectManager = & ObjectManager::Instance();
    this->unitGroupManager = & UnitGroupManager::Instance();
    this->mapManager->setDependencies();

	mm = new UnitsGroup();

    std::string mapName = Broodwar->mapPathName();
    mapName = mapName.substr(mapName.find_last_of("\\") + 1, mapName.length() - mapName.find_last_of("\\") - 1);
    Broodwar->sendText("map name: %s", mapName.c_str());

	// Vec center;
	std::set<Unit*> allUnits = Broodwar->getAllUnits();

	for (std::set<Unit*>::iterator i=allUnits.begin(); i!=allUnits.end(); i++)
	{
        onUnitShow(*i);
        if ((*i)->getType().isBuilding())
		{
			this->buildings.insert(std::make_pair(*i,(*i)->getType()));
            // regions->onUnitCreate(*i);
            mapManager->onUnitCreate(*i);
		} 
        else 
        {
            if ((*i)->getPlayer() != Broodwar->self())
                continue;
            else
                mm->takeControl(*i);
        }
	}

	std::list<pGoal> goals;

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

    /* LINE IN THE MIDDLE (+/- 64 pixs) */   
    /*pFormation tmp_form = pFormation(new LineFormation(
        Position(Broodwar->mapWidth()/2*32 + sign*64,(Broodwar->mapHeight()/2 + 4)*32), Vec(1,0)));
    pSubgoal tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    pGoal tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);*/

    /* SQUARE IN THE MIDDLE */
    /*pFormation tmp_form = pFormation(new SquareFormation(
        Position(Broodwar->mapWidth()/2*32, (Broodwar->mapHeight()/2 + 4)*32)));
    pSubgoal tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    pGoal tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);*/

    /* BALL IN THE MIDDLE (+/- 256 pixs) */
    goals.push_back(pGoal(new AttackGoal(mm, 
        Position(Broodwar->mapWidth()/2*32 - sign*256, (Broodwar->mapHeight()/2 + 4)*32))));

    /* ARC IN THE MIDDLE (+/- 256 pixs) */
    pFormation tmp_form = pFormation(new ArcFormation(
        Position(Broodwar->mapWidth()/2*32 - sign*256,(Broodwar->mapHeight()/2 + 4)*32), 
        Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
    pSubgoal tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    pGoal tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);

    /* ARC IN THE MIDDLE (+/- 128 pixs) */
    tmp_form = pFormation(new ArcFormation(
        Position(Broodwar->mapWidth()/2*32 - sign*128,(Broodwar->mapHeight()/2 + 4)*32), 
        Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
    tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);
    
    /* ARC IN THE MIDDLE */
    tmp_form = pFormation(new ArcFormation(
        Position(Broodwar->mapWidth()/2*32,(Broodwar->mapHeight()/2 + 4)*32), 
        Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
    tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);

    /* ARC IN THE MIDDLE (+/- 128 pixs) */
    tmp_form = pFormation(new ArcFormation(
        Position(Broodwar->mapWidth()/2*32 + sign*128,(Broodwar->mapHeight()/2 + 4)*32), 
        Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
    tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);

    /* ARC IN THE MIDDLE (+/- 256 pixs) */
    tmp_form = pFormation(new ArcFormation(
        Position(Broodwar->mapWidth()/2*32 + sign*256,(Broodwar->mapHeight()/2 + 4)*32), 
        Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
    tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);

    /* ARC IN THE MIDDLE (+/- 312 pixs) */
    tmp_form = pFormation(new ArcFormation(
        Position(Broodwar->mapWidth()/2*32 + sign*312,(Broodwar->mapHeight()/2 + 4)*32), 
        Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
    tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);


    /* SQUARE ON THE OTHER SIDE */
    tmp_form = pFormation(new SquareFormation(
        Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
    tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
    tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
    goals.push_back(tmp_goal);
    
    if (mapName == std::string("muta.scm") || mapName == std::string("mutaCOMPUTER.scm"))
    {
        tmp_form = pFormation(new ArcFormation(
            Position(Broodwar->mapWidth()/2*32 + sign*350,(Broodwar->mapHeight()/2 + 4)*32), 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
        tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()*32 - 128, Broodwar->mapHeight()*32 - 128))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()*32, 128))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(128, Broodwar->mapHeight()*32))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(128, 128))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()*32 - 128, Broodwar->mapHeight()*32 - 128))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()*32, 128))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(128, Broodwar->mapHeight()*32))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(128, 128))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()/2*32, 22*32))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()/2*32, 50*32))));
    }
    else if (mapName == std::string("zealots.scm") || mapName == std::string("zealotsCOMPUTER.scm"))
    {
        tmp_form = pFormation(new ArcFormation(
            Position(Broodwar->mapWidth()/2*32 + sign*350,(Broodwar->mapHeight()/2 + 4)*32), 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
        tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
        goals.push_back(tmp_goal);
        tmp_form = pFormation(new ArcFormation(
            Position(Broodwar->mapWidth()/2*32 + sign*450,(Broodwar->mapHeight()/2 + 4)*32), 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
        tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32))));
        tmp_form = pFormation(new LineFormation(
            Position(Broodwar->mapWidth()/2*32,(Broodwar->mapHeight()/2 + 4)*32), Vec(1,0)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
        tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position((Broodwar->mapWidth() - (Broodwar->mapWidth() - mp.x()))*32, (Broodwar->mapHeight()/2 + 4)*32))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()/2*32, 22*32))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()/2*32, 50*32))));
    }
    else
    {
        tmp_form = pFormation(new ArcFormation(
            Position(Broodwar->mapWidth()/2*32 + sign*350,(Broodwar->mapHeight()/2 + 4)*32), 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
        tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position((Broodwar->mapWidth() - mp.x())*32, (Broodwar->mapHeight()/2 + 4)*32))));
        tmp_form = pFormation(new LineFormation(
            Position(Broodwar->mapWidth()/2*32,(Broodwar->mapHeight()/2 + 4)*32), Vec(1,0)));
        tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
        tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
        goals.push_back(tmp_goal);
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position((Broodwar->mapWidth() - (Broodwar->mapWidth() - mp.x()))*32, (Broodwar->mapHeight()/2 + 4)*32))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()/2*32, 22*32))));
        goals.push_back(pGoal(new AttackGoal(mm, 
            Position(Broodwar->mapWidth()/2*32, 50*32))));
    }

	mm->setGoals(goals);

#ifdef BW_QT_DEBUG
	g_onStartDone = true;
#endif
}

void MicroAIModule::onFrame()
{
    if (Broodwar->getLastError() != BWAPI::Errors::None)
        Broodwar->printf("LAST ERROR: %s", Broodwar->getLastError().toString().c_str());
#ifdef BW_QT_DEBUG
    if (!*qapp)
        Broodwar->printf("Qt not connected\n");
#endif
    objectManager->onFrame();
	if (mm != NULL) 
        mm->update();
    // regions->display();
    if (!enemiesFound && mm->enemies.size())
    {
        enemiesFound = true;
        if (mm->enemiesCenter.getDistance(Position(eStartLocation)) > 128.0)
        {
            /// stacking goals
            /* Attack in their direction */
            pGoal tmp_goal = pGoal(new AttackGoal(mm, mm->enemiesCenter));
            mm->addGoalFront(tmp_goal);
            /* ARC in their direction (- 192 pixs) */
            Vec direction = Vec(mm->center.x() - mm->enemiesCenter.x(), mm->center.y() - mm->enemiesCenter.y());
            direction.normalize();
            direction *= 192;
            Position arcCenter = direction.translate(mm->enemiesCenter);
            pFormation tmp_form = pFormation(new ArcFormation(
                arcCenter, mm->enemiesCenter));
            pSubgoal tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
            tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
            mm->addGoalFront(tmp_goal);
            /* ARC in their direction (- 288 pixs) */
            direction *= 1.5;
            arcCenter = direction.translate(mm->enemiesCenter);
            tmp_form = pFormation(new ArcFormation(
                arcCenter, mm->enemiesCenter));
            tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
            tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
            mm->addGoalFront(tmp_goal);
            /* ARC in their direction (- 360 pixs) */
            direction *= 1.25;
            arcCenter = direction.translate(mm->enemiesCenter);
            tmp_form = pFormation(new ArcFormation(
                arcCenter, mm->enemiesCenter));
            tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
            tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
            mm->addGoalFront(tmp_goal);
            /* Line far */
            direction *= 1.25;
            arcCenter = direction.translate(mm->enemiesCenter);
            direction *= -1.0;
            tmp_form = pFormation(new LineFormation(
                arcCenter, direction));
            tmp_subgoal = pSubgoal(new FormationSubgoal(SL_AND, tmp_form));
            tmp_goal = pGoal(new Goal(mm, tmp_subgoal));
            mm->addGoalFront(tmp_goal);
        }
    }
    
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

    mapManager->update();
    //Broodwar->printf("weapons ground upgrade level %d", Broodwar->enemy()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Weapons));
    //Broodwar->printf("weapons ground upgrade level %d", eUnitsFilter->getViewedUnits().begin()->first->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Weapons));
    //Broodwar->printf("%s", Broodwar->getLastError().toString().c_str());
}

MicroAIModule::~MicroAIModule()
{
#ifdef BW_QT_DEBUG
    (*qapp)->quit();
#endif
    MapManager::Destroy();
    ObjectManager::Destroy();
    // regions::Destroy();
    mm->~UnitsGroup();
}

#ifdef BW_QT_DEBUG
MicroAIModule::MicroAIModule(QApplication** qapplication)
: enemiesFound(false)
{
    qapp = qapplication;
}
#else
MicroAIModule::MicroAIModule()
{
}
#endif

void MicroAIModule::onEnd(bool isWinner)
{
	char buffer[5];
	if (isWinner)
	{
		sprintf_s(buffer, "won");
	} 
	else
	{
		sprintf_s(buffer, "lost");
	}
    std::string mapName = Broodwar->mapPathName();
	mapName = mapName.substr(mapName.find_last_of("\\") + 1, mapName.length() - mapName.find_last_of("\\") - 1);
	FILE *outfile;
	if (fopen_s(&outfile, "bwapi-data\\logs\\wins_loses.log", "a+")==0)
	{
		fprintf_s(outfile, "%s on map %s", buffer, mapName.c_str());
		fclose(outfile);
	}
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
        eUnitsFilter->bwOutput();
    } else if (text.substr(0,3) == "/go")
    {
        int x = 0;
        int y = 0;
        if (text[3] != ' ')
            Broodwar->printf("Usage: /go XCOORD YCOORD;\n");
        int lx = text.find(' ', 4) - 4;
        int ly = text.find(';') - (4+lx);
        std::string xx = text.substr(4, 4+lx);
        x = atoi(xx.c_str());
        std::string yy = text.substr(4+lx+1, 4+lx+ly);
        y = atoi(yy.c_str());
        Broodwar->printf("lx: %d | ly: %d", lx, ly);
        set<pBayesianUnit> tmp;
        mm->selectedUnits(tmp);
        for (set<pBayesianUnit>::iterator it = tmp.begin(); it != tmp.end(); ++it)
        {
            (*it)->target = Position(x, y);
        }
        Broodwar->printf("We will go in %d, %d\n", x, y);
	} else
	{
		Broodwar->printf("You typed '%s'!",text.c_str());
	}
	return;
}

void MicroAIModule::onUnitCreate(Unit* unit)
{
    mapManager->onUnitCreate(unit);
    // regions->onUnitCreate(unit);
}

void MicroAIModule::onUnitDestroy(Unit* unit)
{
    eUnitsFilter->onUnitDestroy(unit);
    mapManager->onUnitDestroy(unit);
    // regions->onUnitDestroy(unit);
    mm->onUnitDestroy(unit);
}

void MicroAIModule::onUnitShow(Unit* unit)
{
    eUnitsFilter->onUnitShow(unit);
    mapManager->onUnitShow(unit);
	// regions->onUnitShow(unit);
    unitGroupManager->onUnitDiscover(unit);
    mm->onUnitShow(unit);
}

void MicroAIModule::onUnitHide(Unit* unit)
{
    eUnitsFilter->onUnitHide(unit);
    mapManager->onUnitHide(unit);
	// regions->onUnitHide(unit);
    unitGroupManager->onUnitEvade(unit);
    mm->onUnitHide(unit);
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
