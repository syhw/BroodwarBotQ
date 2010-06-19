/********************************************************************************
** Form generated from reading UI file 'menuwidget.ui'
**
** Created: Sat 19. Jun 13:38:53 2010
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
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
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
    QWidget *tab;
    QVBoxLayout *verticalLayout;
    QTreeView *treeView;
    QWidget *tab_2;
    QWidget *tab_EUFilter;
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
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout = new QVBoxLayout(tab);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        treeView = new QTreeView(tab);
        treeView->setObjectName(QString::fromUtf8("treeView"));

        verticalLayout->addWidget(treeView);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());
        tab_EUFilter = new QWidget();
        tab_EUFilter->setObjectName(QString::fromUtf8("tab_EUFilter"));
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
        if (EVUnits->rowCount() < 5)
            EVUnits->setRowCount(5);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        EVUnits->setVerticalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        EVUnits->setVerticalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        EVUnits->setVerticalHeaderItem(2, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        EVUnits->setVerticalHeaderItem(3, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        EVUnits->setVerticalHeaderItem(4, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        EVUnits->setItem(0, 0, __qtablewidgetitem10);
        EVUnits->setObjectName(QString::fromUtf8("EVUnits"));
        EVUnits->setGeometry(QRect(0, 0, 771, 411));
        EVUnits->setMinimumSize(QSize(381, 0));
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
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MenuWidget", "Composants", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MenuWidget", "Visu", 0, QApplication::UnicodeUTF8));
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
        ___qtablewidgetitem6->setText(QApplication::translate("MenuWidget", "New Row", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = EVUnits->verticalHeaderItem(2);
        ___qtablewidgetitem7->setText(QApplication::translate("MenuWidget", "New Row", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = EVUnits->verticalHeaderItem(3);
        ___qtablewidgetitem8->setText(QApplication::translate("MenuWidget", "New Row", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = EVUnits->verticalHeaderItem(4);
        ___qtablewidgetitem9->setText(QApplication::translate("MenuWidget", "New Row", 0, QApplication::UnicodeUTF8));

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
