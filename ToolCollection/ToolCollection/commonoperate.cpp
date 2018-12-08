#include "commonoperate.h"
#include <QtWidgets/QApplication>
#include "fileextract.h"

CommonOperate::CommonOperate(QObject *parent, QMainWindow *mainWin, QTextBrowser *txtbrower)
	: QObject(parent),
	m_mainWindow(mainWin),
	m_textBrowser(txtbrower)
{

}

CommonOperate::~CommonOperate()
{

}

QString CommonOperate::GetErrMsg(ErrorMsgStruct errMsg)
{
	if (!errMsg.HasError())
	{
		return QString();
	}
	QString msg;
	msg += QString("%1:\r\n").arg(errMsg._imageFolder);
	if (!errMsg._imageFolder.contains(errMsg._parentFolder, Qt::CaseInsensitive))
	{
		msg += "Warning-影像目录存放路径可能有误！\r\n";
	}
	if (errMsg._missingFiles.size() > 0)
	{
		msg += "Error-影像目录缺少必要文件：\r\n";
		msg += "  " + errMsg._missingFiles.join("\r\n  ") + "\r\n";
	}
	if (errMsg._illegalFiles.size() > 0)
	{
		msg += "Error-影像目录存在命名不规范文件：\r\n";
		msg += "  " + errMsg._illegalFiles.join("\r\n  ") + "\r\n";
	}
	if (errMsg._extraFiles.size() > 0)
	{
		msg += "Error-影像目录存在多余文件：\r\n";
		msg += "  " + errMsg._extraFiles.join("\r\n  ") + "\r\n";
	}
	//msg += "\r\n";
	return msg;
}

void CommonOperate::MsgPrint(QString msg)
{
	if (!m_textBrowser)
	{
		return;
	}
	m_textBrowser->append(msg);
	qApp->processEvents();
}

void CommonOperate::MsgPrint(ErrorMsgStruct errMsg)
{
	if (!m_textBrowser)
	{
		return;
	}
	MsgPrint(GetErrMsg(errMsg));
	qApp->processEvents();
}

void CommonOperate::MsgClear()
{
	if (!m_textBrowser)
	{
		return;
	}
	m_textBrowser->clear();
	qApp->processEvents();
}

QMainWindow* CommonOperate::GetMainWin()
{
	if (!m_mainWindow)
	{
		return NULL;
	}
	return m_mainWindow;
}

bool CommonOperate::IsMsgWndVisible()
{
	if (!m_mainWindow)
	{
		return false;
	}
	
	FileExtract* wnd = dynamic_cast<FileExtract*>(m_mainWindow);
	if (wnd)
	{
		return wnd->isMsgWndVisible();
	}
}

void CommonOperate::SetMsgWndVisible(bool visible)
{
	if (!m_mainWindow)
	{
		return;
	}
	FileExtract* wnd = dynamic_cast<FileExtract*>(m_mainWindow);
	if (wnd)
	{
		wnd->setMsgWndVisible(visible);
	}
}

QString ParentDirName(const QString& dirOrFile)
{
	QString parentFolder = dirOrFile;
	parentFolder.replace("\\", "/");
	parentFolder = parentFolder.left(parentFolder.lastIndexOf("/"));
	parentFolder = parentFolder.mid(parentFolder.lastIndexOf("/") + 1);
	return parentFolder;
}
