#ifndef CVSTAT_H
#define CVSTAT_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>

class CVStat : public QObject
{
	Q_OBJECT

public:
	CVStat(QObject *parent);
	~CVStat();

	bool open(const QString & path = "./statcvs.db3");
	QStringList getAllStat() const;
	QStringList getAllCVSRoot() const;

	
	QSqlQuery getQuery(const QString & cvsroot, const QString & sql);

	QDateTime getLastUpdate(const QString & cvsroot) const;
	void setLastUpdate(const QString & cvsroot, QDateTime dt);
	int getId(const QString & cvsroot) const;

	void addCVSEntry(const QString & cvsroot) const;

	int doStat(const QString & cvsroot);
	void doRemove(const QString & cvsroot);

	void importLog(const QString & cvsroot, const QString & logfile);
protected:
	QString makeTableName(const QString & cvsroot) const;
private:
private:
	QSqlDatabase m_db;
};

#endif // CVSTAT_H
