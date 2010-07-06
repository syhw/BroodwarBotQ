/********************************************************************************
** Form generated from reading UI file 'menuwidget.ui'
**
** Created: Tue 6. Jul 17:20:43 2010
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
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
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
    QWidget *tab_Goals;
    QVBoxLayout *verticalLayout;
    QTreeView *componentTree;
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
        componentTree = new QTreeView(tab_Goals);
        componentTree->setObjectName(QString::fromUtf8("componentTree"));

        verticalLayout->addWidget(componentTree);

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

        verticalLayout_2->addWidget(tabWidget);


        retranslateUi(MenuWidget);

        tabWidget->setCurrentIndex(1);


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
    } // retranslateUi

};

namespace Ui {
    class MenuWidget: public Ui_MenuWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENUWIDGET_H
