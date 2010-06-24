#pragma once
#include <CSingleton.h>
#include <BWAPI.h>

#include <UnitGroup.h>
#include <Regions.h>

class UnitManager: public CSingleton<UnitManager>
{
	friend class CSingleton<UnitManager>;

private:
	/**
	 *  \brief default constructor
	 */
	UnitManager();

	/**
	 *  \brief destructor
	 */
	~UnitManager();

protected : 
	std::list<BWAPI::Unit*> lUnits_;          /**< list of all available units */
	std::list<UnitGroup*> lGroup_;            /**< list of the groups of units managed */
	std::list<Objective*> lObjective_;        /**< list of the objective to fulfill */
	Regions* regions;

	/*************** Constructors and destructors ***************/

public : 

	/************************* Accessors ************************/

	/**
	 *  \brief return the name of the class
	 *  \return the name of the class
	 */
    std::string getName() const;

	/************************** Update **************************/

	/**
	 *  \brief update the unit manager at each time step
	 */
	void update();

	/**
	 *  \brief check for new objectives to create
	 */
	void checkNewObjectives();

	/**
	 *  \brief assign groups of units to the current objectives to fulfill
	 */
	void assignGroupToObj();

	/******* Modification of the list of available units ********/

	/**
	 *  \brief add a new unit
	 *  \param unit : the new created unit
	 */
	void onUnitCreate(BWAPI::Unit* unit);

	/**
	 *  \brief add a new unit
	 *  \param unit : the new created unit
	 */
	void onUnitDestroy(BWAPI::Unit* unit);
};
