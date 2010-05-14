#include "Objective.h"

/*************** Constructors and destructors ***************/

/**
 *  \brief default constructor
 *  Default objective is not defined
 */
Objective::Objective()
{
	type_ = ND_OBJ;
	status_ = OBJ_NOT_ASSIGNED;
	reqPower_ = 0;
	minReqPower_ = 0;
	baseLoc_ = NULL;
	pos_ = NULL;
}

/**
 *  \brief destructor
 */
Objective::~Objective()
{
	if(pos_ != NULL) delete pos_;
}


/************************* Accessors ************************/


/************************* Execution ************************/

/**
 *  \brief assign a group to the current objective
 *  \param group : the group to assign
 */
void Objective::assignGroup(UnitGroup* group)
{
	if(group != NULL && !group->empty()) lGroup_.push_back(group);
}
