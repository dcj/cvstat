#include "newwizard.h"
#include <QtGui/QFileDialog>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>

NewWizard::NewWizard(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->connect(ui.browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowseClicked(bool)));
}

NewWizard::~NewWizard()
{

}

QString NewWizard::getCVSRoot() const
{
	QString cvsroot = ui.cvsRoot->currentText();
	if (isCVSDirectory(cvsroot))
		return cvsroot;
	return QString();
}

void NewWizard::setCVSRootList( const QStringList & cvsroots )
{
	ui.cvsRoot->addItems(cvsroots);
}

void NewWizard::onBrowseClicked( bool checked )
{
	QFileDialog dlg;
	dlg.setFileMode(QFileDialog::Directory);
	if (dlg.exec())
	{
		QString path = dlg.selectedFiles().at(0);
		if (isCVSDirectory(path))
			ui.cvsRoot->setEditText(path);
		else
			QMessageBox::warning(this, tr("warning"), tr("the directory selected is not a CVS directory"));
	}
}

bool NewWizard::isCVSDirectory( const QString & path ) const
{
	QDir dir(path + "/CVS");
	if (!dir.exists())
	{
		return false;
	}
	return true;

}