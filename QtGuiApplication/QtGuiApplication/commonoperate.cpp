#include "commonoperate.h"
#include <QtWidgets/QApplication>

CommonOperate::CommonOperate(QObject *parent, QMainWindow *mainWin, QTextBrowser *txtbrower)
	: QObject(parent),
	m_mainWindow(mainWin),
	m_textBrowser(txtbrower)
{

}

CommonOperate::~CommonOperate()
{

}

void CommonOperate::MsgPrint(QString msg)
{
	if (!m_textBrowser)
	{
		return;
	}
	//如何实现线程安全的输出？？
	m_textBrowser->append(msg);
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
