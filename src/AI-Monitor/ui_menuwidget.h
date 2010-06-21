/********************************************************************************
** Form generated from reading UI file 'menuwidget.ui'
**
** Created: Mon 21. Jun 23:33:18 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MENUWIDGET_H
#define UI_MENUWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MenuWidget
{
public:
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QSlider *hSlider_gameSpeed;
    QTabWidget *tabWidget;
    QWidget *tab_Goals;
    QVBoxLayout *verticalLayout;
    QTreeView *goalsTree;
    QWidget *tab_Strat;
    QGridLayout *gridLayout;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QWidget *tab_EUFilter;
    QHBoxLayout *horizontalLayout_2;
    QTableWidget *EVUnits;

    void setupUi(QWidget *MenuWidget)
    {
        if (MenuWidget->objectName().isEmpty())
            MenuWidget->setObjectName(QString::fromUtf8("MenuWidget"));
        MenuWidget->resize(793, 505);
        verticalLayout_2 = new QVBoxLayout(MenuWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(MenuWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        hSlider_gameSpeed = new QSlider(MenuWidget);
        hSlider_gameSpeed->setObjectName(QString::fromUtf8("hSlider_gameSpeed"));
        hSlider_gameSpeed->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(hSlider_gameSpeed);

        tabWidget = new QTabWidget(MenuWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab_Goals = new QWidget();
        tab_Goals->setObjectName(QString::fromUtf8("tab_Goals"));
        verticalLayout = new QVBoxLayout(tab_Goals);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        goalsTree = new QTreeView(tab_Goals);
        goalsTree->setObjectName(QString::fromUtf8("goalsTree"));

        verticalLayout->addWidget(goalsTree);

        tabWidget->addTab(tab_Goals, QString());
        tab_Strat = new QWidget();
        tab_Strat->setObjectName(QString::fromUtf8("tab_Strat"));
        gridLayout = new QGridLayout(tab_Strat);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_2 = new QPushButton(tab_Strat);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        gridLayout->addWidget(pushButton_2, 2, 1, 1, 1);

        pushButton_3 = new QPushButton(tab_Strat);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

        gridLayout->addWidget(pushButton_3, 2, 2, 1, 1);

        pushButton = new QPushButton(tab_Strat);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 1, 1, 1, 1);

        pushButton_4 = new QPushButton(tab_Strat);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));

        gridLayout->addWidget(pushButton_4, 1, 2, 1, 1);

        pushButton_5 = new QPushButton(tab_Strat);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));

        gridLayout->addWidget(pushButton_5, 1, 0, 1, 1);

        pushButton_6 = new QPushButton(tab_Strat);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));

        gridLayout->addWidget(pushButton_6, 2, 0, 1, 1);

        pushButton_7 = new QPushButton(tab_Strat);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));

        gridLayout->addWidget(pushButton_7, 0, 0, 1, 1);

        pushButton_8 = new QPushButton(tab_Strat);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));

        gridLayout->addWidget(pushButton_8, 0, 1, 1, 1);

        pushButton_9 = new QPushButton(tab_Strat);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));

        gridLayout->addWidget(pushButton_9, 0, 2, 1, 1);

        tabWidget->addTab(tab_Strat, QString());
        tab_EUFilter = new QWidget();
        tab_EUFilter->setObjectName(QString::fromUtf8("tab_EUFilter"));
        horizontalLayout_2 = new QHBoxLayout(tab_EUFilter);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        EVUnits = new QTableWidget(tab_EUFilter);
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
        if (EVUnits->rowCount() < 3)
            EVUnits->setRowCount(3);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        EVUnits->setVerticalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        EVUnits->setVerticalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        EVUnits->setVerticalHeaderItem(2, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        EVUnits->setItem(0, 0, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        EVUnits->setItem(1, 1, __qtablewidgetitem9);
        EVUnits->setObjectName(QString::fromUtf8("EVUnits"));
        EVUnits->setMinimumSize(QSize(381, 0));

        horizontalLayout_2->addWidget(EVUnits);

        tabWidget->addTab(tab_EUFilter, QString());

        verticalLayout_2->addWidget(tabWidget);


        retranslateUi(MenuWidget);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MenuWidget);
    } // setupUi

    void retranslateUi(QWidget *MenuWidget)
    {
        MenuWidget->setWindowTitle(QApplication::translate("MenuWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MenuWidget", "Game speed:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_Goals), QApplication::translate("MenuWidget", "Composants", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        pushButton_9->setText(QApplication::translate("MenuWidget", "PushButton", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_Strat), QApplication::translate("MenuWidget", "Visu", 0, QApplication::UnicodeUTF8));
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
        QTableWidgetItem *___qtablewidgetitem5 = EVUnits->verticalHeaderItem(0);
        ___qtablewidgetitem5->setText(QApplication::translate("MenuWidget", "0", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = EVUnits->verticalHeaderItem(1);
        ___qtablewidgetitem6->setText(QApplication::translate("MenuWidget", "1", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = EVUnits->verticalHeaderItem(2);
        ___qtablewidgetitem7->setText(QApplication::translate("MenuWidget", "2", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = EVUnits->isSortingEnabled();
        EVUnits->setSortingEnabled(false);
        EVUnits->setSortingEnabled(__sortingEnabled);

        tabWidget->setTabText(tabWidget->indexOf(tab_EUFilter), QApplication::translate("MenuWidget", "EUFilter", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MenuWidget: public Ui_MenuWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENUWIDGET_H
