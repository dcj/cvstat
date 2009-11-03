#include "mainframe.h"
#include "newwizard.h"
#include <QMessageBox>
#include <QtGui/QSplitter>
#include <QtGui/QListWidget>
#include <QtGui/QTableView>
#include <QtGui/QTextEdit>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlQuery>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QtGui/QFileDialog>
#include <QtCore/QTextStream>
#include "queryextoolsdialog.h"

MainFrame::MainFrame(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), m_stat(NULL)
{
	ui.setupUi(this);

	hsp = new QSplitter(Qt::Horizontal, this);
	vsp = new QSplitter(Qt::Vertical, hsp);
	wCvsRoots = new QListWidget(hsp);
	wResultGird = new QTableView(vsp);
	wCmdEdit = new QTextEdit(vsp);

	vsp->addWidget(wCvsRoots);
	vsp->addWidget(wCmdEdit);
	hsp->addWidget(wCvsRoots);
	hsp->addWidget(vsp);

	setCentralWidget(hsp);

	
	connect(ui.action_New_Stat, SIGNAL(triggered(bool)), this, SLOT(OnNewStat(bool)));
	connect(ui.action_Query, SIGNAL(triggered(bool)), this, SLOT(OnQuery(bool)));
	connect(ui.action_Update_Stat, SIGNAL(triggered(bool)), this, SLOT(OnUpdateStat(bool)));
	connect(ui.action_Remove_Stat, SIGNAL(triggered(bool)), this, SLOT(OnRemoveStat(bool)));
	connect(ui.action_View, SIGNAL(triggered(bool)), this, SLOT(OnViewFile(bool)));
	connect(ui.action_Export, SIGNAL(triggered(bool)), this, SLOT(OnToolsExport(bool)));

	connect(wCvsRoots, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(OnCvsRootClicked(QListWidgetItem*)));
	
	m_stat.open();

	wCvsRoots->addItems(m_stat.getAllCVSRoot());
	wResultGird->setModel(new QSqlQueryModel());


	QSettings settings("config.ini", QSettings::IniFormat);
	hsp->restoreState(settings.value("layout/hsp").toByteArray());
	vsp->restoreState(settings.value("layout/vsp").toByteArray());
	resize(settings.value("layout/window").toSize());
	wCmdEdit->setText(settings.value("runtime/lastcmd").toString());
	m_viewTools = settings.value("config/viewtools").toStringList();
	m_viewDiffTools = settings.value("config/difftools").toStringList();
}

MainFrame::~MainFrame()
{
	QSettings settings("config.ini", QSettings::IniFormat);
	settings.setValue("layout/hsp", hsp->saveState());
	settings.setValue("layout/vsp", vsp->saveState());
	settings.setValue("layout/window", size());
	settings.setValue("runtime/lastcmd", wCmdEdit->toPlainText());
	settings.setValue("config/viewtools", m_viewTools);
	settings.setValue("config/difftools", m_viewDiffTools);
}

void MainFrame::OnNewStat(bool /*checked*/)
{

	NewWizard dlg(this);
	dlg.setCVSRootList(m_stat.getAllCVSRoot());
	int n = dlg.exec();
	if (n)
	{
		m_stat.addCVSEntry(dlg.getCVSRoot());
		m_stat.doStat(dlg.getCVSRoot());
		wCvsRoots->addItem(dlg.getCVSRoot());
	}
}



QString MainFrame::getCurrentCvsRoot() const
{
	QString cvsroot;
	QList<QListWidgetItem*> items = wCvsRoots->selectedItems();
	if (!items.empty())
	{
		QListWidgetItem * item = items.at(0);
		if (item != NULL)
			cvsroot = item->text();
	}
	return cvsroot;
}

bool MainFrame::checkCurrentCvsRoot(QString & cvsroot) const
{
	cvsroot = getCurrentCvsRoot();
	if (cvsroot.isEmpty())
	{
		QMessageBox::information(NULL,
			tr("info"), tr("a cvsroot must be selected"));
		return false;
	}
	return true;
}

void MainFrame::OnQuery( bool /*checked*/)
{
	QString cvsroot;
	if (checkCurrentCvsRoot(cvsroot))
	{
		QString sql = wCmdEdit->toPlainText();
		QSqlQueryModel * model = dynamic_cast<QSqlQueryModel*>(wResultGird->model());
		if (model)
		{
			model->setQuery(m_stat.getQuery(cvsroot, sql));
		}
	}
}


void MainFrame::OnUpdateStat(bool /*checked*/)
{
	QString cvsroot;
	if (checkCurrentCvsRoot(cvsroot))
	{
		m_stat.doStat(cvsroot);
	}
}

void MainFrame::OnRemoveStat(bool /*checked*/)
{
	QString cvsroot;
	if (checkCurrentCvsRoot(cvsroot))
	{
		m_stat.doRemove(cvsroot);
		wCvsRoots->clear();
		wCvsRoots->addItems(m_stat.getAllCVSRoot());
	}
}

void MainFrame::OnViewFile(bool /*checked*/)
{
	if (m_viewTools.empty())
	{
		QueryExToolsDialog dlg(this);
		if (dlg.exec())
			m_viewTools = dlg.getTools();
		else
			return;
	}

	QString path = getCurrentCvsRoot() + "/" +
				   wResultGird->currentIndex().data().toString().trimmed();

	QString program = m_viewTools.at(0);
	QString param = QString(m_viewTools.at(1)).arg(path);

	QProcess *proc = new QProcess(this);
	QStringList params;
	params.append(param);
	proc->start(program, params);
}

void MainFrame::OnViewDiff(bool /*checked*/)
{
}

void MainFrame::OnToolsExport(bool /*checked*/)
{
	QSqlQueryModel * model = dynamic_cast<QSqlQueryModel*>(wResultGird->model());
	if (!model)
		return;

	QFileDialog dlg(this, tr("Select Export File"));
	dlg.setAcceptMode(QFileDialog::AcceptSave);
	if (dlg.exec())
	{
		QString fname = dlg.selectedFiles().at(0);
		QFile file(fname);
		if (file.open(QFile::WriteOnly | QFile::Truncate))
		{
			QTextStream fout(&file);
			for(int row = 0; row < model->rowCount(); row++)
			{
				for(int col = 0; col < model->columnCount(); col++)
				{
					QVariant field = model->index(row, col).data();
					if (col == 0)
						fout << field.toString();
					else
						fout << ',' << field.toString();
				}
			}
		}
	}
}

void MainFrame::OnCvsRootClicked(QListWidgetItem* item)
{
	wCvsRoots->setToolTip(item->text());
	QString title= QString(tr("CvsStat--[%1]")).arg(item->text());
	setWindowTitle(title);
}

int MainFrame::createStat( const QString & /*cvsRoot*/, const QString & /*cvsPath*/)
{
	return 0;
}

void MainFrame::contextMenuEvent ( QContextMenuEvent * /*event*/ )
{
	if (wCvsRoots->hasFocus())
	{
		QMenu m(this);
		m.insertAction(NULL, this->ui.action_New_Stat);
		m.insertAction(NULL, this->ui.action_Update_Stat);
		m.insertAction(NULL, this->ui.action_Remove_Stat);
		m.exec(QCursor::pos());
	}
	if (wResultGird->hasFocus())
	{
		wResultGird->currentIndex();
	}
}
