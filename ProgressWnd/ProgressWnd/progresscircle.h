#ifndef PROGRESSCIRCLE_H
#define PROGRESSCIRCLE_H

#include <QWidget>
#include "ui_progresscircle.h"

class ProgressCircle : public QWidget
{
	Q_OBJECT

public:
	ProgressCircle(QWidget *parent = 0);
	~ProgressCircle();

private:
	Ui::ProgressCircle ui;
};

#endif // PROGRESSCIRCLE_H
