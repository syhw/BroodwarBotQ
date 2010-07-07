#pragma once
#include <BWAPI.h>
#include "Formations.h"
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
	ST_VIEW  = 2,
	ST_ATTACK = 3,
	ST_REACH_F = 4, //In formation
	ST_ATTACK_F = 5, //In formation

} SubgoalType;

typedef enum
{
	SC_ONCE   = 0, //The subgoal must ve validated once to accomplish the goal
	SC_ACTIVE = 1//The subgoal must be valid all along the accomplishment
 	
} SubgoalCondition;

class Subgoal
{
public:
	//Constructors
	Subgoal(SubgoalType t, SubgoalCondition c, BWAPI::Position pos);
	Subgoal(SubgoalType t, SubgoalCondition c, pFormation formation);
	~Subgoal();

	//Accessors
	BWAPI::Position subgoalPosition() const;
	SubgoalType subgoalType() const;
	SubgoalCondition subgoalCondition() const;
	//Check accomplishment
	bool isRealized();//Depends on the condition
	//if SC_ACTIVE return if subgoal still valid
	//if SC_ONCE return if already accomplished

		
private:
	Subgoal();
	bool check();
	BWAPI::Position pos;
	SubgoalType type;
	SubgoalCondition cond;
	pFormation formation;    /**< formation to accomplish the subgoal if F in the type*/
	bool validated;

};