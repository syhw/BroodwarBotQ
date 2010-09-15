#include "EUnitsFilter.h"
#include <QtGui/QTableWidgetItem>
#include <QtGui/QApplication>

using namespace BWAPI;
using namespace std;

EUnitsFilter::EUnitsFilter()
: BaseObject( "EUnitsFilter")
{
    timeManager = & TimeManager::Instance();

    _interestingTypes.insert(UnitTypes::Protoss_Arbiter);
    _interestingTypes.insert(UnitTypes::Protoss_Arbiter_Tribunal);
    _interestingTypes.insert(UnitTypes::Protoss_Archon);
    _interestingTypes.insert(UnitTypes::Protoss_Assimilator);
    _interestingTypes.insert(UnitTypes::Protoss_Carrier);
    _interestingTypes.insert(UnitTypes::Protoss_Citadel_of_Adun);
    _interestingTypes.insert(UnitTypes::Protoss_Corsair);
    _interestingTypes.insert(UnitTypes::Protoss_Cybernetics_Core);
    _interestingTypes.insert(UnitTypes::Protoss_Dark_Archon);
    _interestingTypes.insert(UnitTypes::Protoss_Dark_Templar);
    _interestingTypes.insert(UnitTypes::Protoss_Dragoon);
    _interestingTypes.insert(UnitTypes::Protoss_Fleet_Beacon);
    _interestingTypes.insert(UnitTypes::Protoss_Forge);
    _interestingTypes.insert(UnitTypes::Protoss_Gateway);
    _interestingTypes.insert(UnitTypes::Protoss_High_Templar);
    _interestingTypes.insert(UnitTypes::Protoss_Nexus);
    _interestingTypes.insert(UnitTypes::Protoss_Observatory);
    _interestingTypes.insert(UnitTypes::Protoss_Observer);
    _interestingTypes.insert(UnitTypes::Protoss_Photon_Cannon);
    _interestingTypes.insert(UnitTypes::Protoss_Probe);
    _interestingTypes.insert(UnitTypes::Protoss_Pylon);
    _interestingTypes.insert(UnitTypes::Protoss_Reaver);
    _interestingTypes.insert(UnitTypes::Protoss_Robotics_Facility);
    _interestingTypes.insert(UnitTypes::Protoss_Robotics_Support_Bay);
    _interestingTypes.insert(UnitTypes::Protoss_Scout);
    _interestingTypes.insert(UnitTypes::Protoss_Shield_Battery);
    _interestingTypes.insert(UnitTypes::Protoss_Shuttle);
    _interestingTypes.insert(UnitTypes::Protoss_Stargate);
    _interestingTypes.insert(UnitTypes::Protoss_Templar_Archives);
    _interestingTypes.insert(UnitTypes::Protoss_Zealot);
    _interestingTypes.insert(UnitTypes::Terran_Academy);
    _interestingTypes.insert(UnitTypes::Terran_Armory);
    _interestingTypes.insert(UnitTypes::Terran_Barracks);
    _interestingTypes.insert(UnitTypes::Terran_Battlecruiser);
    _interestingTypes.insert(UnitTypes::Terran_Bunker);
    _interestingTypes.insert(UnitTypes::Terran_Command_Center);
    _interestingTypes.insert(UnitTypes::Terran_Comsat_Station);
    _interestingTypes.insert(UnitTypes::Terran_Control_Tower);
    _interestingTypes.insert(UnitTypes::Terran_Covert_Ops);
    _interestingTypes.insert(UnitTypes::Terran_Dropship);
    _interestingTypes.insert(UnitTypes::Terran_Engineering_Bay);
    _interestingTypes.insert(UnitTypes::Terran_Factory);
    _interestingTypes.insert(UnitTypes::Terran_Firebat);
    _interestingTypes.insert(UnitTypes::Terran_Ghost);
    _interestingTypes.insert(UnitTypes::Terran_Goliath);
    _interestingTypes.insert(UnitTypes::Terran_Machine_Shop);
    _interestingTypes.insert(UnitTypes::Terran_Marine);
    _interestingTypes.insert(UnitTypes::Terran_Medic);
    _interestingTypes.insert(UnitTypes::Terran_Missile_Turret);
    _interestingTypes.insert(UnitTypes::Terran_Nuclear_Silo);
    _interestingTypes.insert(UnitTypes::Terran_Physics_Lab);
    _interestingTypes.insert(UnitTypes::Terran_Refinery);
    _interestingTypes.insert(UnitTypes::Terran_Science_Facility);
    _interestingTypes.insert(UnitTypes::Terran_SCV);
    _interestingTypes.insert(UnitTypes::Terran_Siege_Tank_Siege_Mode);
    _interestingTypes.insert(UnitTypes::Terran_Siege_Tank_Tank_Mode);
    _interestingTypes.insert(UnitTypes::Terran_Starport);
    _interestingTypes.insert(UnitTypes::Terran_Supply_Depot);
    _interestingTypes.insert(UnitTypes::Terran_Valkyrie);
    _interestingTypes.insert(UnitTypes::Terran_Vulture);
    _interestingTypes.insert(UnitTypes::Terran_Vulture_Spider_Mine);
    _interestingTypes.insert(UnitTypes::Terran_Wraith);
    _interestingTypes.insert(UnitTypes::Zerg_Extractor);
    _interestingTypes.insert(UnitTypes::Zerg_Infested_Command_Center);
    _interestingTypes.insert(UnitTypes::Zerg_Hatchery);
    _interestingTypes.insert(UnitTypes::Zerg_Lair);
    _interestingTypes.insert(UnitTypes::Zerg_Hive);
    _interestingTypes.insert(UnitTypes::Zerg_Nydus_Canal);
    _interestingTypes.insert(UnitTypes::Zerg_Hydralisk_Den);
    _interestingTypes.insert(UnitTypes::Zerg_Defiler_Mound);
    _interestingTypes.insert(UnitTypes::Zerg_Greater_Spire);
    _interestingTypes.insert(UnitTypes::Zerg_Queens_Nest);
    _interestingTypes.insert(UnitTypes::Zerg_Evolution_Chamber);
    _interestingTypes.insert(UnitTypes::Zerg_Ultralisk_Cavern);
    _interestingTypes.insert(UnitTypes::Zerg_Spire);
    _interestingTypes.insert(UnitTypes::Zerg_Spawning_Pool);
    _interestingTypes.insert(UnitTypes::Zerg_Creep_Colony);
    _interestingTypes.insert(UnitTypes::Zerg_Spore_Colony);
    _interestingTypes.insert(UnitTypes::Zerg_Sunken_Colony);
    _interestingTypes.insert(UnitTypes::Zerg_Lurker);
    _interestingTypes.insert(UnitTypes::Zerg_Devourer);
    _interestingTypes.insert(UnitTypes::Zerg_Zergling);
    _interestingTypes.insert(UnitTypes::Zerg_Hydralisk);
    _interestingTypes.insert(UnitTypes::Zerg_Ultralisk);
    _interestingTypes.insert(UnitTypes::Zerg_Drone);
    _interestingTypes.insert(UnitTypes::Zerg_Overlord);
    _interestingTypes.insert(UnitTypes::Zerg_Mutalisk);
    _interestingTypes.insert(UnitTypes::Zerg_Guardian);
    _interestingTypes.insert(UnitTypes::Zerg_Queen);
    _interestingTypes.insert(UnitTypes::Zerg_Defiler);
    _interestingTypes.insert(UnitTypes::Zerg_Scourge);
    _interestingTypes.insert(UnitTypes::Zerg_Infested_Terran);
}

