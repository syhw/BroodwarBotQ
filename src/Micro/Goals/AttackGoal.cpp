#include <algorithm>
#include "AttackGoal.h"
#include "UnitsGroup.h"

using namespace BWAPI;

/*
AttackGoal::AttackGoal()
: Goal()
{
}

AttackGoal::AttackGoal(const AttackGoal& g)
: Goal( g)
{
}
*/
AttackGoal::AttackGoal( const Position& position, Unit* unit)
: Goal( "Attack", pFormation(new Formation(position)))
, unit (unit)
{
}

AttackGoal::~AttackGoal()
{
}

void AttackGoal::achieve(UnitsGroup* ug)
{
	status = GS_IN_PROGRESS;
	achiviedCpt = 0;
	ug->formation( this->formation);
}

void AttackGoal::checkAchievement(UnitsGroup* ug)
{
	if( Broodwar->isVisible( (int)formation->center.x/32, (int)formation->center.y/32) &&
		!unit->exists())
	{
		status = GS_ACHIEVED;
	}
}
