#include <algorithm>
#include "AttackGoal.h"
#include "UnitsGroup.h"

using namespace BWAPI;


AttackGoal::AttackGoal(BWAPI::Position pos)
:
pos(pos)
{

}

AttackGoal::AttackGoal(const AttackGoal& g)
: Goal( g)
{
}


AttackGoal::~AttackGoal()
{
}


