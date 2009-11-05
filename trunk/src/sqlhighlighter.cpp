#include "sqlhighlighter.h"
#include <QRegExp>
#include <QColor>

SqlHighlighter::SqlHighlighter(QTextEdit * edit) :
		QSyntaxHighlighter(edit)
{
	m_keywords
		<< "ABORT" << "ADD" << "AFTER" << "ALL" << "ALTER" << "ANALYZE" << "AND" <<
		"AS" << "ASC" << "ATTACH" << "AUTOINCREMENT" << "BEFORE" << "BEGIN" <<
		"BETWEEN" << "BY" << "CASCADE" << "CASE" << "CAST" << "CHECK" << "COLLATE"
		 << "COLUMN" << "COMMIT" << "CONFLICT" << "CONSTRAINT" << "CREATE" <<
		"CROSS" << "CURRENT_DATE" << "CURRENT_TIME" << "CURRENT_TIMESTAMP" <<
		"DATABASE" << "DEFAULT" << "DEFERRABLE" << "DEFERRED" << "DELETE" <<
		"DESC" << "DETACH" << "DISTINCT" << "DROP" << "EACH" << "ELSE" << "END" <<
		"ESCAPE" << "EXCEPT" << "EXCLUSIVE" << "EXISTS" << "EXPLAIN" << "FAIL" <<
		"FOR" << "FOREIGN" << "FROM" << "FULL" << "GLOB" << "GROUP" << "HAVING" <<
		"IF" << "IGNORE" << "IMMEDIATE" << "IN" << "INDEX" << "INDEXED" <<
		"INITIALLY" << "INNER" << "INSERT" << "INSTEAD" << "INTERSECT" << "INTO" <<
		"IS" << "ISNULL" << "JOIN" << "KEY" << "LEFT" << "LIKE" << "LIMIT" <<
		"MATCH" << "NATURAL" << "NOT" << "NOTNULL" << "NULL" << "OF" << "OFFSET" <<
		"ON" << "OR" << "ORDER" << "OUTER" << "PLAN" << "PRAGMA" << "PRIMARY" <<
		"QUERY" << "RAISE" << "REFERENCES" << "REGEXP" << "REINDEX" << "RELEASE" <<
		"RENAME" << "REPLACE" << "RESTRICT" << "RIGHT" << "ROLLBACK" << "ROW" <<
		"SAVEPOINT" << "SELECT" << "SET" << "TABLE" << "TEMP" << "TEMPORARY" <<
		"THEN" << "TO" << "TRANSACTION" << "TRIGGER" << "UNION" << "UNIQUE" <<
		"UPDATE" << "USING" << "VACUUM" << "VALUES" << "VIEW" << "VIRTUAL" <<
		"WHEN" << "WHERE";

}

void SqlHighlighter::highlightBlock(const QString &text)
{
	int start = 0;
	QRegExp demi("\\s");
	while(start < text.length())
	{
		int stop = text.indexOf(demi, start + 1);
		if (stop < 0)
			break;
		QString token = text.mid(start, stop - start);
		if (token.compare("data", Qt::CaseInsensitive) == 0)
			setFormat(start, stop - start, QColor(0x00, 0x66, 0x33, 0xFF));
		if (m_keywords.contains(token, Qt::CaseInsensitive))
			setFormat(start, stop - start, QColor(0x00, 0x00, 0xFF, 0xFF));
		start = stop + 1;
		if (start == 0)
			break;
	}
}
