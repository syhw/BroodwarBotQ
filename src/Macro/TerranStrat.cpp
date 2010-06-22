#include <TerranStrat.h>

TerranStrat::TerranStrat():MacroManager()
{
	initWantedUnits();
}

TerranStrat::~TerranStrat()
{
}

void TerranStrat::buildGeyser()
{
	std::set<Base*>& bases = baseManager->getActiveBases();
	for( set<Base*>::iterator it = bases.begin(); it != bases.end(); it++)
	{
		if( ((*it)->getGeysers().size() - (*it)->exploitedGeysers) && //Geyser remaining
			buildManager->getCompletedCount( UnitTypes::Terran_Barracks) > 0 )
		{
			this->buildOrderManager->build(1,UnitTypes::Terran_Refinery,60);
			(*it)->exploitedGeysers++; // TODO enlever les geyser detruits.
		}
	}
}

void TerranStrat::createTechBuildings()
{
	if( canCreateTechBuildings( UnitTypes::Terran_Academy, UnitTypes::Terran_Barracks, 1))
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Academy,70);
		techBuildings.insert( UnitTypes::Terran_Academy);
	}
}

void TerranStrat::createProdBuildings()
{
	bool allBarracksFull = true;
	const list<Unit*>& buildings = prodBuildings[UnitTypes::Terran_Barracks];
	for( list<Unit*>::const_iterator it = buildings.begin(); it != buildings.end(); it++)
		if(!(*it)->isCompleted() || !(*it)->isTraining())
		{
			allBarracksFull = false;
			break;
		}
	if( allBarracksFull && !underConstruction[UnitTypes::Terran_Barracks])
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Barracks,20);
		underConstruction[UnitTypes::Terran_Barracks] = true;
	}
}

void TerranStrat::initWantedUnits()
{
	wantedUnits[UnitTypes::Terran_Marine].plannedType = TYPE_RATIO;
	wantedUnits[UnitTypes::Terran_Marine].plannedValue = 4;
	//wantedUnits[UnitTypes::Terran_Firebat].plannedType = TYPE_RATIO;
	//wantedUnits[UnitTypes::Terran_Firebat].plannedValue = 1;
	wantedUnits[UnitTypes::Terran_Medic].plannedType = TYPE_RATIO;
	wantedUnits[UnitTypes::Terran_Medic].plannedValue = 2;
}
