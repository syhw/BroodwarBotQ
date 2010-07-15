#pragma once
#include "Defines.h"
#include "CSingleton.h"
#include "BaseObject.h"
#include "TimeManager.h"
#include "EViewedUnit.h"
#include <BWTA.h>
#include <BWAPI.h>
#include <map>
#include <utility>
#include "Data.h"

class EUnitsFilter : public CSingleton<EUnitsFilter>, public BaseObject
{
    TimeManager* timeManager;
    std::map<BWAPI::Unit*, EViewedUnit> _eViewedUnits;
public:
    EUnitsFilter();
    ~EUnitsFilter() { }
    void update(BWAPI::Unit* u);
	void onUnitDestroy(BWAPI::Unit* u);
	void onUnitMorph(BWAPI::Unit* u);
	void onUnitShow(BWAPI::Unit* u);
	void onUnitHide(BWAPI::Unit* u);
	void onUnitRenegade(BWAPI::Unit* u);

    void bwOutput();
	virtual void update();
	virtual QWidget* createWidget(QWidget* parent) const;
	virtual void refreshWidget(QWidget* widget) const;
};
