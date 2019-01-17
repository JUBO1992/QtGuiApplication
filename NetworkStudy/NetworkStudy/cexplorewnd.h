#ifndef CEXPLOREWND_H
#define CEXPLOREWND_H

#include <QDialog>
#include "ui_cexplorewnd.h"

class CExploreWnd : public QDialog
{
	Q_OBJECT

public:
	CExploreWnd(QWidget *parent = 0);
	~CExploreWnd();

	private slots:
	void on_pushButton_go_clicked();

private:
	Ui::CExploreWnd ui;
};

#endif // CEXPLOREWND_H
