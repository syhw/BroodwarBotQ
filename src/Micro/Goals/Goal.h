#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Formations.h"

class UnitsGroup;

typedef enum
{
	GT_DEFEND_BASE  = 0,
	GT_ATTACK_BASE  = 1,
	GT_EXPLORE      = 2,
	GT_DROP         = 3,
	GT_PATROL       = 4,
	GT_BLOCK_SLOPE  = 5,
	GT_ND           = 0
} GoalType;

typedef enum
{
	GS_ND_STATUS           = 0,
	GS_ACHIEVED            = 0,
	GS_IN_PROGRESS         = 1,
	GS_MOVE_TO             = 2,
	GS_FLEE                = 3
} GoalStatus;



class Goal
{
protected:
	unsigned int achiviedCpt;

public:
	GoalType type;          /**< type of the goal */
	GoalStatus status;      /**< status of the goal */
	Formation* formation;   /**< formation to adopt during the goal */
	std::string purpose;    /**< string describing the purpose of the goal */

	//Goal();
	Goal(const Goal& g);
	Goal(const char* text, Formation* f);
	virtual ~Goal();

	virtual void achieve(UnitsGroup* ug);
	virtual void checkAchievement(UnitsGroup* ug);
};