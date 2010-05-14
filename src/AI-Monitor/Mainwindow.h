#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    MainWindow( QWidget *parent = 0, ObjectManager* objManager = 0, bool* gameIsFinished = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QSplitter* splitter;
    MenuWidget* menuWidget;
    MiniMap* miniMap;
    ObjectManager* objManager;
    bool* gameIsFinished;
};

#endif // MAINWINDOW_H
