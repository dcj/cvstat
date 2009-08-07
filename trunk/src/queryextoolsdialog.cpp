#include "queryextoolsdialog.h"
#include "ui_queryextoolsdialog.h"
#include <QtGui/QFileDialog>

QueryExToolsDialog::QueryExToolsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::QueryExToolsDialog)
{
    m_ui->setupUi(this);
	connect(m_ui->btnBrows, SIGNAL(clicked()), this, SLOT(OnBrowsClicked()));
}

QueryExToolsDialog::~QueryExToolsDialog()
{
    delete m_ui;
}

void QueryExToolsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void QueryExToolsDialog::OnBrowsClicked()
{
	QFileDialog dlg;
	if (dlg.exec())
	{
		QString path = dlg.selectedFiles().at(0);
		m_ui->edTools->setText(path);
	}
}

void QueryExToolsDialog::accept()
{
	m_tools.clear();
	m_tools.append(m_ui->edTools->text());
	m_tools.append(m_ui->edParam->text());
	QDialog::accept();
}

QStringList QueryExToolsDialog::getTools() const
{
	return m_tools;
}
