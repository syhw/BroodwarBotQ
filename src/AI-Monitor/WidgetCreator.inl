#include "WidgetCreator.h"
#include <QtGui/QTableWidget>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>
#include <QtCore/QTextStream>

template <typename T>
QWidget* WidgetCreator<T>::create(Data<T>* data, QWidget* parent) {
    QWidget* widget = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget (new QLabel (QString(data->getName().c_str()), parent));
    layout->addWidget (new QLineEdit (QString::number(**data), parent));
    return widget;
}

/***
* Specialisation de QWidget pour MAP
*/
template <typename Key, typename Value>
QWidget* WidgetCreator<std::map<Key, Value> >::create(
        Data<std::map<Key, Value> >* data, QWidget* parent) {
    QWidget* widget = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout();
    QTableView* table = new QTableView(widget);
    layout->addWidget(table);
    QStandardItemModel* mapModel = new QStandardItemModel((*data)->size(), 2);
    QStandardItem *parentItem = mapModel->invisibleRootItem();
    for (typename std::map<Key, Value>::const_iterator it = (*data)->begin(); it != (*data)->end(); ++it)
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
    return widget;
}    

