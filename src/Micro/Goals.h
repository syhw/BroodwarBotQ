#ifndef _SMART_POINTER_GOAL_INCLUDED
#define _SMART_POINTER_GOAL_INCLUDED 1
#include <boost/shared_ptr.hpp>
class Goal;
typedef boost::shared_ptr<Goal> pGoal;
#endif
#include "Goals/Goal.h"
#include "Goals/DefendGoal.h"
#include "Goals/AttackGoal.h"
