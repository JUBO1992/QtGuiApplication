#include "QtGuiApplication.h"
#include "test_prj.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//QtGuiApplication w;
	//w.show();
	test_prj w;
	w.show();
	return a.exec();
}
