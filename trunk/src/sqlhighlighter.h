#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QStringList>

class SqlHighlighter : public QSyntaxHighlighter
{
public:
	SqlHighlighter(QTextEdit * edit);

protected:
	virtual void highlightBlock ( const QString & text );

protected:
	QStringList m_keywords;
};

#endif // SQLHIGHLIGHTER_H
