﻿#ifndef TEST_PRJ_H
#define TEST_PRJ_H

#include <QtWidgets/QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include "ui_test_prj.h"
#include "mymodel.h"
#include "notepadwindow.h"
#include "drawwindow.h"
#include "spindelegate.h"
#include <functional>
#include "mybmw.h"

typedef void(*pVoidFunc)();//定义无返回值无参函数指针

struct CustomCmd
{
	QString _name;	//命令名称
	void* _func;	//命令函数
	QString _msg;	//命令说明
};

class test_prj : public QMainWindow
{
	Q_OBJECT

public:
	test_prj(QWidget *parent = 0);
	~test_prj();
	void mouseMoveEvent(QMouseEvent *);

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
	void hideOrShowTabWidget();
	void on_pushButton_marineOpen_clicked();
	void on_pushButton_marineRegister_clicked();
	void on_pushButton_RegCmd_clicked();
	void on_pushButton_Progress_clicked();
	void on_comboBox_Cmd_currentTextChanged(QString);

private:
	void initMenuBtn();
	void addCommand();
	void addCommand(const QString& name, const void* func, const QString& msg, QMap<QString, CustomCmd>& cmdList);
	void update_comboBox_Cmd();

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

	QMap<QString, CustomCmd> _cmd_list;//命令列表

	MyBMW _mybmw;
	bool _isshow;
};

#endif // TEST_PRJ_H
