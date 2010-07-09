#pragma once
#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include "MiniMap.h"
#include "MenuWidget.h"
#include "ObjectManager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

public slots:
    void changeGameSpeed(int value);
    void componentDoubleClicked(QModelIndex index);
    void addTabWidget(QWidget* qw);
		void initComponentsTree();

public:
    Ui::MainWindow *ui;
    QSplitter* splitter;
    MenuWidget* menuWidget;
    MiniMap* miniMap;
    ObjectManager* objManager;
};
