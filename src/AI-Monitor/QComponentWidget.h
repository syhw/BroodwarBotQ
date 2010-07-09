#pragma once
#include "Defines.h"
#include "BaseObject.h"
#include <QtGui/QWidget>

class QComponentWidget : public QWidget
{
	Q_OBJECT

public:
  QComponentWidget();
	~QComponentWidget();


public slots:
	void refreshWidget(BaseObject* obj);
};
