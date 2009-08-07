#include "waitupdatedialog.h"
#include "ui_waitupdatedialog.h"
#include <QtCore/QTimer>

WaitUpdateDialog::WaitUpdateDialog(QProcess & process, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::WaitUpdateDialog)
{
	setWindowModality(Qt::ApplicationModal);
    m_ui->setupUi(this);
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)),
			this, SLOT(onLogFinish(int, QProcess::ExitStatus)));
	m_ui->importProgress->setMaximum(100);
	m_timer->start(100);
}

WaitUpdateDialog::~WaitUpdateDialog()
{
    delete m_ui;
}

void WaitUpdateDialog::changeEvent(QEvent *e)
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

int step = 1;
void WaitUpdateDialog::incressProgress()
{
	if (m_ui->importProgress->value() == m_ui->importProgress->maximum())
		step = -1;
	else if (m_ui->importProgress->value() == 0) 
		step = 1;

	m_ui->importProgress->setValue(m_ui->importProgress->value() + step);
}

void WaitUpdateDialog::onTimer()
{
	incressProgress();
}

void WaitUpdateDialog::onLogFinish(int /*exitCode*/,
	   	QProcess::ExitStatus /*exitStatus*/)
{
	close();
}
