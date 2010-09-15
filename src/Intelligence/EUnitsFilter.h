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

// This class works closely tied to MapManager (see MapManager::onFrame())
class EUnitsFilter : public CSingleton<EUnitsFilter>, public BaseObject
{
    friend class CSingleton<EUnitsFilter>;
    TimeManager* timeManager;
    std::set<BWAPI::UnitType> _interestingTypes;
    std::map<BWAPI::Unit*, EViewedUnit> _eViewedUnits;
    std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType, BWAPI::Position> > _invisibleUnits;
public:
    //std::map<BWAPI::Unit*, EViewedUnit> _eViewedUnits;

    void update(BWAPI::Unit* u);
    void filter(BWAPI::Unit* u);
    void onUnitDestroy(BWAPI::Unit* u);
    void onUnitMorph(BWAPI::Unit* u);
    void onUnitShow(BWAPI::Unit* u);
    void onUnitHide(BWAPI::Unit* u);
    void onUnitRenegade(BWAPI::Unit* u);

    const std::map<BWAPI::Unit*, EViewedUnit>& getViewedUnits();
    EViewedUnit getViewedUnit(BWAPI::Unit* u);
    int getNumbersType(BWAPI::UnitType ut);
    const std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType, BWAPI::Position> >& getInvisibleUnits();
    bool empty();
    void bwOutput();
    virtual void update();
#ifdef BW_QT_DEBUG
    virtual QWidget* createWidget(QWidget* parent) const;
    virtual void refreshWidget(QWidget* widget) const;
#endif
private:
    EUnitsFilter();
    ~EUnitsFilter() { }
};
