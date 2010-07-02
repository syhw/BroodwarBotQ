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
    if (u->getPlayer() == Broodwar->self()) return;
    if (u->getPlayer()->isNeutral()) return;
    if (eViewedUnits->count(u))
        (*eViewedUnits)[u].update(timeManager->getElapsedTime());
    else 
        (*eViewedUnits)[u] = EViewedUnit(u, timeManager->getElapsedTime());
        //eViewedUnits.insert(make_pair(u, EViewedUnit(u, timeManager->getElapsedTime())));
#ifdef BW_QT_DEBUG
    outputQt();
#endif
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
bool init_qt()
{
    if (!qmainwindow) 
    {
        Broodwar->printf("qmainwindow null");
        return false;
    }
    // create and define your widget
    QWidget* qw = new QWidget();
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
    
    // attach it into MainWindow
    qmainwindow->addTabWidget(qw);
    return true;
}

void EUnitsFilter::outputQt()
{ 
    if (!qt_initialized)
        qt_initialized = init_qt();
    if (!qt_initialized)
        return;

    /*for 

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