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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MenuWidget *ui;
};

#endif // MENUWIDGET_H
