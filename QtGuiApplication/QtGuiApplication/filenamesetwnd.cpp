#include "filenamesetwnd.h"
#include <QMessageBox>
#include <QDir>
#include <marinebase.h>

#pragma execution_character_set("utf-8")

FileNameSetWnd::FileNameSetWnd(QWidget *parent, const QString& filename/* = QString()*/)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.lineEdit_filename->setText(filename);

	QString tempDir = qApp->applicationDirPath() + "/system/TEMPLESETFILE/";
	QStringList list;
	QDir dir(tempDir);
	dir.setFilter(QDir::Files); //设置过滤
	QFileInfoList fileInfoList = dir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		QString fileName = fileInfo.fileName();
		if (fileName.endsWith(".db",Qt::CaseInsensitive))
		{
			list << fileName;
		}
	}
	ui.comboBox_template->addItems(list);
	int idx = list.indexOf(MS_TEMPLATE_NAME);
	if (idx != -1)
	{
		ui.comboBox_template->setCurrentIndex(idx);
		m_template = MS_TEMPLATE_PATH;
	}
}

FileNameSetWnd::~FileNameSetWnd()
{

}

void FileNameSetWnd::on_pushButton_ok_clicked()
{
	m_fileName = ui.lineEdit_filename->text();
	if (m_fileName.isEmpty())
	{
		QMessageBox::information(this, "提示", "文件名不能为空！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	accept();
}

void FileNameSetWnd::on_pushButton_cancel_clicked()
{
	reject();
}

void FileNameSetWnd::on_comboBox_template_currentTextChanged(QString str)
{
	m_template = qApp->applicationDirPath() + "/system/TEMPLESETFILE/" + str;
}
