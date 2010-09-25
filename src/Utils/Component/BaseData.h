#pragma once
#ifdef BW_QT_DEBUG
#include <QtGui/QWidget>
#endif

class BaseData
{
public:
	BaseData();
    ~BaseData();
#ifdef BW_QT_DEBUG
    virtual QWidget* createWidget( QWidget* parent) = 0;
#endif
};
