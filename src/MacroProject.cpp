#include <PrecompiledHeader.h>
#include "MacroProject.h"
#include "Utils/Util.h"
#include "Macro/BWSAL.h"
#include <set>

#define __POOL_TIME_RUSH__ 130 // seconds, 3 workers + 1 pool + 11 seconds
#define __BBS_TIME_RUSH__ 230 // seconds, 4 workers + 2 barracks + 18 seconds
#define __GATES_TIME_RUSH__ 190 // seconds, 4 workers + 2 gateways + 18 seconds

using namespace BWAPI;
using namespace std;

// TODO TODO TODO: work with accessibility while making it false when onUnitEvade

BattleBroodAI::BattleBroodAI()
{
}

BattleBroodAI::~BattleBroodAI()
{
}

void BattleBroodAI::onStart()
{
    //Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
#ifdef __DEBUG__
    Broodwar->enableFlag(Flag::UserInput);
    Broodwar->setLocalSpeed(0);
#endif
	Broodwar->setLatCom(false);
	Broodwar->setCommandOptimizationLevel(1);

    BWTA::readMap();
    BWTA::analyze();
    this->analyzed=true;
   
	this->intelligence = & Intelligence::Instance();
	this->macro = & Macro::Instance();
	this->micro = & Micro::Instance();
    this->timeManager = & TimeManager::Instance();
}

void BattleBroodAI::onEnd(bool isWinner)
{
	Intelligence::Destroy();
	Macro::Destroy();
	Micro::Destroy();
    TimeManager::Destroy();

    if (isWinner)
    {
        //log win to file
    }
}

void BattleBroodAI::onFrame()
{
#ifdef __DEBUG__
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
	Broodwar->drawTextScreen(300, 20, "APM %d", Broodwar->getAPM());
    clock_t start = clock();

    if (Broodwar->getLastError() != BWAPI::Errors::None)
        Broodwar->printf("LAST ERROR: %s", Broodwar->getLastError().toString().c_str());
#endif
    if (Broodwar->isReplay()) return;
    if (!this->analyzed) return;

#ifdef BW_POS_MOUSE
    char mousePos[100];
	sprintf_s(mousePos, "%d, %d", 
		Broodwar->getScreenPosition().x() + Broodwar->getMousePosition().x(), 
		Broodwar->getScreenPosition().y() + Broodwar->getMousePosition().y());
    Broodwar->drawTextMouse(12, 0, mousePos);
	char mouseTilePos[100];
	sprintf_s(mouseTilePos, "%d, %d", 
		(Broodwar->getScreenPosition().x() + Broodwar->getMousePosition().x())/32, 
		(Broodwar->getScreenPosition().y() + Broodwar->getMousePosition().y())/32);
    Broodwar->drawTextMouse(12, 16, mouseTilePos);
#endif

    //timeManager->update();
	TheArbitrator->update();
    clock_t time1 = clock();
	intelligence->update(); // Intelligence update
    clock_t time2 = clock();
	double interval = (double)(time2 - time1)/CLOCKS_PER_SEC;
	if (interval > 0.040)
		Broodwar->printf("INTELLIGENCE took %2.5f seconds", interval);
	macro->update(); // Macro update
	time1 = clock();
	interval = (double)(time1 - time2)/CLOCKS_PER_SEC;
	if (interval > 0.040)
		Broodwar->printf("MACRO took %2.5f seconds", interval);
	micro->update(); // Micro update
	time2 = clock();
	interval = (double)(time2 - time1)/CLOCKS_PER_SEC;
	if (interval > 0.040)
		Broodwar->printf("MICRO took %2.5f seconds", interval);

#ifdef __DEBUG__
    std::set<Unit*> units=Broodwar->self()->getUnits();
    if (this->showManagerAssignments)
    {
        for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++)
        {
            if (TheArbitrator->hasBid(*i))
            {
                int x=(*i)->getPosition().x();
                int y=(*i)->getPosition().y();
                std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> > bids=TheArbitrator->getAllBidders(*i);
                int y_off=0;
                bool first = false;
                const char activeColor = '\x07', inactiveColor = '\x16';
                char color = activeColor;
                for(std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> >::iterator j=bids.begin();j!=bids.end();j++)
                {
                    Broodwar->drawTextMap(x,y+y_off,"%c%s: %d",color,j->first->getShortName().c_str(),(int)j->second);
                    y_off+=15;
                    color = inactiveColor;
                }
            }
        }
    }
#endif

#ifdef __DEBUG__
	clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;
    if (duration > 0.040) 
        Broodwar->printf("onFrame() took: %2.5f seconds\n", duration);
#endif
}

void BattleBroodAI::onUnitCreate(BWAPI::Unit* unit)
{
	intelligence->onUnitCreate(unit);
	micro->onUnitCreate(unit);
}

void BattleBroodAI::onUnitDestroy(BWAPI::Unit* unit)
{
	intelligence->onUnitDestroy(unit);
	macro->onUnitDestroy(unit);
	micro->onUnitDestroy(unit);
	TheArbitrator->onRemoveObject(unit);
}

void BattleBroodAI::onUnitShow(BWAPI::Unit* unit)
{
	intelligence->onUnitShow(unit);
	micro->onUnitShow(unit);
}

