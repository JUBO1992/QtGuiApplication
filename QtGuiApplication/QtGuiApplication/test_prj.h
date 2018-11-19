#ifndef TEST_PRJ_H
#define TEST_PRJ_H

#include <QtWidgets/QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include "ui_test_prj.h"
#include "mymodel.h"
#include "notepadwindow.h"
#include "drawwindow.h"
#include "spindelegate.h"

class test_prj : public QMainWindow
{
	Q_OBJECT

public:
	test_prj(QWidget *parent = 0);
	~test_prj();
	void mouseMoveEvent(QMouseEvent *);

private:
	Ui::test_prjClass ui;
	NotepadWindow *np;
	DrawWindow *dw;
	QLabel *_status_label;
	MyModel *_mymodel;
	QTableView *_table_view;
	QTableView *_table_view2; 
	QStandardItemModel *_model;
	SpinDelegate *_spin_delegate;


public slots:
	void on_pushbutton_clicked();
	void on_drawwidget_clicked();
	void on_notepad_clicked();
	void on_qtable_clicked();
	void on_viewdelegate_clicked();
	void on_pushButton_FileExtract_clicked();
	void on_pushButton_LanTool_clicked();
	void on_pushButton_MsgWin_clicked();
	void on_pushButton_TestBtn_clicked();
};

#endif // TEST_PRJ_H
