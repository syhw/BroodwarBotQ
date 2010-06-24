#include "AttackBaseObj.h"

/*************** Constructors and destructors ***************/

/**
 *  \brief default constructor
 */
AttackBaseObj::AttackBaseObj()
{
	type_ = ATTACK_BASE_OBJ;
	status_ = OBJ_NOT_ASSIGNED;
	reqPower_ = 0;
	baseLoc_ = NULL;
	pos_ = NULL;
}

/**
 *  \brief init constructor
 *  \param baseLoc : the location of the base to attack
 */
AttackBaseObj::AttackBaseObj(BWTA::BaseLocation* baseLoc)
{
	type_ = ATTACK_BASE_OBJ;
	status_ = OBJ_NOT_ASSIGNED;
	reqPower_ = 5;                        // test value... to change
	minReqPower_ = 2;                     // test value... to change
	baseLoc_ = baseLoc;
	if(baseLoc_ != NULL) pos_ = new BWAPI::Position(baseLoc_->getPosition());
	else pos_ = NULL;
}

/**
 *  \brief default constructor
 */
AttackBaseObj::~AttackBaseObj()
{
}


/************************* Execution ************************/

/**
 *  \brief start the objective execution
 */
void AttackBaseObj::start()
{
}

/**
 *  \brief stop the objective execution
 */
void AttackBaseObj::stop()
{
}
