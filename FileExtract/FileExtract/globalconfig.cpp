#include "globalconfig.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QApplication>
#include <QTextCodec>

GlobalConfig::GlobalConfig(QObject *parent)
	: QObject(parent)
{

}

GlobalConfig::~GlobalConfig()
{

}

//定义用#分割key#value
void GlobalConfig::SetProperty(QString key, QString value)
{
	QFile file(qApp->applicationDirPath() + "\\FileExtract.cfg");
	if (!file.exists())
	{
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			return;
		}
		QTextStream out(&file);
		out.setCodec(QTextCodec::codecForName("utf-8"));   //设置输出文件编码格式
		out.setGenerateByteOrderMark(true);
		out << key << "#" << value << endl;
		out.flush();
	}
	else if (file.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		QMap<QString, QString> keyvalue;
		while (!file.atEnd())
		{
			QString oneline = file.readLine();
			QStringList  list = oneline.split("#");
			if (list.size() == 2)
			{
				keyvalue.insert(list[0].trimmed(), list[1].trimmed());
			}
		}
		keyvalue[key] = value;//插入新值
		file.resize(0);
		QTextStream out(&file);
		out.setCodec(QTextCodec::codecForName("utf-8"));   //设置输出文件编码格式
		out.setGenerateByteOrderMark(true);
		QMap<QString, QString>::iterator it;
		for (it = keyvalue.begin(); it != keyvalue.end(); ++it)
		{
			out << it.key() << "#" << it.value() << endl;
		}
		out.flush();
	}
	file.close();
}

//定义用#分割key#value
QString GlobalConfig::GetProperty(QString key)
{
	QString value = QString();
	QFile file(qApp->applicationDirPath() + "\\FileExtract.cfg");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMap<QString, QString> keyvalue;
		while (!file.atEnd())
		{
			QString oneline = file.readLine();
			QStringList  list = oneline.split("#");
			if (list.size() == 2)
			{
				keyvalue.insert(list[0], list[1]);
			}
		}
		file.close();
		value = keyvalue.value(key,QString());
	}
	return value;
}