void EUnitsFilter::update(Unit* u)
{
    if (u->getPlayer() == Broodwar->self())
        return;
    if (u->getPlayer()->isNeutral())
        return;
    if (!_interestingTypes.count(u->getType()))
        return;
    if (_eViewedUnits.count(u))
        _eViewedUnits[u].update(Broodwar->getFrameCount());
    else 
        _eViewedUnits[u] = EViewedUnit(u, Broodwar->getFrameCount());
    if ((!(u->isDetected()) || u->isCloaked() || u->isBurrowed()) && Broodwar->isVisible(_eViewedUnits[u].position))
    {
        _invisibleUnits[u] = std::make_pair<UnitType, Position>(u->getType(), u->getPosition());
        Broodwar->printf("Type %s", _eViewedUnits[u].type.getName().c_str());
    }
}

void EUnitsFilter::filter(Unit* u)
{
    if (_eViewedUnits[u].type.isBuilding()) return; // we consider that buildings don't move
    if (_invisibleUnits.count(u) && Broodwar->getFrameCount() - _eViewedUnits[u].lastSeen > 216) // 9 secondes
    {
        _invisibleUnits.erase(u);
        return;
    }
    if (Broodwar->getFrameCount() - _eViewedUnits[u].lastSeen > 2880) // 2 minutes
        _eViewedUnits.erase(u);
}

