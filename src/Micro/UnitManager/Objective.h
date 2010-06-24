#pragma once
#include <BWAPI.h>
#include <BWTA.h>

#include "UnitManager/UnitGroup.h"
class UnitGroup;

typedef enum
{
	DEFEND_BASE_OBJ = 0,
	ATTACK_BASE_OBJ = 1,
	EXPLORE_OBJ     = 2,
	DROP_OBJ        = 3,
	PATROL_OBJ      = 4,
	BLOCK_SLOPE_OBJ = 5,
	ND_OBJ          = 0
} objective_t;

typedef enum
{
	OBJ_IN_PROGRESS   = 1,
	MOVE_TO_OBJ       = 2,
	OBJ_NOT_ASSIGNED  = 3,
	FLEE_OBJ          = 4,
	ND_OBJ_STATUS     = 0
} objectiveStatus_t;

class Objective
{
protected : 
	objective_t type_;                                  /**< type of the objective */
	objectiveStatus_t status_;                          /**< status of the objective */
	double reqPower_;                                   /**< required power of the group to start the objective */
	double minReqPower_;                                /**< minimal required power of the group to not abort the objective */
	BWTA::BaseLocation* baseLoc_;                       /**< the base location associated to the objective */
	BWAPI::Position* pos_;                              /**< the position associated to the objective */
	std::list<UnitGroup*> lGroup_;                      /**< list of the groups used to fulfill the objective */

	/*************** Constructors and destructors ***************/

public : 
	/**
	 *  \brief default constructor
	 *  Default objective is not defined
	 */
	Objective();

	/**
	 *  \brief destructor
	 */
	virtual ~Objective();

	/************************* Accessors ************************/

	/**
	 *  \brief return the objective type
	 *  \return the objective type
	 */
	inline objective_t type() { return this->type_; }

	/**
	 *  \brief return the objective status
	 *  \return the objective status
	 */
	inline objectiveStatus_t status() { return this->status_; }

	/**
	 *  \brief return the required power needed to ensure the objective
	 *  \return the required power needed to ensure the objective
	 */
	inline double reqPower() { return this->reqPower_; }

	/**
	 *  \brief return the base location associated to the objective
	 *  \return the base location associated to the objective
	 */
	inline BWTA::BaseLocation* baseLoc() { return this->baseLoc_; }

	/**
	 *  \brief return the position associated to the objective
	 *  \return the position associated to the objective
	 */
	inline BWAPI::Position* pos() { return this->pos_; }

	/************************* Execution ************************/

	/**
	 *  \brief start the objective execution
	 */
	virtual void start() = 0;

	/**
	 *  \brief stop the objective execution
	 */
	virtual void stop() = 0;

	/**
	 *  \brief assign a group to the current objective
	 *  \param group : the group to assign
	 */
	void assignGroup(UnitGroup* group);
};