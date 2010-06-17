#ifndef _SMART_POINTER_GOAL
#define _SMART_POINTER_GOAL 1
#include <boost/shared_ptr.hpp>
class Goal;
typedef boost::shared_ptr<Goal> pGoal;
#endif
#include "Goals/Goal.h"
#include "Goals/DefendGoal.h"
#include "Goals/AttackGoal.h"
