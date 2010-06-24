#pragma once
#include <BWAPI.h>
#include <BWTA.h>

#include "Objective.h"

class AttackBaseObj : public Objective
{
protected : 

	/*************** Constructors and destructors ***************/

protected : 
	/**
	 *  \brief default constructor
	 */
	AttackBaseObj();

public : 
	/**
	 *  \brief init constructor
	 *  \param baseLoc : the location of the base to attack
	 */
	AttackBaseObj(BWTA::BaseLocation* baseLoc);

	/**
	 *  \brief default constructor
	 */
	~AttackBaseObj();

	/************************* Execution ************************/

	/**
	 *  \brief start the objective execution
	 */
	virtual void start();

	/**
	 *  \brief stop the objective execution
	 */
	virtual void stop();
};