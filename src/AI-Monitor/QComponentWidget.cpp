#include "QComponentWidget.h"
#include "Data.inl"

QComponentWidget::QComponentWidget(const BaseObject* obj)
: QDialog(0)
, baseObj(obj)
{
#ifdef BW_QT_DEBUG
    setWindowModality(Qt::NonModal);
    setWindowTitle (QString (baseObj->getClassName().c_str()));
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    tabWidget = new QTabWidget();
    mainLayout->addWidget(tabWidget);

    // Display the component output.
    tabWidget->addTab(baseObj->createWidget(tabWidget), QString("Option"));

		// Display the Data<>
    const std::vector<BaseData*>& data = baseObj->getData();
    for (unsigned int i = 0; i < (data.size()+4)/5; ++i)
    {
        // create a tab
        QWidget* tab = new QWidget( tabWidget);
        QVBoxLayout* layout = new QVBoxLayout( tab);
        tab->setLayout( layout);
        for (unsigned int j = 0; j < 5; ++j)
        {
            unsigned int dataIndex = i*5+j;
            if (dataIndex >= data.size()) continue;
            layout->addWidget(data[dataIndex]->createWidget(tab));
        }
        tabWidget->addTab(tab, QString("Data"));
    }

    // Create warnings text area
    textEdit_sout = new QTextEdit(tabWidget);
    textEdit_sout->setText(QString(baseObj->getWarnings().c_str()));
    tabWidget->addTab(textEdit_sout, QString( "Warnings"));

    // Create errors text area
    textEdit_serr = new QTextEdit(tabWidget);
    textEdit_serr->setText(QString(baseObj->getErrors().c_str()));
    tabWidget->addTab( textEdit_serr, QString( "Errors"));

    connect(reinterpret_cast<const QObject*>(baseObj->refreshWidgetEmiter), SIGNAL(refreshWidget()), this, SLOT(refreshWidget()), Qt::QueuedConnection);
#endif
}

QComponentWidget::~QComponentWidget()
{

}

void QComponentWidget::refreshWidget()
{
#ifdef BW_QT_DEBUG
    baseObj->refreshWidget(tabWidget->widget (0));
    textEdit_sout->sestText(QString(baseObj->getWarnings().c_str()));
    textEdit_serr->setText(QString(baseObj->getErrors().c_str()));
#endif
}
