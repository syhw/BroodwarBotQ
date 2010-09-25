#ifdef BW_QT_DEBUG
#include "menuwidget.h"
#include "ui_menuwidget.h"
#include <BWAPI.h>

#include <QtGui/QWidget>
#include <QtGui/QTableWidget>

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

		/*
    // create and define your widget
    QWidget* qw = new QWidget();
    qw->setObjectName(QString::fromUtf8("tab_EUFilter_test"));
    QHBoxLayout* horizontalLayout = new QHBoxLayout(qw);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout_EUFilter"));
    QTableWidget *EVUnits = new QTableWidget(qw);
    if (EVUnits->columnCount() < 5)
        EVUnits->setColumnCount(5);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(3, __qtablewidgetitem3);
    QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
    EVUnits->setHorizontalHeaderItem(4, __qtablewidgetitem4);
    EVUnits->setObjectName(QString::fromUtf8("EVUnits"));
    QTableWidgetItem *___qtablewidgetitem = EVUnits->horizontalHeaderItem(0);
    ___qtablewidgetitem->setText(QApplication::translate("MenuWidget", "pointer", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem1 = EVUnits->horizontalHeaderItem(1);
    ___qtablewidgetitem1->setText(QApplication::translate("MenuWidget", "unittype", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem2 = EVUnits->horizontalHeaderItem(2);
    ___qtablewidgetitem2->setText(QApplication::translate("MenuWidget", "last_pos", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem3 = EVUnits->horizontalHeaderItem(3);
    ___qtablewidgetitem3->setText(QApplication::translate("MenuWidget", "last_seen", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem4 = EVUnits->horizontalHeaderItem(4);
    ___qtablewidgetitem4->setText(QApplication::translate("MenuWidget", "HP", 0, QApplication::UnicodeUTF8));
    horizontalLayout->addWidget(EVUnits);
    //ui->tabWidget->addTab(qw, QString());
		*/
}

MenuWidget::~MenuWidget()
{
    delete ui;
}

void MenuWidget::addTabWidget(QWidget* qw)
{
//#define BW_DO_NOT_RUN
#ifndef BW_DO_NOT_RUN
    ui->tabWidget->addTab(qw, QString("lol")); //QString("Data")
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
#endif