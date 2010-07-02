#include "menuwidget.h"
#include "ui_menuwidget.h"
#include <BWAPI.h>

MenuWidget::MenuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    //QTableWidgetItem *newItem = new QTableWidgetItem(0);
    //QString str = "lol";
    //QTableWidgetItem *newItem2 = new QTableWidgetItem(str, 0);
    //ui->EVUnits->setItem(1, 1, newItem2);
    //ui->EVUnits->setItem(0, 0, newItem);
}

MenuWidget::~MenuWidget()
{
    delete ui;
}

void MenuWidget::addTabWidget(QWidget* qw)
{
#define BW_DO_NOT_RUN
#ifndef BW_DO_NOT_RUN
    this->ui->tabWidget->addTab(qw, QString()); //QString("Data")
#endif
}

void MenuWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MenuWidget::draw()
{
    ui->retranslateUi(this);
}
