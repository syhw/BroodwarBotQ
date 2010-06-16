/********************************************************************************
** Form generated from reading UI file 'menuwidget.ui'
**
** Created: Fri 28. May 19:21:05 2010
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
    QWidget *tab_EUnitsFilter;

    void setupUi(QWidget *MenuWidget)
    {
        if (MenuWidget->objectName().isEmpty())
            MenuWidget->setObjectName(QString::fromUtf8("MenuWidget"));
        MenuWidget->resize(400, 300);
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
        tab_EUnitsFilter = new QWidget();
        tab_EUnitsFilter->setObjectName(QString::fromUtf8("tab_EUnitsFilter"));
        tabWidget->addTab(tab_EUnitsFilter, QString());

        verticalLayout_2->addWidget(tabWidget);


        retranslateUi(MenuWidget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MenuWidget);
    } // setupUi

    void retranslateUi(QWidget *MenuWidget)
    {
        MenuWidget->setWindowTitle(QApplication::translate("MenuWidget", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MenuWidget", "Game speed:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MenuWidget", "Composants", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MenuWidget", "Visu", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_EUnitsFilter), QApplication::translate("MenuWidget", "EUFilter", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MenuWidget: public Ui_MenuWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENUWIDGET_H
