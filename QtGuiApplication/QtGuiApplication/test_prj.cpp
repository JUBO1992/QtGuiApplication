﻿#include <QMouseEvent>
#include <QPoint>
#include <QTextStream>
#include <QStringList>
#include "test_prj.h"
#include "commonoperate.h"
#include "threadtest.h"
#include <windows.h>
#include "menuitemwidget.h"
#include <QWidgetAction>
#include <QMenu>
#include "projectopenwnd.h"
#include "projectregisterwnd.h"
#include <thread>
#include "projcsstr.h"
#include <QFileDialog>
#include "commandfunc.h"
#include "progressbase.h"
#include "mybmw.h"

#pragma execution_character_set("utf-8")

using namespace std;

CommonOperate * g_COperate = NULL;
QString g_curOpenPrjID = QString();

test_prj::test_prj(QWidget *parent)
: QMainWindow(parent)
, _mybmw(0)
, _isshow(false)
{
	ui.setupUi(this);

	setWindowTitle(tr("MainWindow"));
	//setMouseTracking(true);
	//setAttribute(Qt::WA_Hover, true);
	//setFixedSize(366, 312);
	//setWindowFlags(Qt::WindowCloseButtonHint);
	ui.centralWidget->layout()->setMargin(0);
	ui.centralWidget->layout()->setSpacing(0);


	np = new NotepadWindow();
	dw = new DrawWindow();
	_mymodel = new MyModel(0);
	_table_view = new QTableView;
	_table_view2 = new QTableView;
	_model = new QStandardItemModel(4, 4);
	_spin_delegate = new SpinDelegate;

	ui.imgLabel->setScaledContents(true);
	QImage img;
	img.load("png/image.png");
	ui.imgLabel->setPixmap(QPixmap::fromImage(img));

	_status_label = new QLabel;
	ui.statusBar->addWidget(_status_label);
	//_status_label->setMaximumSize(150, 20);
	_status_label->setText(tr("This is a test!"));

	g_COperate = new CommonOperate(NULL, this, ui.textBrowser);

	QFile qssfile("basestyle.qss");
	qssfile.open(QFile::ReadOnly);
	QString qss;
	qss = qssfile.readAll();
	this->setStyleSheet(qss);

	ui.pushButton_MenuFile->setParent(ui.tabWidget_Main);

	ui.widget_Top->layout()->setMargin(0);
	ui.widget_Top->layout()->setSpacing(0);

	ui.dockWidgetContents->layout()->setMargin(0);
	ui.dockWidgetContents->layout()->setSpacing(0);

	initMenuBtn();

	connect(ui.tabWidget_Main, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(hideOrShowTabWidget()));

	addCommand();
	update_comboBox_Cmd();
}

test_prj::~test_prj()
{

}

void test_prj::initMenuBtn()
{
	QMenu* menu = new QMenu(this);
	ui.pushButton_MenuFile->setMenu(menu);

	QWidgetAction* actionNew = new QWidgetAction(this);
	actionNew->setDefaultWidget(new MenuItemWidget(QPixmap("png/file_new.png"), QString::fromStdWString(L"新建"), this));
	menu->addAction(actionNew);


	QWidgetAction* actionOpen = new QWidgetAction(this);
	actionOpen->setDefaultWidget(new MenuItemWidget(QPixmap("png/file_open.png"), QString::fromStdWString(L"打开"), this));
	menu->addAction(actionOpen);

	QWidgetAction* actionSave = new QWidgetAction(this);
	actionSave->setDefaultWidget(new MenuItemWidget(QPixmap("png/file_save.png"), QString::fromStdWString(L"保存"), this));
	menu->addAction(actionSave);

	QWidgetAction* actionSaveAs = new QWidgetAction(this);
	actionSaveAs->setDefaultWidget(new MenuItemWidget(QPixmap("png/file_saveas.png"), QString::fromStdWString(L"另存为"), this));
	menu->addAction(actionSaveAs);

	menu->addSeparator();

	QWidgetAction* actioPrint = new QWidgetAction(this);
	actioPrint->setDefaultWidget(new MenuItemWidget(QPixmap("png/file_print.png"), QString::fromStdWString(L"打印"), this));
	menu->addAction(actioPrint);

	menu->addSeparator();

	QWidgetAction* actionAout = new QWidgetAction(this);
	actionAout->setDefaultWidget(new MenuItemWidget(QPixmap("png/about.png"), QString::fromStdWString(L"关于"), this));
	menu->addAction(actionAout);

	QWidgetAction* actionExit = new QWidgetAction(this);
	actionExit->setDefaultWidget(new MenuItemWidget(QPixmap("png/exit.png"), QString::fromStdWString(L"退出"), this));
	menu->addAction(actionExit);
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

}

