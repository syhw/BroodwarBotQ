#pragma once
#include <QtCore/QObject>
#include <QtGui/QWidget>

class QtOutputer : public QObject
{
    Q_OBJECT

    bool qt_initialized;
    QWidget* _qwidget;

public:
    QtOutputer(/*QWidget* parent = NULL*/);
    bool init();
    bool init(QWidget* qw);
    void output(QWidget* qw);
signals:
    void addTabToWidget(QWidget* qw);
};
