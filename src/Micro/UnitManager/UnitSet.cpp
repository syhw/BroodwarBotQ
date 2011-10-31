#include "UnitSet.h"

using namespace BWAPI;

/*************** Constructors and destructors ***************/

/*
 *  \brief default constructor
 */
UnitSet::UnitSet()
{
	objective_ = ND_GO;
	status_ = ND_GS;
	curPos_ = Position(0,0);
	objPos_ = Position(0,0);
}

/*
 *  \brief init constructor (with the objective of the group)
 *  \param obj : the objective of the group
 *  \param pos : the position associated to the objective
 */
UnitSet::UnitSet(groupObjective_t obj,BWAPI::Position pos)
{
	objective_ = obj;
	status_ = ND_GS;
	curPos_ = Position(0,0);
	objPos_ = pos;
}

/**
 *  \brief init constructor
 *  \param pos : the position to regroup units
 */
UnitSet::UnitSet(BWAPI::Position regroupPos)
{
	objective_ = ND_GO;
	status_ = ND_GS;
	curPos_ = regroupPos;
	objPos_ = Position(0,0);
}

/*
 *  \brief destructor
 */
UnitSet::~UnitSet()
{
}


/************************* Accessors ************************/

/*
 *  \brief update the average position of the group
 */
void UnitSet::updateAvgPos()
{
	double x = 0;
	double y = 0;
	int nb = lUnits_.size();

	for(std::list<Unit*>::const_iterator i=lUnits_.begin();i!=lUnits_.end();i++)
    {
		x += (*i)->getPosition().x();
		y += (*i)->getPosition().y();
	}
	if(nb != 0) x /= nb;
	if(nb != 0) y /= nb;

	curPos_ = Position((int)x,(int)y);
}

/*
 *  \brief return the average position of the group
 *  \return the average position of the group
 */
BWAPI::Position UnitSet::pos() const
{
	return this->curPos_;
}

/**
 *  \brief return the average position of the objective
 *  \return the average position of the objective
 */
BWAPI::Position UnitSet::objPos() const
{
	return this->objPos_;
}


/************************** Actions *************************/

/*
 *  \brief move to the desired position
 *  \param pos : the position to go to
 */
void UnitSet::moveTo(BWAPI::Position pos)
{
	for(std::list<Unit*>::const_iterator i=lUnits_.begin();i!=lUnits_.end();i++)
    {
		(*i)->rightClick(pos);
	}
}

/*
 *  \brief attack to the desired position
 *  \param pos : the position to go to
 */
void UnitSet::attackTo( const BWAPI::Position pos)
{
	for(std::list<Unit*>::const_iterator i=lUnits_.begin();i!=lUnits_.end();i++)
    {
		(*i)->attackMove(pos);
	}
}

/*
 *  \brief patrol to the desired position
 *  \param pos : the position to go to
 */
void UnitSet::patrolTo(BWAPI::Position pos)
{
	for(std::list<Unit*>::const_iterator i=lUnits_.begin();i!=lUnits_.end();i++)
    {
		(*i)->patrol(pos);
	}
}

/*
 *  \brief stop the group
 */
void UnitSet::stop()
{
	for(std::list<Unit*>::const_iterator i=lUnits_.begin();i!=lUnits_.end();i++)
    {
		(*i)->stop();
	}
}


/********************** Objective evaluation ****************/

/**
 *  \brief check if the current group can fulfill the inpût objective
 *  \param obj : the input objective
 *  \return true if the group can fulfill the objective
 */
bool UnitSet::canFulFillObj(Objective* obj) const
{
	if(this->nbUnits() >= obj->reqPower()) return true;                // to test... must be changed
	else return false;
}


/********************** Units evaluation ********************/

/**
 *  \brief check if the current group requires the input type of unit
 *  \param unit : the input unit
 *  \return true if the group requires the input unit
 */
bool UnitSet::requires(BWAPI::Unit* unit) const
{
	return true;                                                       // to test... must be changed
}

/**
 *  \brief add a new unit to the group
 *  \param unit : the input unit
 */
void UnitSet::addUnit(BWAPI::Unit* unit)
{
	lUnits_.push_back(unit);
}
