#include "fileextract.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>
#include <QProgressDialog>
#include <QTextStream>
#include "fileoperate.h"
#include "globalconfig.h"
#include "codedetect.h"
#include "commonoperate.h"

CommonOperate * g_COperate = NULL;

FileExtract::FileExtract(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	SetWindowBackground();
	////设置背景色，这里setColor和setBrush都可以使用！
	//palette.setColor(QPalette::Background, Qt::lightGray);
	//QPalette palette;
	//palette.setBrush(this->backgroundRole(), QColor(245, 245, 245));
	//palette.setBrush(this->backgroundRole(), QColor(255, 225, 179));
	//palette.setBrush(this->backgroundRole(), QColor(129, 168, 211));
	//this->setPalette(palette);
	
	//设置背景图片
	//QPixmap pixmap = QPixmap("yb-nacol.jpg").scaled(this->size());
	//QPixmap pixmap = QPixmap("yb-nacol.jpg");
	//QPalette palette(this->palette());
	//palette.setBrush(QPalette::Background, QBrush(pixmap));
	//this->setPalette(palette);

	//此方法会将控件中的子控件的背景也设置为透明
	//ui.tabWidget->setStyleSheet("background-color:rgba(0,0,0,0)");

	//此方法只设置控件的背景为透明，子控件不设置
	//QPalette pll = ui.tabWidget->palette();
	//pll.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255, 0)));
	//ui.tabWidget->setPalette(pll);

	m_model = new ListViewModel();
	ui.listView->setModel(m_model);
	m_list.clear();

	ui.dockWidget->layout()->setMargin(0);
	ui.dockWidget->layout()->setSpacing(0);

	g_COperate = new CommonOperate(NULL, this, ui.textBrowser);

	QString tabindex = GlobalConfig::GetProperty("TABWIDGETINDEX");
	if (!tabindex.isEmpty())
	{
		ui.tabWidget->setCurrentIndex(tabindex.toInt());
	}
}

FileExtract::~FileExtract()
{
	QString tabindex = QString("%1").arg(ui.tabWidget->currentIndex());
	GlobalConfig::SetProperty("TABWIDGETINDEX", tabindex);
	m_list.clear();
}

void FileExtract::SetWindowBackground()
{
	QColor qclr = QColor(245, 245, 245);//默认背景
	QString color = GlobalConfig::GetProperty("BACKCOLOR");
	QStringList clist = color.split(QRegExp(","));
	if (color.isEmpty() || clist.size() != 3)
	{
		GlobalConfig::SetProperty("BACKCOLOR", "245,245,245");
	}
	else
	{
		int r = clist[0].toInt() % 256;
		int g = clist[1].toInt() % 256;
		int b = clist[2].toInt() % 256;
		qclr = QColor(r, g, b);
	}
	QPalette palette;
	palette.setBrush(this->backgroundRole(), qclr);
	this->setPalette(palette);
	
	//QFile file("FileExtract.cfg");
	//if (!file.exists())
	//{
	//	QPalette palette;
	//	palette.setBrush(this->backgroundRole(), QColor(129, 168, 211));
	//	this->setPalette(palette);
	//	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	//	{
	//		return;
	//	}
	//	QTextStream out(&file);
	//	out << QString("%1 %2 %3").arg(129).arg(168).arg(211) << endl;
	//	out.flush();
	//}
	//else
	//{
	//	if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
	//	{
	//		return;
	//	}
	//	QString str = file.readLine();
	//	QStringList slist = str.split(QRegExp("\\s+"));
	//	if (slist.size() < 3)
	//	{
	//		QPalette palette;
	//		palette.setBrush(this->backgroundRole(), QColor(129, 168, 211));
	//		this->setPalette(palette);
	//		file.reset();
	//		QTextStream out(&file);
	//		out << QString("%1 %2 %3").arg(129).arg(168).arg(211) << endl;
	//		out.flush();
	//	}
	//	else
	//	{
	//		int r = slist[0].toInt() % 256;
	//		int g = slist[1].toInt() % 256;
	//		int b = slist[2].toInt() % 256;
	//		QPalette palette;
	//		palette.setBrush(this->backgroundRole(), QColor(r, g, b));
	//		this->setPalette(palette);
	//	}
	//}
	//file.close();
}

void FileExtract::OpenListClicked()
{
	QString dir = GlobalConfig::GetProperty("LISTFILEPATH01");
	QString fpath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Open File"),
		dir, "txt files(*.txt)");
	if (fpath.isEmpty())
	{
		return;
	}
	GlobalConfig::SetProperty("LISTFILEPATH01", QFileInfo(fpath).absolutePath() + "/");
	ui.lineEdit_ListFile->setText(fpath);
	QFile file(fpath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	EncodingType type = CodeDetect::GetEncodingType(file);
	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		QString str = QString();
		if (type == ENCODINGTYPE_UTF8)
		{
			str = QString::fromUtf8(line);
		}
		else
		{
			str = QString::fromLocal8Bit(line);
		}
		m_list.push_back(str.trimmed());
	}
	m_model->setStringList(m_list);
}

