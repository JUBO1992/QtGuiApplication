#ifndef AUTOPROGRESSBAR_H
#define AUTOPROGRESSBAR_H

#include <QWidget>
#include "ui_autoprogressbar.h"

class AutoProgressBar : public QWidget
{
    Q_OBJECT

public:
    AutoProgressBar(QWidget *parent = 0);
    ~AutoProgressBar();

private:
    Ui::AutoProgressBar ui;
};

#endif // AUTOPROGRESSBAR_H
