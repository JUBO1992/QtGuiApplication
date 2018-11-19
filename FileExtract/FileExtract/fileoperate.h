#ifndef FILEOPERATE_H
#define FILEOPERATE_H

#include <QObject>

static class FileOperate : public QObject
{
public:
	FileOperate(QObject *parent);
	~FileOperate();

	/*! \breif �����ļ�
	*/
	static bool CopyFile(const QString fromFile, const QString toFile, bool coverFileIfExist);

	/*! \breif ����Ŀ¼
	*/
	static bool CopyDir(const QString fromDir, const QString toDir, bool coverFileIfExist);

	/*! \breif �����ļ���Ŀ¼
	*/
	static bool CopyFileOrDir(const QString from, const QString to, bool coverFileIfExist);

	/*! \breif �����б��ȡ�ļ���Ŀ¼
	*/
	static QStringList GetFileDirsFromList(const QString fromDir, const QStringList list,
		bool fuzzy = false, bool recursion = true, bool searchFile = true, bool searchDir = true);

	static QStringList GetFileDirsFromList(const QString fromDir, const QStringList list, QMap<QString, QStringList> &searchresult, 
		bool fuzzy = false, bool recursion = true, bool searchFile = true, bool searchDir = true);


private:
	/*! \breif �����ж�
	*/
	static bool FuzzySearch( const QString fname, const QStringList list, bool fuzzy = false);

	static QString FuzzySearch2(const QString fname, const QStringList list, bool fuzzy = false);
};

#endif // FILEOPERATE_H
