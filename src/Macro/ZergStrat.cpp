#include <ZergStrat.h>

ZergStrat::ZergStrat():MacroManager()
{
	initWantedUnits();
}

ZergStrat::~ZergStrat()
{

}

void ZergStrat::buildGeyser()
{
	/*
	std::set<Base*>& bases = baseManager->getActiveBases();
	for( set<Base*>::iterator it = bases.begin(); it != bases.end(); it++)
	{
		if( ((*it)->getGeysers().size() - (*it)->exploitedGeysers) && //Geyser remaining
			buildManager->getCompletedCount( UnitTypes::Zerg_Spawning_Pool) > 0 )
		{
			this->buildOrderManager->build(1,UnitTypes::Zerg_Extractor,60);
			(*it)->exploitedGeysers++; // TODO enlever les geyser detruits.
		}
	}
	*/
}

void ZergStrat::createProdBuildings()
{
	/*
	if( buildOrderManager->getUnusedMinerals() >= UnitTypes::Zerg_Hatchery.mineralPrice())
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Hatchery,20);
		underConstruction[UnitTypes::Zerg_Hatchery] = true;
	}
	*/
}

void ZergStrat::createTechBuildings()
{
	/*
	if( canCreateTechBuildings( UnitTypes::Zerg_Spawning_Pool, UnitTypes::None, 1))
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Spawning_Pool,40);
		techBuildings.insert( UnitTypes::Zerg_Spawning_Pool);
	}
	if( canCreateTechBuildings( UnitTypes::Zerg_Hydralisk_Den, UnitTypes::Zerg_Spawning_Pool, 1))
	{
		this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Hydralisk_Den,30);
		techBuildings.insert( UnitTypes::Zerg_Hydralisk_Den);
	}
	*/
}

void ZergStrat::initWantedUnits()
{
	wantedUnits[UnitTypes::Zerg_Zergling].plannedType = TYPE_RATIO;
	wantedUnits[UnitTypes::Zerg_Zergling].plannedValue = 1;
	wantedUnits[UnitTypes::Zerg_Hydralisk].plannedType = TYPE_RATIO;
	wantedUnits[UnitTypes::Zerg_Hydralisk].plannedValue = 2;
}

// TODO : faire en sorte de faire des units de tous les types repartis en fonction du ratio
// TODO : accorder la creation de workers et de combattants
void ZergStrat::trainUnits()
{
	/*
	std::list<BWAPI::Unit*> larva;
	const set<Unit*>& myUnits = Broodwar->self()->getUnits();
	for( set<Unit*>::const_iterator itMyUnits = myUnits.begin(); itMyUnits != myUnits.end(); itMyUnits++)
		if( (*itMyUnits)->getType() == UnitTypes::Zerg_Larva)
			larva.push_back((*itMyUnits));

	//TODO trier les units selon le ratio
	list<UnitType> sortedUnitsType;
	for( map<UnitType, PlannedUnit>::iterator wu = wantedUnits.begin(); wu != wantedUnits.end(); wu++)
	{
		list<UnitType>::iterator posToInsert = sortedUnitsType.begin();
		for( list<UnitType>::iterator sut = sortedUnitsType.begin(); sut != sortedUnitsType.end(); sut++)
		{
			if( wu->second.criteria > wantedUnits[*sut].criteria) break;
			posToInsert++;
		}
		sortedUnitsType.insert( posToInsert, wu->first);
	}

	if (sortedUnitsType.size() > 0)
	{
		list<UnitType>::iterator st = sortedUnitsType.begin();
		//Broodwar->printf("Units to train : %d : %s", sortedUnitsType.size(), (*st).getName().c_str());
		int nblarva = larva.size();
		for (list<BWAPI::Unit*>::iterator l = larva.begin(); l != larva.end(); l++)
		{
			if (canTrainUnits((*st), nblarva))
			{
				this->buildOrderManager->buildAdditional(1, (*st), 50);
				Broodwar->printf("Train : %s", (*st).getName().c_str());
				nblarva --;
			}
		}
	}
	*/
}
