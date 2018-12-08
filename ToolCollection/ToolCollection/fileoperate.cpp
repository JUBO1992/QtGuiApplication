#include "fileoperate.h"
#include <QString>
#include <QFile>
#include <QDir>
#include <QMap>
#include <iterator>

FileOperate::FileOperate(QObject *parent)
	: QObject(parent)
{

}

FileOperate::~FileOperate()
{

}

bool FileOperate::CopyFile(const QString fromFile, const QString toFile, bool coverFileIfExist)
{
	//�ж������ļ��Ƿ����
	if (!QFile::exists(fromFile)){
		return false;
	}
	//�ж�����������Ƿ�һ��
	QString dstFile = toFile;
	dstFile.replace("\\", "/");
	if (fromFile == dstFile){
		return true;
	}
	//�ж����·���Ƿ���ڣ��������򴴽����·��
	QString dstDir = dstFile.left(dstFile.lastIndexOf("/"));
	QDir dir(dstDir);
	if (!dir.exists())
	{
		if(!dir.mkpath(dstDir))
			return false;
	}
	//�ж�����ļ��Ƿ����
	if (QFile::exists(toFile)){
		if (coverFileIfExist){
			QFile::remove(dstFile);
		}
	}
	//�����ļ�
	if (!QFile::copy(fromFile, dstFile))
	{
		return false;
	}
	return true;
}

bool FileOperate::CopyDir(const QString fromDir, const QString toDir, bool coverFileIfExist)
{
	QDir sourceDir(fromDir);
	QDir targetDir(toDir);
	if (!targetDir.exists()){    /**< ���Ŀ��Ŀ¼�����ڣ�����д��� */
		//if (!targetDir.mkdir(targetDir.absolutePath()))
		if (!targetDir.mkpath(toDir))
			return false;
	}

	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList){
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()){    /**< ��ΪĿ¼ʱ���ݹ�Ľ���copy */
			if (!CopyDir(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName()),
				coverFileIfExist))
				return false;
		}
		else{            /**< �������ǲ���ʱ�������ļ�����ɾ������ */
			if (coverFileIfExist && targetDir.exists(fileInfo.fileName())){
				targetDir.remove(fileInfo.fileName());
			}

			/// �����ļ�copy  
			if (!QFile::copy(fileInfo.filePath(),
				targetDir.filePath(fileInfo.fileName()))){
				return false;
			}
		}
	}
	return true;
}

bool FileOperate::CopyFileOrDir(const QString from, const QString to, bool coverFileIfExist)
{
	QFileInfo info(from);
	if (info.isFile())
	{
		return CopyFile(from, to, coverFileIfExist);
	}
	if (info.isDir())
	{
		return CopyDir(from, to, coverFileIfExist);
	}
	return false;
}

QStringList FileOperate::GetFileDirsFromList(const QString fromDir, const QStringList list, 
	bool fuzzy /*= false*/, bool recursion /*= true*/, bool searchFile /*= true*/, bool searchDir /*= true*/)
{
	QStringList result;
	QDir sourceDir(fromDir);
	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList){
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;
		//if (list.indexOf(fileInfo.fileName()) >= 0)
		if (FuzzySearch(fileInfo.fileName(), list, fuzzy))
		{
			//searchFile �Ƿ������ļ�
			if (fileInfo.isFile() && searchFile)
			{
				//�˴�����ѹջȫ·����������������Ҫ���Ӳ���
				result.push_back(fileInfo.filePath());
				continue;
			}
			//searchDir �Ƿ�����Ŀ¼
			if (fileInfo.isDir() && searchDir)
			{
				//�˴�����ѹջȫ·����������
				result.push_back(fileInfo.filePath());
				continue;
			}
		}
		//recursion �Ƿ�ݹ�����
		if (fileInfo.isDir() && recursion)
		{
			QStringList templist = GetFileDirsFromList(fileInfo.filePath(), list, fuzzy, recursion, searchFile, searchDir);
			//����resultΪ����ֵ�����Դ˴���Ҫ����ѹջ
			foreach(QString str, templist)
			{
				result.push_back(str);
			}
		}
	}
	return result;
}

QStringList FileOperate::GetFileDirsFromList(const QString fromDir, const QStringList list, QMap<QString, QStringList> &searchresult, 
	bool fuzzy /*= false*/, bool recursion /*= true*/, bool searchFile /*= true*/, bool searchDir /*= true*/)
{
	QStringList result;
	QDir sourceDir(fromDir);
	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach (QFileInfo fileInfo, fileInfoList){
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;
		//if (list.indexOf(fileInfo.fileName()) >= 0)
		//if (FuzzySearch(fileInfo.fileName(), list, fuzzy))
		QString temp = fileInfo.fileName();
		QString litem = FuzzySearch2(fileInfo.fileName(), list, fuzzy);
		if (!litem.isEmpty())
		{
			//searchFile �Ƿ������ļ�
			if (fileInfo.isFile() && searchFile)
			{
				//�˴�����ѹջȫ·����������
				result.push_back(fileInfo.filePath());

				QMap<QString, QStringList>::iterator it;
				it = searchresult.find(litem);
				if (it == searchresult.end())
				{
					searchresult.insert(litem, QStringList() << fileInfo.filePath());
				}
				else
				{
					it.value() << fileInfo.filePath();
				}

				continue;
			}
			//searchDir �Ƿ�����Ŀ¼
			if (fileInfo.isDir() && searchDir)
			{
				//�˴�����ѹջȫ·����������
				result.push_back(fileInfo.filePath());

				QMap<QString, QStringList>::iterator it;
				it = searchresult.find(litem);
				if (it == searchresult.end())
				{
					searchresult.insert(litem, QStringList() << fileInfo.filePath());
				}
				else
				{
					it.value() << fileInfo.filePath();
				}
				
				continue;
			}
		}
		//recursion �Ƿ�ݹ�����
		if (fileInfo.isDir() && recursion)
		{
			QStringList templist = GetFileDirsFromList(fileInfo.filePath(), list, searchresult, fuzzy, recursion, searchFile, searchDir);
			//����resultΪ����ֵ�����Դ˴���Ҫ����ѹջ��searchresult����Ҫ����ѹջ
			foreach (QString str, templist)
			{
				result.push_back(str);
			}
		}
	}
	return result;
}

bool FileOperate::FuzzySearch(const QString fname, const QStringList list, bool fuzzy /*= false*/)
{
	if (fuzzy)
	{
		for each (QString item in list)
		{
			//if (fname.contains(item.left(item.indexOf(".")))
			//if (fname.indexOf(item.left(item.indexOf(".")) == 0)
			if (item.left(item.indexOf(".")) == fname.left(fname.indexOf(".")))
			{
				return true;
			}
		}
	}
	else
	{
		if (list.indexOf(fname) >= 0)
		{
			return true;
		}
	}
	return false;
}

QString FileOperate::FuzzySearch2(const QString fname, const QStringList list, bool fuzzy /*= false*/)
{
	if (fuzzy)
	{
		for each (QString item in list)
		{
			//if (fname.contains(item.left(item.indexOf(".")))
			//if (fname.indexOf(item.left(item.indexOf(".")) == 0)
			if (item.left(item.indexOf(".")) == fname.left(fname.indexOf(".")))
			{
				return item;
			}
		}
	}
	else
	{
		if (list.indexOf(fname) >= 0)
		{
			return fname;
		}
	}
	return QString();
}
