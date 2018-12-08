#ifndef COMMONOPERATE_H
#define COMMONOPERATE_H

#include <QObject>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextBrowser>

#pragma execution_character_set("utf-8")

enum CheckStateEnum
{
	Required = 0,		//��ѡ
	Optional = 1,		//��ѡ
};

enum CheckLevel
{
	ImageLevel = 0,		//��鵥��Ӱ��Ŀ¼��Ϊ�ľ�Ӱ���ļ�
	SensorLevel = 1,	//���һ�ִ�������Ӱ�񣬴�����Ŀ¼�°����ྰӰ��Ŀ¼
	ProjectLevel = 2,	//���һ�����̵�Ӱ�񣬹���Ŀ¼�°������������Ŀ¼
};

struct CheckItemStruct
{
	CheckItemStruct(bool isCheck = true, CheckStateEnum state = Required, QString condition = "")
	{
		_isCheck = isCheck;
		_state = state;
		_condition = condition;
	}
	bool _isCheck;
	CheckStateEnum _state;
	QString _condition;
};

struct ErrorMsgStruct
{
	//�����Ƿ��д�
	bool HasError()
	{
		if (_imageFolder.isEmpty())
		{
			return false;
		}
		if (_imageFolder.contains(_parentFolder,Qt::CaseInsensitive)
			&& _illegalFiles.size() == 0
			&& _missingFiles.size() == 0
			&& _extraFiles.size() == 0)
		{
			return false;
		}
		return true;
	}
	QString _parentFolder;			//���ļ������ƣ��磺ZY3��
	QString _imageFolder;			//��ǰӰ���ļ������ƣ��磺ZY3-XXXXXXXX-XXXXXXXX��
	QStringList _illegalFiles;		//�������淶�ļ�
	QStringList _missingFiles;		//ȱʧ�ļ�
	QStringList _extraFiles;		//�����ļ����ļ���
};

class CommonOperate : public QObject
{
	Q_OBJECT

public:
	CommonOperate(QObject *parent = NULL, QMainWindow *mainWin = NULL, QTextBrowser *txtbrower = NULL);
	~CommonOperate();

	QString GetErrMsg(ErrorMsgStruct);
	void MsgPrint(QString);
	void MsgPrint(ErrorMsgStruct);
	void MsgClear();
	QMainWindow* GetMainWin();

	bool IsMsgWndVisible();
	void SetMsgWndVisible(bool visible);


private:
	QMainWindow * m_mainWindow;//������
	QTextBrowser * m_textBrowser;//��ʾ�ı��ؼ�
};

//�ļ����ļ��еĸ��ļ�������
QString ParentDirName(const QString& dirOrFile);

#endif // COMMONOPERATE_H
