#ifndef QUERYEXTOOLSDIALOG_H
#define QUERYEXTOOLSDIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QStringList>

namespace Ui {
    class QueryExToolsDialog;
}

class QueryExToolsDialog : public QDialog {
    Q_OBJECT
public:
    QueryExToolsDialog(QWidget *parent = 0);
    ~QueryExToolsDialog();

	QStringList getTools() const;

protected:
    void changeEvent(QEvent *e);


protected slots:
	void OnBrowsClicked();
	void accept();

protected:
	QStringList m_tools;
private:
    Ui::QueryExToolsDialog *m_ui;
};

#endif // QUERYEXTOOLSDIALOG_H
