#include "cvstat.h"
#include <QtCore/QRegExp>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtCore/QProcess>
#include <QtSql/QSqlError>
#include <QtCore/QtDebug>
#include <QtCore/QCryptographicHash>
#include <QtCore/QBitArray>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QChar>
#include "waitupdatedialog.h"



CVStat::CVStat(QObject *parent)
	: QObject(parent)
{
	m_db = QSqlDatabase::addDatabase("QSQLITE");
}

CVStat::~CVStat()
{

}

bool CVStat::open( const QString & path /*= ":/statcvs.db3"*/ )
{
	if (m_db.isOpen())
		m_db.close();
	m_db.setDatabaseName(path);
	if (m_db.open())
	{
		QSqlQuery query(m_db);
		return query.exec(
			"CREATE TABLE IF NOT EXISTS _config(cid INTEGER PRIMARY KEY, cvsroot TEXT, tname TEXT, lastupdate INTEGER);");
	}
	return false;
}

QStringList CVStat::getAllStat() const
{
	QStringList tables = m_db.tables();
	QRegExp expr("^[^_].+", Qt::CaseSensitive, QRegExp::RegExp2);
	return tables.filter(expr);
}

QStringList CVStat::getAllCVSRoot() const
{
	QSqlQuery query(m_db);
	QStringList cvsroots;
	if (query.exec("SELECT DISTINCT cvsroot FROM _config ORDER BY cvsroot"))
	{
		while(query.next())
			cvsroots.append(query.record().value(0).toString());
	}
	return cvsroots;
}

int CVStat::doStat( const QString & cvsroot)
{
	QProcess cvsproc;
	QStringList params;
	QDateTime dt = getLastUpdate(cvsroot);

	params.append("log");
	params.append("-d");
	params.append(QString(">") + dt.toString("yyyy/MM/dd hh:mm:ss"));

	cvsproc.setStandardOutputFile("templog.txt");
	cvsproc.setStandardErrorFile ("templog.err");
	cvsproc.setWorkingDirectory(cvsroot);

	WaitUpdateDialog dlg(cvsproc);

	cvsproc.start("cvs", params);

	dlg.exec();

	importLog(cvsroot, "templog.txt");

	setLastUpdate(cvsroot, QDateTime::currentDateTime());

	return 0;
}

QDateTime CVStat::getLastUpdate( const QString & cvsroot ) const
{
	QSqlQuery query(m_db);
	QString sql = QString("SELECT lastupdate FROM _config WHERE cvsroot = '%1'").arg(cvsroot);
	QDateTime lastupdate;
	if (query.exec(sql))
	{
		time_t t = 0;
		if(query.next())
			t = (time_t)query.record().value(0).toULongLong();
		lastupdate.setTime_t(t);
	}
	return lastupdate;
}

void CVStat::doRemove(const QString & cvsroot)
{
	QSqlQuery query(m_db);
	QString sql = QString("DELETE FROM _config WHERE cvsroot='%1'").arg(cvsroot);
	query.exec(sql);

	sql = QString("DROP TABLE %1").arg(makeTableName(cvsroot));
	query.exec(sql);
}


void CVStat::setLastUpdate( const QString & cvsroot, QDateTime dt )
{
	QSqlQuery query(m_db);
	QString sql = QString("UPDATE _config SET lastupdate = %1 WHERE cvsroot = '%2'").arg(dt.toTime_t()).arg(cvsroot);
	query.exec(sql);
}


int CVStat::getId( const QString & cvsroot ) const
{
	QSqlQuery query(m_db);
	QString sql = QString("SELECT cid FROM _config WHERE cvsroot = '%1'").arg(cvsroot);
	int cid = -1;
	if (query.exec(sql))
	{
		if(query.next())
			cid = query.record().value(0).toInt();
	}
	return cid;
}

void CVStat::addCVSEntry( const QString & cvsroot) const
{
	QSqlQuery query(m_db);
	QString sql = QString("SELECT COUNT(*) FROM _config WHERE cvsroot = '%1'").arg(cvsroot);
	if (query.exec(sql))
	{
		int c = 0;
		if(query.next())
			c = query.record().value(0).toInt();
		if (c == 0)
			query.exec(QString("INSERT INTO _config (cvsroot, tname, lastupdate) VALUES ('%1', '%2', 0)").arg(cvsroot).arg(makeTableName(cvsroot)));
	}
	else
	{
		qDebug() << query.lastError().text();
	}
}

void CVStat::importLog( const QString & cvsroot, const QString & logfile )
{
	QString tableName = makeTableName(cvsroot);
	QFile file(logfile);
	QTextStream ts(&file);
	ts.setCodec("gb2312");
	QString fname, revision, date, author, msg;
	int addLines, delLines;
	QSqlQuery query(m_db);
	QString dml = QString(
		"CREATE TABLE IF NOT EXISTS %1("
		"	rid INTEGER PRIMARY KEY,"
		"	rname TEXT,"
		"	rdate TEXT,"
		"	rauthor TEXT,"
		"	rrevision TEXT,"
		"	radd TEXT,"
		"	rdel TEXT,"
		"	rmsg TEXT)").arg(tableName);
	
	QRegExp reInfo("date: (\\d+/\\d+/\\d+ \\d+:\\d+:\\d+);\\s+author: (\\w+);\\s+state: \\w+;\\s+lines: (\\+\\d+) (\\-\\d+)");
	file.open(QFile::ReadOnly);

	query.exec(dml);
	query.exec("BEGIN");
	while(!ts.atEnd())
	{
		QString line = ts.readLine();
		if (line.startsWith("Working file"))
		{

			fname = line.split(':').at(1);
			while (!line.startsWith("========================") && !ts.atEnd())
			{
				line = ts.readLine();
				if (line.startsWith("revision"))
				{
					revision = line.split(' ').at(1);
					line = ts.readLine();
					reInfo.indexIn(line);
					QStringList sl = reInfo.capturedTexts();

					line = ts.readLine();
					while(
						!line.startsWith("-----------") && 
						!line.startsWith("===========") && 
						!ts.atEnd())
					{
						msg += line;
						line = ts.readLine();
					}
					if (sl.size() == 5)
					{
						date = sl.at(1);
						author = sl.at(2);
						addLines = sl.at(3).toLong();
						delLines = sl.at(4).toLong();
						QString sql = QString(
							"INSERT INTO %1(rname, rdate, rauthor, rrevision, radd, rdel, rmsg) VALUES('%2', '%3', '%4', '%5', %6, %7, '%8')"
							).arg(tableName).arg(fname).arg(date).arg(author).arg(revision).arg(addLines).arg(delLines).arg(msg);
						query.exec(sql);
						msg.clear();
					}
				}
			}
		}
	}
	query.exec("COMMIT");
}

QString CVStat::makeTableName( const QString & cvsroot ) const
{
	QByteArray r = QCryptographicHash::hash(cvsroot.toAscii(), QCryptographicHash::Md5).toHex();
	return QString("T") + QString::fromAscii(r.data(), r.size());
}

QSqlQuery CVStat::getQuery( const QString & cvsroot, const QString & sql )
{
	QString s = sql;
	return QSqlQuery(s.replace("DATA", makeTableName(cvsroot), Qt::CaseInsensitive), m_db);
}
