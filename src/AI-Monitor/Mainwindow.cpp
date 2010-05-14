#include "Mainwindow.h"
#include "ui_Mainwindow.h"

MainWindow::MainWindow(QWidget *parent, ObjectManager* objManager, bool* gameIsFinished) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    objManager(objManager),
    gameIsFinished(gameIsFinished)
{
    ui->setupUi(this);

    MenuWidget* menuWidget = new MenuWidget (this);
    MiniMap* miniMap = new MiniMap (this);

    splitter = new QSplitter (this);
    splitter->addWidget (menuWidget);
    splitter->addWidget (miniMap);
    ui->horizontalLayout_main->addWidget (splitter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
