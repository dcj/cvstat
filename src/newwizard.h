#ifndef NEWWIZARD_H
#define NEWWIZARD_H

#include <QDialog>
#include "ui_newwizard.h"

class NewWizard : public QDialog
{
	Q_OBJECT

public:
	NewWizard(QWidget *parent = 0);
	~NewWizard();

	QString getCVSRoot() const;

	void setCVSRootList(const QStringList & cvsroots );
	bool isCVSDirectory(const QString & path) const;

protected slots:
	void onBrowseClicked(bool checked);

private:
	Ui::NewWizardClass ui;
};

#endif // NEWWIZARD_H
