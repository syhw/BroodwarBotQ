#pragma once
#include <BWAPI.h>
#include <list>

#include "Objective.h"
class Objective;

typedef enum
{
	DEFEND_BASE_GO = 0,
	ATTACK_BASE_GO = 1,
	EXPLORE_GO     = 2,
	DROP_GO        = 3,
	PATROL_GO      = 4,
	BLOCK_SLOPE_GO = 5,
	ND_GO          = 0
} groupObjective_t;

typedef enum
{
	OBJ_IN_PROGRESS_GS = 1,
	MOVE_TO_OBJ_GS     = 2,
	WAIT_4_UNITS_GS    = 3,
	FLEE_OBJ_GS        = 4,
	ND_GS              = 0
} groupStatus_t;

class UnitGroup
{
protected : 
	std::list<BWAPI::Unit*> lUnits_;          /**< list of units contained in the group */
	groupObjective_t objective_;              /**< objective of the group */
	groupStatus_t status_;                    /**< status of the group */
	BWAPI::Position curPos_;                  /**< average postion of the group on the map */
	BWAPI::Position objPos_;                  /**< average postion of the objective of the group */

	/*************** Constructors and destructors ***************/
public : 
	/**
	 *  \brief default constructor
	 */
	UnitGroup();

	/**
	 *  \brief init constructor (with the objective of the group)
	 *  \param obj : the objective of the group
	 *  \param pos : the position associated to the objective
	 */
	UnitGroup(groupObjective_t obj,BWAPI::Position pos);

	/**
	 *  \brief init constructor
	 *  \param pos : the position to regroup units
	 */
	UnitGroup(BWAPI::Position regroupPos);

	/**
	 *  \brief destructor
	 */
	~UnitGroup();

	/************************* Accessors ************************/

protected : 
	/**
	 *  \brief update the average position of the group
	 */
	void updateAvgPos();

public : 
	/**
	 *
	 */
	inline BWAPI::Position curPos() const { return this->curPos_; }

	/**
	 *  \brief return the average position of the group
	 *  \return the average position of the group
	 */
	BWAPI::Position pos() const;

	/**
	 *  \brief return the average position of the objective
	 *  \return the average position of the objective
	 */
	BWAPI::Position objPos() const;

	/**
	 *  \brief check if the group is empty
	 *  \return true if the group is empty
	 */
	inline bool empty() const { return lUnits_.empty(); }

	/**
	 *  \brief return the number of units that belong to the current group
	 *  \return the number of units that belong to the current group
	 */
	inline int nbUnits() const { return lUnits_.size(); }

	/************************** Actions *************************/

public : 
	/**
	 *  \brief move to the desired position
	 *  \param pos : the position to go to
	 */
	void moveTo(BWAPI::Position pos);

	/**
	 *  \brief attack to the desired position
	 *  \param pos : the position to go to
	 */
	void attackTo(const BWAPI::Position pos);

	/**
	 *  \brief patrol to the desired position
	 *  \param pos : the position to go to
	 */
	void patrolTo(BWAPI::Position pos);

	/**
	 *  \brief stop the group
	 */
	void stop();

	/******************** Objective evaluation ******************/

	/**
	 *  \brief check if the current group can fulfill the input objective
	 *  \param obj : the input objective
	 *  \return true if the group can fulfill the objective
	 */
	bool canFulFillObj(Objective* obj) const;

	/********************** Units management ********************/

	/**
	 *  \brief check if the current group requires the input type of unit
	 *  \param unit : the input unit
	 *  \return true if the group requires the input unit
	 */
	bool requires(BWAPI::Unit* unit) const;

	/**
	 *  \brief add a new unit to the group
	 *  \param unit : the input unit
	 */
	void addUnit(BWAPI::Unit* unit);
};
