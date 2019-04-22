#include "commonoperate.h"
#include <QtWidgets/QApplication>

#pragma execution_character_set("utf-8")

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
	std::lock_guard<std::mutex> lock(m_mutex);
	m_textBrowser->append(msg);
	qApp->processEvents();
}

void CommonOperate::MsgClear()
{
	if (!m_textBrowser)
	{
		return;
	}
	std::lock_guard<std::mutex> lock(m_mutex);
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
