#ifndef CHECKITEMWND_H
#define CHECKITEMWND_H

#include <QDialog>
#include "ui_checkitemwnd.h"
#include "commonoperate.h"

class CheckItemWnd : public QDialog
{
	Q_OBJECT

public:
	CheckItemWnd(QWidget *parent = 0, CheckItemStruct item = CheckItemStruct());
	~CheckItemWnd();

	CheckItemStruct getCheckItem(){ return m_item; }
	private slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();

private:
	Ui::CheckItemWnd ui;
	CheckItemStruct m_item;
};

#endif // CHECKITEMWND_H
