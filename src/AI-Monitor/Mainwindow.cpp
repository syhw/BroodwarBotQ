#include "Mainwindow.h"
#include "ui_Mainwindow.h"
#include "ui_menuwidget.h"

MainWindow::MainWindow(QWidget *parent, ObjectManager* objManager) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    objManager(objManager)
{
    ui->setupUi(this);

    MenuWidget* menuWidget = new MenuWidget (this);
    MiniMap* miniMap = new MiniMap (this);

    splitter = new QSplitter (this);
    splitter->addWidget (menuWidget);
    splitter->addWidget (miniMap);
    ui->horizontalLayout_main->addWidget (splitter);

    connect( menuWidget->ui->hSlider_gameSpeed, SIGNAL(valueChanged(int)), this, SLOT(changeGameSpeed(int)));
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

void MainWindow::changeGameSpeed( int value)
{
    objManager->setGameSpeed( value);
}
