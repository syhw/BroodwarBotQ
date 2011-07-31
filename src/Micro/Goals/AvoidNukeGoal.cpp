#include <PrecompiledHeader.h>
#include "Micro/Goals/AvoidNukeGoal.h"
#include "Regions/MapManager.h"

using namespace BWAPI;
using namespace std;

/// Either we have detected the ghost(s) from the start and try and kill them
/// Or we have not and we flee the nuke
/// We assume that the detected ghost(s) around are responsible for the nuke
AvoidNukeGoal::AvoidNukeGoal(Position target)
: Goal(100) // highest priority
, _nukePos(target)
, _detectedGhost(false)
{
	for each (Unit* u in Broodwar->getUnitsInRadius(_nukePos, 12*32)) // 10 tiles for the Ghost upgraded vision...
	{
		if (u->getPlayer() == Broodwar->enemy() && u->getType() == UnitTypes::Terran_Ghost && u->isDetected())
			_ghostsToKill.push_back(u);
	}
	_detectedGhost = !_ghostsToKill.empty();

	// TODO change the following
	BWTA::BaseLocation* home = BWTA::getStartLocation(Broodwar->self());
	_safePos = home->getPosition(); 
	if (BWTA::getRegion(TilePosition(_safePos)) == home->getRegion())
	{
		if (!home->getRegion()->getReachableRegions().empty())
			_safePos = (*home->getRegion()->getReachableRegions().begin())->getCenter();
		else // Island...
			Broodwar->printf("Nuke on a home island, non implemented");
	}
	_status = GS_IN_PROGRESS;
}

void AvoidNukeGoal::fleeTheNuke()
{
	_unitsGroup.switchMode(MODE_MOVE);
	_unitsGroup.move(_safePos);
}

void AvoidNukeGoal::achieve()
{
	/// Check for the end of the Nuke
	if (Broodwar->getFrameCount() > 20*24 + _firstFrame) // 20 > 17 seconds for the nuke to fall
	{
		_status = GS_ACHIEVED;
		return;
	}
	/// Remove killed ghosts
	for (list<Unit*>::const_iterator it = _ghostsToKill.begin();
		it != _ghostsToKill.end(); )
	{
		if (!(*it)->exists()) // Assume we still detect it (TODO)
			_ghostsToKill.erase(it++);
		else
			++it;
	}
    /// (We could also put potential damages in the damages map?)
	for each (Unit* u in Broodwar->getUnitsInRadius(_nukePos, 9*32)) // bid all units in 9 build tiles around
		bidOnUnit(u);

	if (_detectedGhost)
	{
		/// Kill the ghosts
		if (_ghostsToKill.empty())
		{
			_status = GS_ACHIEVED;
		    return;
		}
		else if (Broodwar->getFrameCount() > 12*24 + _firstFrame)
		{
			/// To late to try and kill the ghost (14 seconds to drop the Nuke)
			fleeTheNuke();
		}
		else
		{
			for each (pBayesianUnit bu in _unitsGroup.units)
				bu->attackEnemyUnit(_ghostsToKill.front());
		}
	}
	else
	{
		/// Flee the nuke
		fleeTheNuke();
	}
}