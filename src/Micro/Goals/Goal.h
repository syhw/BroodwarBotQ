#pragma once
#include <boost/shared_ptr.hpp>
#ifndef GOAL_SMART_POINTER
#define GOAL_SMART_POINTER
class Goal;
typedef boost::shared_ptr<Goal> pGoal;
#endif
#include "Subgoal.h"
#ifndef SUBGOAL_SMART_POINTER
#define SUBGOAL_SMART_POINTER
class Subgoal;
typedef boost::shared_ptr<Subgoal> pSubgoal;
#endif
class UnitsGroup;
#include "Macro/Arbitrator.h"
#include "Micro/UnitsGroup.h"
#include "Micro/Goals/GoalManager.h"
#include "Defines.h"
#include <BWAPI.h>
#include <BWTA.h>
#include <list>
#include <map>



typedef enum
{
	GS_WAIT_PRECONDITION   = 0,
	GS_IN_PROGRESS         = 1,
	GS_IN_CANCEL           = 2,
	GS_CANCELED            = 3,
	GS_ACHIEVED            = 4
} GoalStatus;

/***
 * Simple Goal class without canceling check in check()
 * and with a dummy canBidOn().
 * neededUnits/preconditions should perhaps be moved into a special Subgoal
 */
class Goal : public Arbitrator::Controller<BWAPI::Unit*, double>
{
	//friend class UnitsGroup;
	friend class GoalManager;
protected:
	/// Units it can use
	UnitsGroup _unitsGroup;
	std::list<BWAPI::Unit*> _incompleteUnits;
	/// Preconditions
	std::map<BWAPI::UnitType, int> _neededUnits;
	std::set<BWAPI::Unit*> _biddedOn;
    int _priority;
    int _firstFrame;
	/// Subgoals
	std::list<pSubgoal> _subgoals;
    /// Status
	GoalStatus _status;
	void bidOnUnitType(const BWAPI::UnitType& ut);
	void bidOnUnit(BWAPI::Unit* u);
public:
	Goal(int priority = 50, int firstFrame = 0);
	Goal(pSubgoal s, int priority = 50, int firstFrame = 0);
	Goal(const std::map<BWAPI::UnitType, int>& nU, pSubgoal s,
		int priority = 50, int firstFrame = 0);
	Goal(const std::map<BWAPI::UnitType, int>& nU,
		int priority = 50, int firstFrame = 0);
	virtual ~Goal();
	
	/// Controller methods
	virtual void onOffer(std::set<BWAPI::Unit*> objects);
	virtual void onRevoke(BWAPI::Unit* u, double bid);
    virtual std::string getName() const;
	virtual void update();

	void onUnitDestroy(BWAPI::Unit* unit);

	virtual void achieve();
	virtual void check();
	virtual void cancel(); // Does nothing, to be overwritten in cancelable goals
	virtual void canBidOn(BWAPI::Unit* u); // Does nothing, to be overwritten in "taking all possible units" goals

	void canBidOn(const std::set<BWAPI::Unit*>& setU); // Simple helper calling the canBidOn(Unit*) of the specialized Goal
	void addSubgoal(pSubgoal s);
	void addNeededUnits(const std::map<BWAPI::UnitType, int>& neededUnits);
	void setFirstFrame(int firstFrame);
	void setPriority(int priority);
	void setStatus(GoalStatus s);
	GoalStatus getStatus() const;
};
