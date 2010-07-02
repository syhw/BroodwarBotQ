#include "Mainwindow.h"
#include "ui_Mainwindow.h"
#include "ui_menuwidget.h"
#include <BWAPI.h>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDialogButtonBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    objManager = &ObjectManager::Instance();
    ui->setupUi(this);

    MenuWidget* menuWidget = new MenuWidget (this);
    MiniMap* miniMap = new MiniMap (this);

    // Add splitter
    splitter = new QSplitter(this);
    splitter->addWidget(menuWidget);
    splitter->addWidget(miniMap);
    ui->horizontalLayout_main->addWidget (splitter);

    //connect slider
    connect(menuWidget->ui->hSlider_gameSpeed, SIGNAL(valueChanged(int)), this, SLOT(changeGameSpeed(int)));

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

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTabWidget(QWidget* qw)
{
    menuWidget->addTabWidget(qw);
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
    QDialog* window = new QDialog(0);
    window->setWindowTitle (QString (baseObj->getClassName().c_str()));
    QVBoxLayout* mainLayout = new QVBoxLayout(window);
    window->setLayout(mainLayout);
    QTabWidget* tabWidget = new QTabWidget();
    mainLayout->addWidget(tabWidget);

    const std::vector<BaseData*>& data = baseObj->getData();
    for (unsigned int i = 0; i < data.size()/5+1; ++i)
    {
        // create a tab
        QWidget* tab = new QWidget( tabWidget);
        QVBoxLayout* layout = new QVBoxLayout( tab);
        tab->setLayout( layout);
        for (unsigned int j = 0; j < 5; ++j)
        {
            unsigned int dataIndex = i*5+j;
            if (dataIndex >= data.size()) continue;
            layout->addWidget(data[dataIndex]->createWidget(tab));
        }
        tabWidget->addTab(tab, QString("Data"));
    }

    // Create warnings text area
    QTextEdit* textEdit_sout = new QTextEdit(tabWidget);
    textEdit_sout->setText(QString(baseObj->getWarnings().c_str()));
    tabWidget->addTab(textEdit_sout, QString( "Warnings"));

    // Create errors text area
    QTextEdit* textEdit_serr = new QTextEdit(tabWidget);
    textEdit_serr->setText(QString(baseObj->getErrors().c_str()));
    tabWidget->addTab( textEdit_serr, QString( "Errors"));
    
    window->exec();
}
