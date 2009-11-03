#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QtGui/QMainWindow>
#include <QtCore/QStringList>
#include "ui_mainframe.h"
#include "cvstat.h"

class QListWidget;
class QTableView;
class QTextEdit;
class QSplitter;
class QListWidgetItem;
class QContextMenuEvent;

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainFrame();

protected:
	int createStat(const QString & cvsRoot, const QString & cvsPath);
	void contextMenuEvent ( QContextMenuEvent * event );
protected slots:
	void OnNewStat(bool checked);
	void OnQuery(bool checked);
	void OnUpdateStat(bool checked);
	void OnRemoveStat(bool checked);
	void OnViewFile(bool checked);
	void OnViewDiff(bool checked);
	void OnToolsExport(bool checked);
	void OnCvsRootClicked(QListWidgetItem*);
	
protected:
	QString getCurrentCvsRoot() const;
	bool checkCurrentCvsRoot(QString & cvsroot) const;

private:
	Ui::MainFrameClass ui;
	CVStat m_stat;	
	QListWidget * wCvsRoots;
	QTableView * wResultGird;
	QTextEdit *wCmdEdit;
	QSplitter * hsp;
	QSplitter * vsp;
	QStringList m_viewTools;
	QStringList m_viewDiffTools;

};

#endif // MAINFRAME_H
