#include <QMouseEvent>
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

CommonOperate * g_COperate = NULL;

test_prj::test_prj(QWidget *parent)
	: QMainWindow(parent)
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
	WinExec("FileExtract.exe", SW_SHOW);
}

void test_prj::on_pushButton_LanTool_clicked()
{
	WinExec("LanguagePackTool.exe", SW_SHOW);
}

void test_prj::on_pushButton_MsgWin_clicked()
{
	g_COperate->MsgPrint("THIS is a TEST!");
}

void test_prj::on_pushButton_TestBtn_clicked()
{
	runThread();
}

void test_prj::hideOrShowTabWidget()
{
	static bool isTabWidgetHide = false;
	ui.widget_Top->setFixedHeight(isTabWidgetHide ? 150 : 32);
	isTabWidgetHide = !isTabWidgetHide;
}