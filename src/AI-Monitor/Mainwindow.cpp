#ifdef BW_QT_DEBUG
#include "Mainwindow.h"
#include "ui_Mainwindow.h"
#include "ui_menuwidget.h"
#include <BWAPI.h>
#include <QtGui/QDialogButtonBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    objManager = &ObjectManager::Instance();
    ui->setupUi(this);

    menuWidget = new MenuWidget (this);
    miniMap = new MiniMap (this);

    // Add splitter
    splitter = new QSplitter(this);
    splitter->addWidget(menuWidget);
    splitter->addWidget(miniMap);
    ui->horizontalLayout_main->addWidget (splitter);

    //connect slider
    connect(menuWidget->ui->hSlider_gameSpeed, SIGNAL(valueChanged(int)), this, SLOT(changeGameSpeed(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTabWidget(QWidget* qw)
{
	// Check if the signal is well received
	QMessageBox::critical( qw, "plop", "signal recu");

	menuWidget->ui->tabWidget->addTab(qw, QString("plop"));
	// OR 
	// menuWidget->addTabWidget(qw);
}

void MainWindow::initComponentsTree()
{
    // Generate component tree
    QStandardItemModel* componentModel = new QStandardItemModel();
    QStandardItem *parentItem = componentModel->invisibleRootItem();
    const std::vector<BaseObject*>& objects = objManager->getObjects();
    for (std::vector<BaseObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
        QStandardItem *item = new QStandardItem(QString((*it)->getClassName().c_str()));
        item->setEditable(false);
        parentItem->appendRow(item);
    }
    menuWidget->ui->componentTree->setModel( componentModel);
    connect(menuWidget->ui->componentTree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(componentDoubleClicked(QModelIndex)));
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

void MainWindow::changeGameSpeed(int value)
{
    objManager->setGameSpeed(value);
}

void MainWindow::componentDoubleClicked(QModelIndex index)
{
    objManager = &ObjectManager::Instance();

    const BaseObject* baseObj = objManager->getObjects()[index.row()];
    QComponentWidget* component = new QComponentWidget(baseObj);
    component->exec();
}
#endif