void BattleBroodAI::onUnitHide(BWAPI::Unit* unit)
{
	intelligence->onUnitHide(unit);
}

void BattleBroodAI::onUnitDiscover(BWAPI::Unit* unit)
{
	macro->onUnitDiscover(unit);
}

void BattleBroodAI::onUnitEvade(BWAPI::Unit* unit){
	macro->onUnitEvade(unit);
}

void BattleBroodAI::onUnitMorph(BWAPI::Unit* unit)
{
	intelligence->onUnitMorph(unit);
	macro->onUnitMorph(unit);
}

void BattleBroodAI::onUnitRenegade(BWAPI::Unit* unit)
{
	intelligence->onUnitRenegade(unit);
	macro->onUnitRenegade(unit);
}

void BattleBroodAI::onPlayerLeft(BWAPI::Player* player)
{
    //Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void BattleBroodAI::onNukeDetect(BWAPI::Position target)
{
    //if (target!=Positions::Unknown)
    //	Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
    //else
    //	Broodwar->printf("Nuclear Launch Detected");
}

void BattleBroodAI::onSendText(std::string text)
{
    if (text=="/show players")
    {
        showPlayers();
        return;
    } else if (text=="/show forces")
    {
        showForces();
        return;
    } else if (text=="/show visibility")
    {
        show_visibility_data=true;
    } else if (text=="/analyze")
    {
        if (analyzed == false)
        {
            // Broodwar->printf("Analyzing map... this may take a minute");
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
        }
        return;
    }
    else if (text.substr(0,9) == "/setspeed")
    {
        int x = 0;
        if (text[9] != ' ')
            Broodwar->printf("Usage: /setspeed SPEEDVALUE");
        std::string xx = text.substr(10);
        x = atoi(xx.c_str());
        //  Broodwar->printf("Set Speed %i", x);
        Broodwar->setLocalSpeed(x);
    }
    else if (text=="/target")
    {
        set<pBayesianUnit> tmp;
        // for (std::list<UnitsGroup*>::iterator it = this->warManager->unitsGroups.begin(); it != this->warManager->unitsGroups.end(); it++) // why HEAD does have a different warManager->unitsGroups? @merge
        for (std::list<UnitsGroup*>::iterator it = micro->warManager->unitsGroups.begin(); it != micro->warManager->unitsGroups.end(); it++)
        {
            set<pBayesianUnit> tmp;
            (*it)->selectedUnits(tmp);
            if (tmp.size() == 0)
                //    Broodwar->printf("No selected units");
                for (std::set<pBayesianUnit>::const_iterator i = tmp.begin(); i != tmp.end(); ++i)
                {
                    //  Broodwar->printf("Target de l'unité : (%i, %i)", (*i)->target.x(), (*i)->target.y());
                    Position pos(Broodwar->getScreenPosition() + Broodwar->getMousePosition());
                    // Broodwar->printf("Curseur : (%i, %i)", pos.x(), pos.y());
                }
        }
    }

    UnitType type=UnitTypes::getUnitType(text);
    if (text=="debug")
    {
        this->showManagerAssignments=true;
        return;
    }
    /*if (type!=UnitTypes::Unknown)
    {
		macro->buildOrderAdd(type);
    }
    else
    {
        TechType type=TechTypes::getTechType(text);
        if (type!=TechTypes::Unknown)
        {
			macro->techAdd(type);
        }
        else
        {
            UpgradeType type=UpgradeTypes::getUpgradeType(text);
            if (type!=UpgradeTypes::Unknown)
            {
				macro->upgradeAdd(type);
            }
            else
                Broodwar->printf("You typed '%s'!",text.c_str());
        }
    }*/
    return;
}

void BattleBroodAI::onReceiveText(BWAPI::Player* player, std::string text)
{
	return;
}

void BattleBroodAI::onSaveGame(std::stringbuf gameName)
{
	return;
}

DWORD WINAPI AnalyzeThread()
{
    BWTA::analyze();
    analyzed   = true;
    analysis_just_finished = true;

    //self start location only available if the map has base locations
    if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
    {
        home = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
    }
    //enemy start location only available if Complete Map Information is enabled.
    if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
    {
        enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
    }
    return 0;
}

void BattleBroodAI::drawStats()
{
    std::set<Unit*> myUnits = Broodwar->self()->getUnits();
    Broodwar->drawTextScreen(5,0,"I have %d units:",myUnits.size());
    std::map<UnitType, int> unitTypeCounts;
    for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
    {
        if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
        {
            unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
        }
        unitTypeCounts.find((*i)->getType())->second++;
    }
    int line=1;
    for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
    {
        Broodwar->drawTextScreen(5,16*line,"- %d %ss",(*i).second, (*i).first.getName().c_str());
        line++;
    }
}

void BattleBroodAI::showPlayers()
{
    std::set<Player*> players=Broodwar->getPlayers();
    for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
    {
        //   Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
    }
}
void BattleBroodAI::showForces()
{
    std::set<Force*> forces=Broodwar->getForces();
    for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
    {
        std::set<Player*> players=(*i)->getPlayers();
        //  Broodwar->printf("Force %s has the following players:",(*i)->getName().c_str());
        for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
        {
            //      Broodwar->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
        }
    }
}