#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QStandardPaths>
#include <QProcess>
#include <thread>
#include "fileextract2.h"
#include "fileoperate.h"
#include "globalconfig.h"
#include "codedetect.h"

FileExtract2::FileExtract2(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_model = new FileDirListModel();
	ui.listView->setModel(m_model);
	m_list.clear();
	m_searchdir = ui.checkBox_searchDir->checkState();
}

FileExtract2::~FileExtract2()
{
	m_list.clear();
}

void FileExtract2::scanInputClicked()
{
	QString dir = GlobalConfig::GetProperty("INPUTPATH02");
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (path.isEmpty())
	{
		return;
	}
	GlobalConfig::SetProperty("INPUTPATH02", path);
	ui.lineEdit_input->setText(path);
	searchClicked();
}

void FileExtract2::scanOutputClicked()
{
	QString dir = GlobalConfig::GetProperty("OUTPUTPATH02");
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (path.isEmpty())
	{
		return;
	}
	GlobalConfig::SetProperty("OUTPUTPATH02", path);
	ui.lineEdit_output->setText(path);
}

void FileExtract2::scanListClicked()
{
	QString dir = GlobalConfig::GetProperty("LISTFILEPATH02");
	QString fpath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Open File"),
		dir, "txt files(*.txt)");
	if (fpath.isEmpty())
	{
		return;
	}
	GlobalConfig::SetProperty("LISTFILEPATH02", QFileInfo(fpath).absolutePath() + "/");
	QFile file(fpath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	EncodingType type = CodeDetect::GetEncodingType(file);
	QByteArray all = file.readAll();
	QString str = QString();
	if (type == ENCODINGTYPE_UTF8)
	{
		str = QString::fromUtf8(all);
	}
	else
	{
		str = QString::fromLocal8Bit(all);
	}
	ui.textEdit->setText(str);
}

void FileExtract2::textEidt_textChanged()
{
	QString lines = ui.textEdit->toPlainText();
	QStringList list = lines.split("\n");
	m_list.clear();
	for (int i = 0; i < list.size(); ++i)
	{
		if (!list[i].trimmed().isEmpty())
		{
			m_list.push_back(list[i].trimmed());
		}
	}
}

void FileExtract2::searchClicked()
{
	searchFileOrDir();
}

void FileExtract2::searchResultClicked()
{
	writeProcessResult();
}

