#ifndef __FILEREAD_H__
#define __FILEREAD_H__

#include <QStringList>
#include <QFile>

class FileRead
{
public:
	enum OpenFlag {
		Readonly,
		ReadAndWrite
	};

	FileRead(QString RegionDir, OpenFlag of = Readonly);
	~FileRead();

	void extractStr(std::map<QString, QStringList> &words,QString regx);
	void updateStr(std::map<QString, QString> &languageMap, QString regx);
	void updateStr2(std::map<QString, QString> &languageMap, QString regx);
	void readstrFile(QString RegionDir, OpenFlag of = Readonly);
	bool analysisStr(QString str);
	QString subQStr(QString str, int &pos);
	QString trope(QString str);
	QString _trope(QString str);
private:
	QFile strFile;
	QString fileDir;
	bool doemit;
	bool isSame;
	bool isUiFile;
};

#endif