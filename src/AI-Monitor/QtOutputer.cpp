#include "QtOutputer.h"
#include "Defines.h"
#include <BWAPI.h>

#ifdef BW_QT_DEBUG

using namespace std;

QtOutputer::QtOutputer(/*QWidget* parent*/)
: qt_initialized(false)
, QObject(/*parent*/)
{ 
}

bool QtOutputer::init()
{
    if (!qmainwindow) 
    {
#ifndef CREATOR_COMPIL
        BWAPI::Broodwar->printf("qmainwindow null");
#endif
        return false;
    }
    addTabToWidget(_qwidget);
    return true;
}

bool QtOutputer::init(QWidget* qw)
{
    // connect to main window
    QObject::connect(this, SIGNAL(addTabToWidget(QWidget*)), qmainwindow, SLOT(addTabWidget(QWidget*)), Qt::QueuedConnection);

    // save the pointer to the QWidget
    _qwidget = qw;

    if (!qmainwindow) 
    {
#ifndef CREATOR_COMPIL
        BWAPI::Broodwar->printf("qmainwindow null");
#endif
        return false;
    }
    // add tab by signaling the qwidget to main window
    emit addTabToWidget(qw);

    return true;
}

void QtOutputer::output(QWidget* qw)
{
    if (!qt_initialized)
        qt_initialized = init();
    if (!qt_initialized)
    {
#ifndef CREATOR_COMPIL
        BWAPI::Broodwar->printf("qmainwindow null");
#endif
        return;
    }
//#ifndef CREATOR_COMPIL
//    BWAPI::Broodwar->printf("qt initialized");
//#endif
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