void FileExtract2::deleteClicked()
{
	//QModelIndex index = ui.listView->currentIndex();
	//int i = index.row();
	//if (i < 0)
	//{
	//	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
	//		QString::fromLocal8Bit("��ѡ��Ҫɾ���ļ�¼��"), QMessageBox::Ok, QMessageBox::Ok);
	//	return;
	//}
	//m_model->removeRow(i);
	//ui.pushButton_delete->setFocus();

	QModelIndexList modelIndexList = ui.listView->selectionModel()->selectedIndexes();
	if (modelIndexList.size() <= 0)
	{	
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("��ѡ��Ҫɾ���ļ�¼��"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	for (int i = modelIndexList.size()-1; i >= 0; --i)
	{
		int j = modelIndexList[i].row();
		m_model->removeRow(j);
	}

	QString msg = QString(QString::fromLocal8Bit("�������������ɾ����  ��%1����¼")).arg(m_model->stringList().size());
	ui.label_6->setText(msg);

}

void FileExtract2::extractClicked()
{
	extractFileOrDir();

	//auto MT_Fun = [&](){
	//	extractFileOrDir();
	//};
	//std::thread t(MT_Fun);
	//t.detach();
}

void FileExtract2::on_checkBox_fuzzy_clicked()
{
	searchFileOrDir();
}

void FileExtract2::on_checkBox_recursion_clicked()
{
	searchFileOrDir();
}

void FileExtract2::on_checkBox_searchFile_clicked()
{
	searchFileOrDir();
}

void FileExtract2::on_checkBox_searchDir_clicked()
{
	searchFileOrDir();
}

void FileExtract2::searchFileOrDir()
{
	QString input = ui.lineEdit_input->text();
	QDir dir1(input);
	if (input.isEmpty() || !dir1.exists())
	{
		//QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
		//	QString::fromLocal8Bit("����Ŀ¼�����ڣ�"), QMessageBox::Ok, QMessageBox::Ok);
		QString msg = QString(QString::fromLocal8Bit("�������������ɾ����  ����Ŀ¼��Ч��"));
		ui.label_6->setText(msg);
		return;
	}
	if (m_list.size() <= 0)
	{
		//QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
		//	QString::fromLocal8Bit("�����б�Ϊ�գ�"), QMessageBox::Ok, QMessageBox::Ok);
		QString msg = QString(QString::fromLocal8Bit("�������������ɾ����  �����б�Ϊ�գ�"));
		ui.label_6->setText(msg);
		return;
	}
	m_model->inputDir = input;

	bool flag0 = ui.checkBox_fuzzy->checkState();
	bool flag1 = ui.checkBox_recursion->checkState();
	bool flag2 = ui.checkBox_searchFile->checkState();
	bool flag3 = ui.checkBox_searchDir->checkState();
	m_searchdir = ui.checkBox_searchDir->checkState();
	m_searchresult.clear();
	for each (QString  item in m_list)
	{
		m_searchresult.insert(item, QStringList());
	}
	ui.pushButton_search->setEnabled(false);
	QStringList searchlist = FileOperate::GetFileDirsFromList(input, m_list, m_searchresult, flag0, flag1, flag2, flag3);
	ui.pushButton_search->setEnabled(true);
	for (int i = 0; i < searchlist.size(); ++i)
	{
		searchlist[i] = searchlist[i].mid(input.length());
	}

	m_model->setStringList(searchlist);

	QString msg = QString(QString::fromLocal8Bit("�������������ɾ����  ��%1����¼")).arg(searchlist.size());
	ui.label_6->setText(msg);
	//msg = QString(QString::fromLocal8Bit("��������,����������¼%1����")).arg(searchlist.size());
	//QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
	//	msg, QMessageBox::Ok, QMessageBox::Ok);
	ui.pushButton_searchresult->setEnabled(true);
}

void FileExtract2::extractFileOrDir()
{
	QStringList searchlist = m_model->stringList();
	if (searchlist.size() <= 0)
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("�������Ϊ�գ�û����Ҫ��ȡ���ļ�/Ŀ¼��"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	//QString input = ui.lineEdit_input->text().trimmed();
	//QDir dir1(input);
	//if (input.isEmpty() || !dir1.exists())
	//{
	//	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
	//		QString::fromLocal8Bit("����Ŀ¼�����ڣ�"), QMessageBox::Ok, QMessageBox::Ok);
	//	return;
	//}
	QString input = m_model->inputDir;//������һ�����ڣ�����Ҫ�����ж�

	QString output = ui.lineEdit_output->text();
	QDir dir2(output);
	if (output.isEmpty() || !dir2.exists())
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("���Ŀ¼�����ڣ�"), QMessageBox::Ok, QMessageBox::Ok);
		return;
		//dir2.mkpath(ui.lineEdit_Output->text());
	}

	if (m_searchdir && output.contains(input))
	{
		QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
			QString::fromLocal8Bit("������Ŀ¼ѡ�ѡʱ�����\nĿ¼����������Ŀ¼����Ŀ¼!"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	bool f1 = false, f2 = false;
	//�ж�input�Ƿ���/��β
	if (input.endsWith("/") || input.endsWith("\\"))
	{
		f1 = true;
	}
	//�ж�output�Ƿ���/��β
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


	ui.pushButton_extract->setEnabled(false);
	bool overrideflag = ui.checkBox_override->checkState();
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
		process.setValue(i + 1);
		QCoreApplication::processEvents();
		if (process.wasCanceled())
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
				QString::fromLocal8Bit("�����жϲ�����"), QMessageBox::Ok, QMessageBox::Ok);
			ui.pushButton_extract->setEnabled(true);
			return;
		}
		QString from = input + searchlist[i];
		QString to = output + searchlist[i];
		FileOperate::CopyFileOrDir(from, to, overrideflag);
	}

	QMessageBox::warning(this, QString::fromLocal8Bit("Warning!"),
		QString::fromLocal8Bit("��ȡ��ɣ�"), QMessageBox::Ok, QMessageBox::Ok);
	ui.pushButton_extract->setEnabled(true);
}

void FileExtract2::writeProcessResult()
{
	QString dirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/foryb-nacol";
	QDir dir(dirPath);
	if (!dir.exists())
	{
		dir.mkpath(dirPath);
	}

	QString sFilePath = dirPath + "/processresult.txt";

	QFile file(sFilePath);
	//��ʽ��AppendΪ׷�ӣ�WriteOnly��ReadOnly  
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(NULL, "Tip", QString::fromLocal8Bit("�޷������ļ�"));
		return;
	}
	QTextStream out(&file);
	QMap<QString, QStringList>::const_iterator i;
	for (i = m_searchresult.constBegin(); i != m_searchresult.constEnd(); ++i)
	{
		QStringList list = i.value();
		QString head = QString(QString::fromLocal8Bit("%1 ������ƥ���¼ %2 ��:")).arg(i.key()).arg(list.size());
		out << head << endl;
		for each (QString  str in list)
		{
			QString input = m_model->inputDir;
			str = str.mid(input.length());
			out << "  " + str << endl;
		}
		out << endl;

	}
	out.flush();
	file.close();

	QProcess* process = new QProcess();
	QString notepadPath = "notepad.exe " + sFilePath;
	process->start(notepadPath);
}
