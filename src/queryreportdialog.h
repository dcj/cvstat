#ifndef QUERYREPORTDIALOG_H
#define QUERYREPORTDIALOG_H

#include <QDialog>
#include <QXmlStreamWriter>
#include "cvstat.h"

namespace Ui {
    class QueryReportDialog;
}

class QueryReportDialog : public QDialog {
    Q_OBJECT
public:
	QueryReportDialog(const QString & cvsroot, CVStat & stat, QWidget *parent = 0);
    ~QueryReportDialog();

public slots:
	void todayClicked();
	void thisWeekClicked();
	void thisMonthClicked();
	void browseSaveToClicked();

protected:
    void changeEvent(QEvent *e);
	void accept();

	void writeReport(QXmlStreamWriter & writer,	const QString & nodeName,
					 const QString & subNodeName, const QStringList & attrsName, const QString & sql);
	void writeReport(QXmlStreamWriter & writer,	const QString & nodeName, const QString & sql);

private:
    Ui::QueryReportDialog *ui;
	CVStat & m_stat;
	QString m_cvsroot;
};

#endif // QUERYREPORTDIALOG_H
