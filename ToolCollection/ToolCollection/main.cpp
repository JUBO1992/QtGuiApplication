#include "fileextract.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//风格可以是 "windows", "motif", "cde", "plastique", "windowsxp", or "macintosh"
	//QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setStyle("Fusion");
	FileExtract w;
	w.show();
	return a.exec();
}