void EUnitsFilter::onUnitDestroy(Unit* u)
{
    if (u->isVisible() && u->getPlayer() != Broodwar->enemy()) return;
    _eViewedUnits.erase(u);
    _invisibleUnits.erase(u);
}

void EUnitsFilter::onUnitMorph(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitShow(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitHide(Unit* u)
{
    update(u);
}

void EUnitsFilter::onUnitRenegade(Unit* u)
{
    update(u);
}

void EUnitsFilter::update()
{
}

const std::map<BWAPI::Unit*, EViewedUnit>& EUnitsFilter::getViewedUnits()
{
    return _eViewedUnits;
}

EViewedUnit EUnitsFilter::getViewedUnit(Unit* u)
{
    return _eViewedUnits[u];
}

int EUnitsFilter::getNumbersType(UnitType ut)
{
    int count = 0;
    for (std::map<BWAPI::Unit*, EViewedUnit>::const_iterator it = _eViewedUnits.begin();
        it != _eViewedUnits.end(); ++it)
    {
        if (it->second.type == ut)
            ++count;
    }
    return count;
}

const std::map<Unit*, std::pair<UnitType, Position> >& EUnitsFilter::getInvisibleUnits()
{
    return _invisibleUnits;
}

bool EUnitsFilter::empty()
{
    return _eViewedUnits.empty();
}

void EUnitsFilter::bwOutput()
{
    for (std::map<BWAPI::Unit*, EViewedUnit>::const_iterator it = _eViewedUnits.begin(); 
        it != _eViewedUnits.end(); ++it)
        Broodwar->printf("Unit: %i", it->first);
}

#ifdef BW_QT_DEBUG

QWidget* EUnitsFilter::createWidget(QWidget* parent) const
{
    QWidget* qw = new QWidget(parent);
    qw->setObjectName(QString::fromUtf8("tab_EUFilter_test"));
    QHBoxLayout* horizontalLayout = new QHBoxLayout(qw);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout_EUFilter"));
    QTableWidget *EVUnits = new QTableWidget(qw);
    if (EVUnits->columnCount() < 5)
        EVUnits->setColumnCount(5);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(3, __qtablewidgetitem3);
    QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(4, __qtablewidgetitem4);
    EVUnits->setObjectName(QString::fromUtf8("EVUnits"));
    QTableWidgetItem *___qtablewidgetitem = EVUnits->horizontalHeaderItem(0);
    ___qtablewidgetitem->setText(QApplication::translate("MenuWidget", "pointer", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem1 = EVUnits->horizontalHeaderItem(1);
    ___qtablewidgetitem1->setText(QApplication::translate("MenuWidget", "unittype", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem2 = EVUnits->horizontalHeaderItem(2);
    ___qtablewidgetitem2->setText(QApplication::translate("MenuWidget", "last_pos", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem3 = EVUnits->horizontalHeaderItem(3);
    ___qtablewidgetitem3->setText(QApplication::translate("MenuWidget", "last_seen", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem4 = EVUnits->horizontalHeaderItem(4);
    ___qtablewidgetitem4->setText(QApplication::translate("MenuWidget", "HP", 0, QApplication::UnicodeUTF8));
    horizontalLayout->addWidget(EVUnits);

		return qw;
}

void EUnitsFilter::refreshWidget(QWidget* /*widget*/) const
{
}

#endif
