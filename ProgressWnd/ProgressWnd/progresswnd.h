#ifndef PROGRESSWND_H
#define PROGRESSWND_H

#include <QtWidgets/QMainWindow>
#include "ui_progresswnd.h"

class ProgressWnd : public QMainWindow
{
	Q_OBJECT

public:
	ProgressWnd(QWidget *parent = 0);
	~ProgressWnd();

private:
	Ui::ProgressWndClass ui;
};

#endif // PROGRESSWND_H
