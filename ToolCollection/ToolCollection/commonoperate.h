#ifndef COMMONOPERATE_H
#define COMMONOPERATE_H

#include <QObject>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextBrowser>

#pragma execution_character_set("utf-8")

enum CheckStateEnum
{
	Required = 0,		//必选
	Optional = 1,		//可选
};

enum CheckLevel
{
	ImageLevel = 0,		//检查单景影像，目录下为改景影像文件
	SensorLevel = 1,	//检查一种传感器的影像，传感器目录下包含多景影像目录
	ProjectLevel = 2,	//检查一个工程的影像，工程目录下包含多个传感器目录
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
	//返回是否有错
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
	QString _parentFolder;			//父文件夹名称（如：ZY3）
	QString _imageFolder;			//当前影像文件夹名称（如：ZY3-XXXXXXXX-XXXXXXXX）
	QStringList _illegalFiles;		//命名不规范文件
	QStringList _missingFiles;		//缺失文件
	QStringList _extraFiles;		//多余文件或文件夹
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
	QMainWindow * m_mainWindow;//主窗体
	QTextBrowser * m_textBrowser;//显示文本控件
};

//文件或文件夹的父文件夹名称
QString ParentDirName(const QString& dirOrFile);

#endif // COMMONOPERATE_H
