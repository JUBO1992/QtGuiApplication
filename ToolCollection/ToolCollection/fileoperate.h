#ifndef FILEOPERATE_H
#define FILEOPERATE_H

#include <QObject>

static class FileOperate : public QObject
{
public:
	FileOperate(QObject *parent);
	~FileOperate();

	/*! \breif 拷贝文件
	*/
	static bool CopyFile(const QString fromFile, const QString toFile, bool coverFileIfExist);

	/*! \breif 拷贝目录
	*/
	static bool CopyDir(const QString fromDir, const QString toDir, bool coverFileIfExist);

	/*! \breif 拷贝文件或目录
	*/
	static bool CopyFileOrDir(const QString from, const QString to, bool coverFileIfExist);

	/*! \breif 根据列表获取文件、目录
	*/
	static QStringList GetFileDirsFromList(const QString fromDir, const QStringList list,
		bool fuzzy = false, bool recursion = true, bool searchFile = true, bool searchDir = true);

	static QStringList GetFileDirsFromList(const QString fromDir, const QStringList list, QMap<QString, QStringList> &searchresult, 
		bool fuzzy = false, bool recursion = true, bool searchFile = true, bool searchDir = true);


private:
	/*! \breif 搜索判断
	*/
	static bool FuzzySearch( const QString fname, const QStringList list, bool fuzzy = false);

	static QString FuzzySearch2(const QString fname, const QStringList list, bool fuzzy = false);
};

#endif // FILEOPERATE_H
