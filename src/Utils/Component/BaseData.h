#pragma once
#include <QtGui/QWidget>

class BaseData
{
public:
	BaseData();
    ~BaseData();

    virtual QWidget* createWidget( QWidget* parent) = 0;
};
