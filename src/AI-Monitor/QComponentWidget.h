#ifdef BW_QT_DEBUG
#pragma once
#include "Defines.h"
#include "BaseObject.h"
#include <QtGui/QDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>

class QComponentWidget : public QDialog
{
	Q_OBJECT

public:
  QComponentWidget(const BaseObject* obj);
	~QComponentWidget();

	const BaseObject* baseObj;

public slots:
	void refreshWidget();

protected:
	QTabWidget* tabWidget;
	QTextEdit* textEdit_sout;
	QTextEdit* textEdit_serr;
};
#endif