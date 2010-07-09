#include "EUnitsFilter.h"
#include <QtGui/QTableWidgetItem>
#include <QtGui/QApplication>

using namespace BWAPI;
using namespace std;

EUnitsFilter::EUnitsFilter()
:BaseObject( "EUnitsFilter")
{
    timeManager = & TimeManager::Instance();
}

void EUnitsFilter::update(Unit* u)
{
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    if (eViewedUnits.count(u))
        eViewedUnits[u].update(timeManager->getElapsedTime());
    else 
        eViewedUnits[u] = EViewedUnit(u, timeManager->getElapsedTime());
        //eViewedUnits.insert(make_pair(u, EViewedUnit(u, timeManager->getElapsedTime())));
}

void EUnitsFilter::onUnitDestroy(Unit* u)
{
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    eViewedUnits.erase(u);
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

void EUnitsFilter::onFrame()
{
// euh, c bizarre, il avait pas de onFrame ce composant ?
}

void EUnitsFilter::refreshWidget(QWidget* /*widget*/) const
{

}
