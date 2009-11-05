#include "queryreportdialog.h"
#include "ui_queryreportdialog.h"
#include <QDate>
#include <QMessageBox>
#include <QFile>
#include <QXmlStreamWriter>
#include <QSqlQuery>
#include <QSqlRecord>

QueryReportDialog::QueryReportDialog(const QString & cvsroot, CVStat & stat, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QueryReportDialog),
	m_cvsroot(cvsroot),
	m_stat(stat)
{
    ui->setupUi(this);
	QDate date = QDate::currentDate();
	ui->toDate->setDate(date);
	date.addDays(1 - date.dayOfWeek());
	ui->fromDate->setDate(date);
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

void QueryReportDialog::accept()
{
	QString dateFormat = "yyyy/MM/dd";
	QString fromDate = ui->fromDate->date().toString(dateFormat);
	QString toDate = ui->toDate->date().addDays(1).toString(dateFormat);
	QString dateWhere = QString("WHERE rdate>\"%1\" and rdate < \"%2\"").arg(fromDate).arg(toDate);

	QFile file("report.xml");
	QXmlStreamWriter writer(&file);
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);

	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	writer.writeStartElement("cvsreport");

	writeReport(writer, "totalAdd", QString("select SUM(radd) from data %1").arg(dateWhere));
	writeReport(writer, "totalDel", QString("select SUM(rdel) from data %1").arg(dateWhere));
	writeReport(writer, "totalChanged", QString("select SUM(radd) + SUM(rdel) from data %1").arg(dateWhere));

	QStringList attrs;
	attrs << "name" << "add" << "del";
	writeReport(writer, "byusers", "user", attrs, QString("select rauthor, SUM(radd) from data %1").arg(dateWhere));

	writer.writeEndElement();
	writer.writeEndDocument();;


	QMessageBox::information(this, "", dateWhere);
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
