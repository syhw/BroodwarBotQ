#include "EEcoEstimator.h"

using namespace BWAPI;

EEcoEstimator::EEcoEstimator()
: BaseObject("EEcoEstimator")
, mWorkers(4)
, gWorkers(0)
, totWorkers(4)
, gaz_exploited(0)
, seconds(0)
, theo_workers(0)
{
	BWTA::BaseLocation * our_base = BWTA::getNearestBaseLocation(Broodwar->self()->getStartLocation());
	
	patch_owned = 0;

	for(std::set<BWAPI::Unit*>::const_iterator u = our_base->getMinerals().begin();u!=our_base->getMinerals().end();u++)
	{
		patch_owned ++;
	}

	gaz_owned = 0;
	
	for(std::set<BWAPI::Unit*>::const_iterator u = our_base->getGeysers().begin();u!=our_base->getGeysers().end();u++)
	{
		gaz_owned ++;
	}

	timeManager = NULL;
}

EEcoEstimator::~EEcoEstimator(){

}

void EEcoEstimator::setDependencies(){
	this->timeManager = & TimeManager::Instance();
}

void EEcoEstimator::update()
{
	//uses 300 as build time for worker
	if ((timeManager->getElapsedTime()%300)==299) {
			theo_workers++;
			mWorkers=theo_workers-gWorkers;
			//Broodwar->printf("Patch_Owned : %d, Number of workers: %d, m_per_mn: %d",patch_owned, theo_workers, m_per_mn());
	}
}

int EEcoEstimator::m_per_mn() const{
		double worker_by_patch = static_cast<double>(patch_owned) / static_cast<double>(mWorkers);
		if (worker_by_patch <= 1.0)
			return 59*mWorkers;
		else if (worker_by_patch <= 1.3)
			return 54*mWorkers;
		else if (worker_by_patch <= 1.6)
			return 50*mWorkers;
		else if (worker_by_patch <= 1.9)
			return 47*mWorkers;
		else if (worker_by_patch <= 2.2)
			return 45*mWorkers;
		else if (worker_by_patch <= 2.5)
			return 43*mWorkers;
		else
			return 41*mWorkers;
}

int EEcoEstimator::g_per_mn() const{
	return gWorkers*gaz_exploited*288;
}

void EEcoEstimator::start_gaz(int gWorkers){
	this->gWorkers += gWorkers;
	this->gaz_exploited++;
}

int EEcoEstimator::get_theo_workers() const{
	return theo_workers;
}

void EEcoEstimator::update_workers(int mworkers, int gworkers){
this->mWorkers = mworkers;
this->gWorkers = gworkers;
}

void EEcoEstimator::add_workers(int mworkers, int gworkers){
this->mWorkers += mworkers;
this->gWorkers += gworkers;
}

void EEcoEstimator::rm_workers(int mworkers, int gworkers){
this->mWorkers -= mworkers;
this->gWorkers -= gworkers;
}

#ifdef BW_QT_DEBUG
QWidget* EEcoEstimator::createWidget(QWidget* parent) const
{
	return new QLabel(QString("createWidget and refreshWidget undefined for this component."), parent);
}

void EEcoEstimator::refreshWidget(QWidget* widget) const
{
// TODO update your widget after having defined it in the previous method :)
}
#endif