#ifndef FILEOPERATE_H
#define FILEOPERATE_H

#include <QObject>
#include "baseoperatelib_global.h"

class BASEOPERATELIB_EXPORT FileOperate : public QObject
{
public:
	FileOperate(QObject *parent);
	~FileOperate();

	/*! \brief 拷贝文件或目录
	 *  \param [in] from源文件或目录
	 *  \param [in] to目标文件或目录
	 *  \param [in] coverFileIfExist是否覆盖
	 *  \return 返回是否成功
	 */
	static bool CopyFileOrDir(const QString& from, const QString& to, bool coverFileIfExist = true);

	/*! \brief 删除文件或目录
	 *  \param [in] path文件或目录路径
 	 *  \return 返回是否成功
	 */
	static bool DelFileOrDir(const QString& path);

	/*! \brief 搜索目录下所有文件
	 *  \param [in] folder搜索路径
	 *  \return 返回搜索到的文件列表
	 */
	static QStringList SearchFilesInDir(const QString& folder);

	/*! \brief 根据列表获取文件、目录
	*/
	static QStringList GetFileDirsFromList(const QString& fromDir, const QStringList& list,
		bool fuzzy = false, bool recursion = true, bool searchFile = true, bool searchDir = true);

	static QStringList GetFileDirsFromList(const QString& fromDir, const QStringList& list, QMap<QString, QStringList> &searchresult, 
		bool fuzzy = false, bool recursion = true, bool searchFile = true, bool searchDir = true);


private:
	//brief 拷贝文件
	static bool CopyFile(const QString& fromFile, const QString& toFile, bool coverFileIfExist = true);

	//brief 拷贝目录
	static bool CopyDir(const QString& fromDir, const QString& toDir, bool coverFileIfExist = true);

	//brief 删除文件
	static bool DeleteFile(const QString& file);

	//brief 删除目录
	static bool DeleteDir(const QString& folder);

	//brief 搜索判断，返回是否存在
	static bool FuzzySearch( const QString& fname, const QStringList& list, bool fuzzy = false);

	//brief 搜索判断，返回文件名
	static QString FuzzySearch2(const QString& fname, const QStringList& list, bool fuzzy = false);
};

#endif // FILEOPERATE_H
