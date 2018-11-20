#include "qtguiapplication.h"

QtGuiApplication::QtGuiApplication(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

QtGuiApplication::~QtGuiApplication()
{

}