void test_prj::addCommand()
{
	addCommand("test1", test_func1, "test1", _cmd_list);
	addCommand("test2", test_func2, "test2", _cmd_list);
	addCommand("test3", test_func3, "test3", _cmd_list);
	addCommand("test4", test_func4, "test4", _cmd_list);
	addCommand("test5", test_func5, "test5", _cmd_list);
}

void test_prj::addCommand(const QString& name, const void* func, const QString& msg, QMap<QString, CustomCmd>& cmdList)
{
	CustomCmd cmd;
	cmd._name = name;
	cmd._func = const_cast<void*>(func);
	cmd._msg = msg;
	cmdList.insert(name, cmd);
}

void test_prj::update_comboBox_Cmd()
{
	ui.comboBox_Cmd->clear();
	QStringList list = _cmd_list.keys();
	ui.comboBox_Cmd->addItems(list);
}

void test_prj::mouseMoveEvent(QMouseEvent *e)
{
	QPoint p = e->pos();
	_status_label->setText(tr("x:%1, y:%2").arg(p.x()).arg(p.y()));
}

void test_prj::on_pushbutton_clicked()
{
	QString ltxt = ui.pushButton->text();
	if (ltxt.compare("Hello world", Qt::CaseSensitive) == 0)
	{
		ui.pushButton->setText("Hi you!");
	}
	else
	{
		ui.pushButton->setText("Hello world");
	}
}

void test_prj::on_drawwidget_clicked()
{
	dw->show();
}

void test_prj::on_notepad_clicked()
{
	np->show();
}

void test_prj::on_qtable_clicked()
{
	_table_view->setWindowTitle(tr("QTable Window"));
	_table_view->setModel(_mymodel);
	_table_view->show();
}

void test_prj::on_viewdelegate_clicked()
{
	_table_view2->setModel(_model);

	_table_view2->setItemDelegateForColumn(3, _spin_delegate);

	_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
	_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Birthday"));
	_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Work"));
	_model->setHeaderData(3, Qt::Horizontal, QObject::tr("Income"));

	QFile file("test.txt");
	if (file.open(QFile::ReadOnly|QFile::Text))
	{
		QTextStream stream(&file);
		QString line;

		_model->removeRows(0, _model->rowCount(QModelIndex()), QModelIndex());
		int row = 0;
		do 
		{
			line = stream.readLine();
			if (!line.isEmpty())
			{
				_model->insertRows(row, 1, QModelIndex());
				QStringList pieces = line.split(",", QString::SkipEmptyParts);
				_model->setData(_model->index(row, 0, QModelIndex()), pieces.value(0));
				_model->setData(_model->index(row, 1, QModelIndex()), pieces.value(1));
				_model->setData(_model->index(row, 2, QModelIndex()), pieces.value(2));
				_model->setData(_model->index(row, 3, QModelIndex()), pieces.value(3));
				row++;
			}
		} while (!line.isEmpty());
		file.close();
	}
	_table_view2->setWindowTitle(QObject::tr("View Delegate Test"));
	_table_view2->show();
}

void test_prj::on_pushButton_FileExtract_clicked()
{
	WinExec("ToolCollection.exe", SW_SHOW);
}

void test_prj::on_pushButton_LanTool_clicked()
{
	WinExec("LanguagePackTool.exe", SW_SHOW);
}

void test_prj::on_pushButton_MsgWin_clicked()
{
	g_COperate->MsgPrint("THIS is a TEST!");
}

void testtest(bool* isok=0)
{
	if (isok)*isok = false;
}

