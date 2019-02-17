#include "compresstool.h"
#include <QProcess>
#include <QApplication>

#pragma execution_character_set("utf-8")

CompressTool::CompressTool()
{

}

CompressTool::~CompressTool()
{

}

bool CompressTool::compress(QString src, QString dst)
{
	QStringList cmdparas;
	cmdparas << "a";
	cmdparas << "-r";
	cmdparas << dst;
	cmdparas << src + "/*";
	QProcess p(0);
	p.start("7za.exe", cmdparas);
	p.waitForStarted();
	p.waitForFinished();
	QString strTemp = QString::fromLocal8Bit(p.readAllStandardOutput());
	bool isOk = false;
	if (strTemp.contains("ok", Qt::CaseInsensitive))
		isOk = true;
	return isOk;
}

bool CompressTool::unCompress(QString src, QString dst)
{
	QStringList cmdparas;
	cmdparas << "e";
	cmdparas << src;
	cmdparas << dst;
	QProcess p(0);
	p.start("7za.exe", cmdparas);
	p.waitForStarted();
	p.waitForFinished();
	QString strTemp = QString::fromLocal8Bit(p.readAllStandardOutput());
	bool isOk = false;
	if (strTemp.contains("ok", Qt::CaseInsensitive))
		isOk = true;
	return isOk;
}
