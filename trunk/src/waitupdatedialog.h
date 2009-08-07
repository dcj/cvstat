#ifndef WAITUPDATEDIALOG_H
#define WAITUPDATEDIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QProcess>
class QTimer;

namespace Ui {
    class WaitUpdateDialog;
}

class WaitUpdateDialog : public QDialog {
    Q_OBJECT
public:
    WaitUpdateDialog(QProcess & process, QWidget *parent = 0);
    ~WaitUpdateDialog();

	void incressProgress();

protected slots:
	void onTimer();
	void onLogFinish(int exitCode, QProcess::ExitStatus exitStatus);
protected:
    void changeEvent(QEvent *e);

protected:
	QTimer * m_timer;

private:
    Ui::WaitUpdateDialog *m_ui;
};

#endif // WAITUPDATEDIALOG_H
