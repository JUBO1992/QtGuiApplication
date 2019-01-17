#ifndef NETWORKSTUDY_H
#define NETWORKSTUDY_H

#include <QtWidgets/QMainWindow>
#include "ui_networkstudy.h"

class NetworkStudy : public QMainWindow
{
	Q_OBJECT

public:
	NetworkStudy(QWidget *parent = 0);
	~NetworkStudy();

private slots:
	void on_pushButton_clicked();
	void on_pushButton_login_clicked();
	void on_pushButton_download_clicked();
	void on_pushButton_upload_clicked();

private:
	Ui::NetworkStudyClass ui;
};

#endif // NETWORKSTUDY_H
