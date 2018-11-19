#ifndef COMMONOPERATE_H
#define COMMONOPERATE_H

#include <QObject>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextBrowser>

class CommonOperate : public QObject
{
	Q_OBJECT

public:
	CommonOperate(QObject *parent = NULL, QMainWindow *mainWin = NULL, QTextBrowser *txtbrower = NULL);
	~CommonOperate();

	void MsgPrint(QString);
	void MsgClear();
	QMainWindow* GetMainWin();


private:
	QMainWindow * m_mainWindow;//������
	QTextBrowser * m_textBrowser;//��ʾ�ı��ؼ�
};

#endif // COMMONOPERATE_H