void FileExtract::InputClicked()
{
	QString dir = GlobalConfig::GetProperty("INPUTPATH01");
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"), 
		dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (path.isEmpty())
	{
		return;
	}
	GlobalConfig::SetProperty("INPUTPATH01", path);
	ui.lineEdit_Input->setText(path);
}

void FileExtract::OutputClicked()
{
	QString dir = GlobalConfig::GetProperty("OUTPUTPATH01");
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		"", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (path.isEmpty())
	{
		return;
	}
	GlobalConfig::SetProperty("OUTPUTPATH01", path);
	ui.lineEdit_Output->setText(path);
}

void FileExtract::OKClicked()
{
	if (m_list.size() <= 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString("列表不能为空！"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	QString input = ui.lineEdit_Input->text().trimmed();
	QDir dir1(input);
	if (input.isEmpty() || !dir1.exists())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString("输入目录不存在！"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	QString output = ui.lineEdit_Output->text();
	QDir dir2(output);
	if (output.isEmpty() || !dir2.exists())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString("输出目录不存在！"), QMessageBox::Ok, QMessageBox::Ok);
		return;
		//dir2.mkpath(ui.lineEdit_Output->text());
	}

	if (output.contains(input))
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString("输出目录不能是输入目录的子目录!"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	bool f1 = false, f2 = false;
	//判断input是否以/结尾
	if (input.endsWith("/") || input.endsWith("\\"))
	{
		f1 = true;
	}
	//判断output是否以/结尾
	if (output.endsWith("/") || output.endsWith("\\"))
	{
		f2 = true;
	}

	if (f1 && !f2)
	{
		output += "/";
	}
	else if (!f1 && f2)
	{
		input = input.left(input.length() - 1);
	}


	ui.pushButton_OK->setEnabled(false);

	bool recursionflag = ui.checkBox_recursion->checkState();
	bool overrideflag = ui.checkBox_override->checkState();

	QStringList searchlist = FileOperate::GetFileDirsFromList(input, m_list, false, recursionflag, true, true);

	for (int i = 0; i < searchlist.size(); ++i)
	{
		searchlist[i] = searchlist[i].mid(input.length());
	}

	QProgressDialog process(this);
	process.setLabelText(tr("processing..."));
	process.setRange(0, searchlist.size());
	process.setModal(true);
	process.setMinimumDuration(1);
	process.setCancelButtonText(tr("cancel"));
	process.show();
	for (int i = 0; i < searchlist.size(); ++i)
	{
		process.setLabelText(tr("processing...") + QString("%1").arg(i + 1));
		process.setValue(i+1);
		QCoreApplication::processEvents();
		if (process.wasCanceled())
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
				QString("您已中断操作！"), QMessageBox::Ok, QMessageBox::Ok);
			ui.pushButton_OK->setEnabled(true);
			return;
		}
		QString from = input + searchlist[i];
		QString to = output + searchlist[i];
		FileOperate::CopyFileOrDir(from, to, overrideflag);
	}
	ui.pushButton_OK->setEnabled(true);

	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
		QString("提取完成！"), QMessageBox::Ok, QMessageBox::Ok);

	//QList<QFileInfo> file(dir1.entryInfoList());
	//QProgressDialog process(this);
	//process.setLabelText(tr("processing..."));
	//process.setRange(0, file.size());
	//process.setModal(true);
	//process.setCancelButtonText(tr("cancel"));
	//int i = 0;
	//for (auto it = file.begin(); it != file.end(); it++, i++)
	//{
	//	process.setValue(i);
	//	if (process.wasCanceled())
	//	{
	//		break;
	//	}

	//	QString name = it->fileName();
	//	//if (m_list.indexOf(name) >= 0)
	//	if (m_list.contains(name))
	//	{
	//		QString indir = ui.lineEdit_Input->text() + "/" + name;
	//		QString outdir = ui.lineEdit_Output->text() + "/" + name;
	//		if (it->isFile())
	//		{
	//			FileOperate::CopyFile(indir, outdir, true);
	//		}
	//		else
	//		{
	//			FileOperate::CopyDir(indir, outdir, true);
	//		}
	//	}
	//}
}

void FileExtract::TabChanged(int idx)
{
	if (idx == 2)
	{
		ui.dockWidget->setVisible(true);
	}
	else
	{
		ui.dockWidget->setVisible(false);
	}
}
