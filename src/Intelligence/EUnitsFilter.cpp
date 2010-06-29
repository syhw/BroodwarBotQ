#include "EUnitsFilter.h"
#include "Data.inl"

using namespace BWAPI;
using namespace std;

EUnitsFilter::EUnitsFilter()
: BaseObject(std::string("EUnitsFilter"))
, eViewedUnits(std::map<Unit*, EViewedUnit>(), "EViewedUnits", this)
{
    timeManager = & TimeManager::Instance();
}

void EUnitsFilter::update(Unit* u)
{
    serr << "plop, j update" << sendl;
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    serr << "eViedUnits.size() : " << eViewedUnits->size() << sendl;
    if (eViewedUnits->count(u))
        (*eViewedUnits)[u].update(timeManager->getElapsedTime());
    else 
        (*eViewedUnits)[u] = EViewedUnit(u, timeManager->getElapsedTime());
        //eViewedUnits.insert(make_pair(u, EViewedUnit(u, timeManager->getElapsedTime())));
#ifdef BW_QT_DEBUG
    // callback to Qt to modify EUFilter tab's QTableWidget with current values
#endif
    serr << "!!!!!, j update" << sendl;
}

void EUnitsFilter::onUnitDestroy(Unit* u)
{
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    eViewedUnits->erase(u);
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

#ifdef BW_QT_DEBUG
void EUnitsFilter::outputQt()
{ /*
    QTabWidget* qtw = qmainwindow->ui->tabWidget

QWidget* WidgetCreator<std::map<Key, Value> >::create(
        Data<std::map<Key, Value> >* data, QWidget* parent) {
    QWidget* widget = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout();
    QTableView* table = new QTableView(widget);
    layout->addWidget(table);
    QStandardItemModel* mapModel = new QStandardItemModel((*data)->size(), 2);
    QStandardItem *parentItem = mapModel->invisibleRootItem();
    for (std::map<Key, Value>::const_iterator it = (*data)->begin(); it != (*data)->end(); ++it)
    {
        std::ostringstream stream1, stream2;
        stream1 << it->first;
        stream2 << it->second;
        QString tmp1(stream1.str().c_str());
        QString tmp2(stream2.str().c_str());
        QStandardItem* item1 = new QStandardItem(tmp1);
        QStandardItem* item2 = new QStandardItem(tmp2);
        QList<QStandardItem*> tmp_list;
        tmp_list.append(item1);
        tmp_list.append(item2);
        QStandardItem* item = new QStandardItem();
        item->appendColumn(tmp_list);
        parentItem->appendRow(item);
    }
    widget;*/
}
#endif