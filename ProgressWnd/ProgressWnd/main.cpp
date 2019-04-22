#include "progresswnd.h"
#include <QtWidgets/QApplication>
#include "mybmw.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//ProgressWnd w;
	//w.show();
	MyBMW w;
	w.show();
	return a.exec();
}
