#include "AttackGoal.h"
#include "Subgoal.h"
#include "FormationSubgoal.h"
#include "Formations.h"

AttackGoal::AttackGoal(UnitsGroup* ug, BWAPI::Position p)
: Goal(ug, pSubgoal(new FormationSubgoal(SL_AND, pFormation(new SquareFormation(p)))))
{
}