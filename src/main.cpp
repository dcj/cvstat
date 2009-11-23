#include <QtGui/QApplication>
#include <QtCore/QTranslator>
#include "mainframe.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTranslator translator;
	translator.load("zh_cn.qm");
	a.installTranslator(&translator);
	MainFrame w;
	w.show();
	return a.exec();
}
