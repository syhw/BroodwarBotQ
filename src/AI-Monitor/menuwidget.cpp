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
    BWAPI::Broodwar->printf("menuwidget initialised"); // doesnt work
}

MenuWidget::~MenuWidget()
{
    BWAPI::Broodwar->printf("menuwidget killed"); // doesnt work
    delete ui;
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