void test_prj::on_pushButton_TestBtn_clicked()
{

	bool isok = true;
	testtest();

	if (_isshow)
	{
		_mybmw.hide();
		_isshow = false;
	}
	else
	{
		_mybmw.show();
		_isshow = true;
	}
	return;

	//QString cmd = ui.lineEdit_cmd->text();
	//g_COperate->MsgPrint(cmd);
	//auto it = _cmd_list.find(cmd);
	//if (it != _cmd_list.end())
	//{
	//	//void(*func)();
	//	//func = (void(*)())it->_func;
	//	//func();
	//	pVoidFunc func = (pVoidFunc)it->_func;
	//	func();
	//}
	//else
	//{
	//	g_COperate->MsgPrint("There is no such cmd!");
	//}
	//return;

	//QString path = QFileDialog::getOpenFileName(NULL, "Open File", "", "prj file(*.prj)");
	//QFile file(path);
	//if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	//{
	//	QString str = file.readAll();
	//	ProjcsStr projObj(str);
	//}

	runThread2();

	////通过函数指针创建线程
	//std::thread t1(counter, 1, 6);
	//std::thread t2(counter, 2, 4);
	//t1.join();
	//t2.join();

	////通过函数对象创建线程
	//std::thread t1{ Counter(1, 1000) };
	//std::thread t2((Counter(1, 1000)));
	//for (int i = 0; i < 1000; ++i)
	//{
	//	g_COperate->MsgPrint(QString("Counter %1 has value %2").arg(0).arg(i));
	//}
	//t1.join();
	//t2.join();

	auto MT_Func = [](int id, int count){
		for (int i = 0; i < count; ++i)
		{
			g_COperate->MsgPrint(QString("Counter %1 has value %2").arg(id).arg(i));
		}
	};

	////多线程进度条测试
	//int threadCount = 2*std::thread::hardware_concurrency();
	//std::thread* threads = new std::thread[threadCount - 1];
	//for (int i = 0; i < threadCount - 1; ++i)
	//{
	//	//threads[i] = std::thread((Counter(i + 1, 500)));
	//	threads[i] = std::thread(MT_Func, i + 1, 500);
	//}
	////Counter(0, 500);
	////counter(0, 500);
	//for (int i = 0; i < threadCount - 1; ++i)
	//{
	//	threads[i].join();
	//}
	//delete[] threads;

	//_mybmw.show();
	ProgressTracker progress("一般进度条测试");
	progress.CreateProgress();
	for (int i = 0; i < 1000; ++i)
	{
		progress.SetProgressValue(i, 1000);
		g_COperate->MsgPrint(QString("Counter %1 has value %2").arg(0).arg(i));
	}
	progress.CloseProgress();
	//_mybmw.hide();

	////通过lambda表达式创建线程
	//std::thread t1([](int id, int numIter){
	//	for (int i = 0; i < numIter; ++i)
	//	{
	//		g_COperate->MsgPrint(QString("Counter %1 has value %2").arg(id).arg(i));
	//	}
	//}, 1, 5);
	//t1.join();
}

void test_prj::hideOrShowTabWidget()
{
	static bool isTabWidgetHide = false;
	ui.widget_Top->setFixedHeight(isTabWidgetHide ? 150 : 32);
	isTabWidgetHide = !isTabWidgetHide;
}

void test_prj::on_pushButton_marineOpen_clicked()
{
	ProjectOpenWnd openWnd(this);
	openWnd.exec();

}

void test_prj::on_pushButton_marineRegister_clicked()
{
	ProjectRegisterWnd registerWnd(this);
	registerWnd.exec();
}

#include <QLibrary>
typedef void(*pCmdRegister)(QStringList & arrNames, QList<void*> & arrFun, QStringList& explainList);
QMap<QString, QLibrary> g_libs;//记录加载的插件，key为插件全路径，value为插件对象

void test_prj::on_pushButton_RegCmd_clicked()
{
	//打开自定义pin后缀的dll文件进行命令注册
	QString dllpath = QFileDialog::getOpenFileName(NULL, "Open File", "", "pin file(*.pin)");
	if (g_libs.contains(dllpath))
	{
		g_COperate->MsgPrint("插件已加载！");
		return;
	}
	QLibrary mylib(dllpath);
	if (!mylib.load())
	{
		g_COperate->MsgPrint("插件加载失败！");
		return;
	}
	pCmdRegister regFunc = (pCmdRegister)mylib.resolve("CmdRegister");
	if (!regFunc)
	{
		g_COperate->MsgPrint("插件格式不匹配！");
		return;
	}
	QStringList arrNames;
	QList<void*> arrFun;
	QStringList explainList;
	regFunc(arrNames, arrFun, explainList);
	for (int i = 0; i < arrNames.size(); ++i)
	{
		CustomCmd cmd;
		cmd._name = arrNames[i];
		cmd._func = arrFun[i];
		cmd._msg = explainList[i];
		if (_cmd_list.contains(cmd._name))
		{
			g_COperate->MsgPrint(QString("%1命令已注册！").arg(cmd._name));
			continue;
		}
		else
		{
			g_COperate->MsgPrint(QString("%1命令注册成功！").arg(cmd._name));
			_cmd_list.insert(cmd._name, cmd);
		}
	}
	update_comboBox_Cmd();
}

void test_prj::on_pushButton_Progress_clicked()
{
	WinExec("ProgressWnd.exe", SW_SHOW);
}

void test_prj::on_comboBox_Cmd_currentTextChanged(QString)
{
	QString cmd = ui.comboBox_Cmd->currentText();
	g_COperate->MsgPrint(cmd);
	auto it = _cmd_list.find(cmd);
	if (it != _cmd_list.end())
	{
		//void(*func)();
		//func = (void(*)())it->_func;
		//func();
		pVoidFunc func = (pVoidFunc)it->_func;
		func();
	}
	else
	{
		g_COperate->MsgPrint("There is no such cmd!");
	}
	return;
}
