#ifdef BW_QT_DEBUG
#pragma once
#include <QtGui/QWidget>
#include "data.h"
#include <map>

template <typename T>
class WidgetCreator
{
    public:
        static QWidget* create(Data<T>* data, QWidget* parent);
};

template <typename Key, typename Value>
class WidgetCreator<std::map<Key, Value> > 
{
    public:
        static QWidget* create(Data<std::map<Key, Value> >* data, QWidget* parent);
};
#endif