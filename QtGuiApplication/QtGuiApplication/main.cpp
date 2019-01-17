#include "QtGuiApplication.h"
#include "test_prj.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//风格可以是 "windows", "motif", "cde", "plastique", "windowsxp", or "macintosh"
	QApplication::setStyle("Fusion");
	//QtGuiApplication w;
	//w.show();
	test_prj w;
	w.show();
	return a.exec();
}
