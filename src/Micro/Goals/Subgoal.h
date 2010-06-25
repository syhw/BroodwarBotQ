#pragma once
#include <BWAPI.h>
#include <boost/shared_ptr.hpp>
#ifndef SUBGOAL_SMART_POINTER
#define SUBGOAL_SMART_POINTER
class Subgoal;
typedef boost::shared_ptr<Subgoal> pSubgoal;
#endif

typedef enum  
{
	ST_REACH_BY_UG = 0, //UG==UnitGroup
	ST_REACH = 1,//By anybody
	ST_VIEW  = 2

} SubgoalType;

typedef enum
{
	SC_ACTIVE = 0,//The subgoal must be valid when accomplishing the goal
	SC_ONCE   = 1 //The subgoal must ve validated once to accomplish the goal
 	
} SubgoalCondition;

class Subgoal
{
public:
	Subgoal();
	Subgoal(SubgoalType t, SubgoalCondition c, BWAPI::Position pos);
	~Subgoal();

	BWAPI::Position subgoalPosition() const;
	SubgoalType subgoalType() const;
	SubgoalCondition subgoalCondition() const;
	bool isRealized();//Depends on the condition
	//if SC_ACTIVE return if subgoal still valid
	//if SC_ONCE return if already accomplished

private:
	bool check();
	BWAPI::Position pos;
	SubgoalType type;
	SubgoalCondition cond;
	bool validated;
};