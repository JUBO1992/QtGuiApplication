#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication.h"

class QtGuiApplication : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication(QWidget *parent = 0);

private:
	Ui::QtGuiApplicationClass ui;
};
