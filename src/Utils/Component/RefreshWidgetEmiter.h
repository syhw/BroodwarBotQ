#ifdef BW_QT_DEBUG
#pragma once
#include "../../Defines.h" // Hack for moc
#include <QtGui/QWidget>
#include <QtGui/QMessageBox>


//Useless class to avoid pb with BW_QT_DEBUG flag
class RefreshWidgetEmiter: public QObject 
{
	Q_OBJECT

public:
	RefreshWidgetEmiter(){}
	~RefreshWidgetEmiter(){}

	void refresh() { emit refreshWidget();}

signals:
	void refreshWidget();
};
#endif