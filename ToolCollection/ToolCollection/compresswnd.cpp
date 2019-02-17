#include "compresswnd.h"
#include "commonfunc.h"
#include "compresstool.h"
#include <QFileDialog>
#include <QMessageBox>

#pragma execution_character_set("utf-8")

CompressWnd::CompressWnd(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

CompressWnd::~CompressWnd()
{

}

void CompressWnd::on_pushButton_input_clicked()
{
	bool isCompress = ui.radioButton_compress->isChecked();
	QString path;
	QString dir = GetProperty("COMPRESSINPUT");
	if (isCompress)
	{
		path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		dir = path;
	}
	else
	{
		path = QFileDialog::getOpenFileName(this, tr("Open File"),
			dir, "all files(*.*)");
		dir = path;// QFileInfo(path).absolutePath();
	}
	if (path.isEmpty())
	{
		return;
	}
	SetProperty("COMPRESSINPUT", dir);
	ui.lineEdit_input->setText(path);
}

void CompressWnd::on_pushButton_output_clicked()
{
	bool isCompress = ui.radioButton_compress->isChecked();
	QString path;
	QString dir = GetProperty("COMPRESSOUTPUT");
	if (isCompress)
	{
		path = QFileDialog::getSaveFileName(this, tr("Save File"),
			dir, "all files(*.*)");
		dir = path;// QFileInfo(path).absolutePath();
	}
	else
	{
		path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		dir = path;
	}
	if (path.isEmpty())
	{
		return;
	}
	SetProperty("COMPRESSOUTPUT", dir);
	ui.lineEdit_output->setText(path);
}

void CompressWnd::on_pushButton_ok_clicked()
{
	bool isCompress = ui.radioButton_compress->isChecked();
	QString input = ui.lineEdit_input->text();
	QString output = ui.lineEdit_output->text();
	if (isCompress && !QDir(input).exists())
	{
		QMessageBox::information(this, "提示", "输入项应为文件夹！");
		return;
	}
	if (!isCompress && !QFile(input).exists())
	{
		QMessageBox::information(this, "提示", "输入文件不存在！");
		return;
	}
	if (!isCompress && !QDir(output).exists())
	{
		QMessageBox::information(this, "提示", "输出项应为文件夹！");
		return;
	}

	if (isCompress)//压缩
	{
		CompressTool::compress(input, output);
	}
	else//解压
	{
		CompressTool::unCompress(input, output);
	}
	QMessageBox::information(this, "提示", "完成！");
}

void CompressWnd::on_pushButton_cancel_clicked()
{
	ui.lineEdit_input->setText(QString());
	ui.lineEdit_output->setText(QString());
}
