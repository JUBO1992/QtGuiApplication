#ifndef MSURVEYMETHODWND_H
#define MSURVEYMETHODWND_H

#include <QDialog>
#include "ui_msurveymethodwnd.h"
#include "marinebase.h"

class MSurveyMethodWnd : public QDialog
{
	Q_OBJECT

public:
	MSurveyMethodWnd(QWidget *parent = 0);
	~MSurveyMethodWnd();

	MSurveyMethod getMethod(){ return m_mehtod; }
	private slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();

private:
	Ui::MSurveyMethodWnd ui;
	MSurveyMethod m_mehtod;
};

#endif // MSURVEYMETHODWND_H
