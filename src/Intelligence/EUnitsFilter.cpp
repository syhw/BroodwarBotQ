#include "EUnitsFilter.h"
#include <QtGui/QTableWidgetItem>
#include <QtGui/QApplication>

using namespace BWAPI;
using namespace std;

EUnitsFilter::EUnitsFilter()
: BaseObject( "EUnitsFilter")
{
    timeManager = & TimeManager::Instance();
}

void EUnitsFilter::update(Unit* u)
{
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    if (u->getType() == BWAPI::UnitTypes::Zerg_Larva 
        || u->getType() == BWAPI::UnitTypes::Zerg_Egg
        //|| u->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg
        || u->getType() == BWAPI::UnitTypes::Protoss_Interceptor
        || u->getType() == BWAPI::UnitTypes::Terran_Nuclear_Missile
        || u->getType() == UnitTypes::None
        || u->getType() == UnitTypes::Unknown)
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
    // we filter only invisible units
    if (_eViewedUnits[u].type.isBuilding()) return; // we consider that buildings don't move behind the scenes
    if (_invisibleUnits.count(u) && Broodwar->getFrameCount() - _eViewedUnits[u].lastSeen > 216) // 9 secondes
    {
        _invisibleUnits.erase(u);
        return;
    }
    if (Broodwar->getFrameCount() - _eViewedUnits[u].lastSeen > 7200) // 5 minutes
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
