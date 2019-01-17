#include "networkstudy.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//风格可以是 "windows", "motif", "cde", "plastique", "windowsxp", or "macintosh"
	QApplication::setStyle("Fusion");
	NetworkStudy w;
	w.show();
	return a.exec();
}
