#ifndef QTGUIAPPLICATION_H
#define QTGUIAPPLICATION_H

#include <QtWidgets/QMainWindow>
#include "ui_qtguiapplication.h"

class QtGuiApplication : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication(QWidget *parent = 0);
	~QtGuiApplication();

private:
	Ui::QtGuiApplicationClass ui;
};

#endif // QTGUIAPPLICATION_H
