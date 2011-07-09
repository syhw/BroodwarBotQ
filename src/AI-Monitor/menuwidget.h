#ifdef BW_QT_DEBUG
#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
	class MenuWidget;
}

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    MenuWidget(QWidget *parent = 0);
    ~MenuWidget();
    void addTabWidget(QWidget* qw);

public slots:
    void draw();

protected:
    void changeEvent(QEvent *e);

public:
    Ui::MenuWidget *ui;
};

#endif // MENUWIDGET_H
#endif