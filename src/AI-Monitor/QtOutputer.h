#pragma once
#include <QtCore/QObject>
#include <QtGui/QWidget>
#include "Defines.h"

class QtOutputer : public QObject
{
    Q_OBJECT

    bool qt_initialized;
    QWidget* _qwidget;

public:
    QtOutputer();
    bool init();
    bool init(QWidget* qw);
    void output(QWidget* qw);
signals:
    void addTabToWidget(QWidget* qw);
};
