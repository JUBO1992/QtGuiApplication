#include "languagepacktool.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	LanguagePackTool w;
	w.show();
	return a.exec();
}
