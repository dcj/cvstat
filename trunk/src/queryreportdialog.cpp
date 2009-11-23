#include "queryreportdialog.h"
#include "ui_queryreportdialog.h"
#include <QDate>
#include <QMessageBox>
#include <QFile>
#include <QXmlStreamWriter>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

#if defined(Q_OS_WIN32)
#include <Windows.h>
#endif

QueryReportDialog::QueryReportDialog(const QString & cvsroot, CVStat & stat, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QueryReportDialog),
	m_cvsroot(cvsroot),
	m_stat(stat)
{
    ui->setupUi(this);
	QDate date = QDate::currentDate();
	ui->toDate->setDate(date);
	ui->fromDate->setDate(date.addDays(1 - date.dayOfWeek()));

	connect(ui->dateTodayButton, SIGNAL(clicked()), this, SLOT(todayClicked()));
	connect(ui->dateWeekButton, SIGNAL(clicked()), this, SLOT(thisWeekClicked()));
	connect(ui->dateMonthButton, SIGNAL(clicked()), this, SLOT(thisMonthClicked()));
	connect(ui->browseSaveToButton, SIGNAL(clicked()), this, SLOT(browseSaveToClicked()));

}

QueryReportDialog::~QueryReportDialog()
{
    delete ui;
}

void QueryReportDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void QueryReportDialog::todayClicked()
{
	QDate date = QDate::currentDate();
	ui->toDate->setDate(date);
	ui->fromDate->setDate(date);
}
void QueryReportDialog::thisWeekClicked()
{
	QDate date = QDate::currentDate();
	ui->toDate->setDate(date);
	ui->fromDate->setDate(date.addDays(1 - date.dayOfWeek()));
}
void QueryReportDialog::thisMonthClicked()
{
	QDate date = QDate::currentDate();
	ui->toDate->setDate(date);
	ui->fromDate->setDate(date.addDays(1 - date.day()));
}
void QueryReportDialog::browseSaveToClicked()
{
	QFileDialog dlg(this, tr("Select Report File"));
	dlg.setAcceptMode(QFileDialog::AcceptSave);
	if (dlg.exec())
		ui->saveToEdit->setText(dlg.selectedFiles().at(0));
}

void QueryReportDialog::accept()
{

	if (ui->saveToEdit->text().isEmpty())
	{
		QMessageBox::information(this, tr("tip"), tr("a report file should be selected"));
		ui->saveToEdit->setFocus();
		return;
	}
	QString fname = ui->saveToEdit->text();
	QFile file(fname);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QMessageBox::information(this, tr("tip"), tr("the report isn't writable, select atheor file instead"));
		ui->saveToEdit->setFocus();
		return;
	}

	QString dateFormat = "yyyy/MM/dd";
	QString fromDate = ui->fromDate->date().toString(dateFormat);
	QString toDate = ui->toDate->date().addDays(1).toString(dateFormat);
	QString dateWhere = QString("WHERE rdate>\"%1\" and rdate < \"%2\"").arg(fromDate).arg(toDate);

	QXmlStreamWriter writer(&file);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();


	writer.writeStartElement("cvsreport");

	writeReport(writer, "totalAdd", QString("select SUM(radd) from data %1").arg(dateWhere));
	writeReport(writer, "totalDel", QString("select SUM(rdel) from data %1").arg(dateWhere));
	writeReport(writer, "totalChanged", QString("select SUM(radd) + SUM(rdel) from data %1").arg(dateWhere));

	QStringList attrs;
	attrs << "name" << "add" << "del";
	writeReport(writer, "user_stat", "user", attrs, QString("select rauthor, SUM(radd), SUM(rdel) from data %1 group by rauthor").arg(dateWhere));

	QStringList users;
	QSqlQuery query = m_stat.getQuery(m_cvsroot, QString("select rauthor from data %1 group by rauthor").arg(dateWhere));
	if (query.exec())
	{
		while(query.next())
			users.append(query.record().value(0).toString());
	}
	attrs.clear();
	attrs << "user" << "name" << "from" << "to";
	for(int i = 0; i < users.length(); i++)
	{
		writeReport(writer, "user_detail", "detail",attrs, QString(
			"select rauthor, rname, MIN(rrevision), MAX(rrevision) from data %1 and rauthor='%2' group by rname"
			).arg(dateWhere).arg(users.at(i)));

	}



	writer.writeEndElement();
	writer.writeEndDocument();;

#if defined(Q_OS_WIN32)
	QT_WA({
			ShellExecute(0, 0, (TCHAR *)fname.utf16(), 0, 0, SW_SHOWNORMAL);
			} , {
			ShellExecuteA(0, 0, fname.toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL);
			});
#endif
}

void QueryReportDialog::writeReport(QXmlStreamWriter & writer,	const QString & nodeName,
				 const QString & subNodeName, const QStringList & attrsName, const QString & sql)
{
	QSqlQuery query = m_stat.getQuery(m_cvsroot, sql);
	writer.writeStartElement(nodeName);
	if (query.exec())
	{

		while(query.next())
		{
			writer.writeStartElement(subNodeName);
			for(int i = 0; i < attrsName.length(); i++)
				writer.writeAttribute(attrsName.at(i), query.record().value(i).toString());
			writer.writeEndElement();

		}
	}
	writer.writeEndElement();
}

void QueryReportDialog::writeReport(QXmlStreamWriter & writer,	const QString & nodeName, const QString & sql)
{
	QSqlQuery query = m_stat.getQuery(m_cvsroot, sql);
	if (query.exec() && query.next())
		writer.writeTextElement(nodeName, query.record().value(0).toString());
}
