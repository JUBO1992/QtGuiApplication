#ifndef COMPRESSWND_H
#define COMPRESSWND_H

#include <QWidget>
#include "ui_compresswnd.h"

class CompressWnd : public QWidget
{
	Q_OBJECT

public:
	CompressWnd(QWidget *parent = 0);
	~CompressWnd();

	private slots:
	void on_pushButton_input_clicked();
	void on_pushButton_output_clicked();
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();


private:
	Ui::CompressWnd ui;
};

#endif // COMPRESSWND_H
