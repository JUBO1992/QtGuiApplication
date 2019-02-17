#include "marineproject.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include "marineprjfunc.h"

#pragma execution_character_set("utf-8")

CMarineProject::CMarineProject(MarinePrjStruct marine)
	: m_project(marine)
{
	getProjectFiles();
}

CMarineProject::~CMarineProject()
{

}

void CMarineProject::setNewMarinePrj(const MarinePrjStruct marine)
{
	m_project = marine;
	getProjectFiles();
	m_updateFiles.clear();
}

void CMarineProject::updateCurMarinePrj(const MarinePrjStruct marine)
{
	UpdateMarinePrj(marine, m_project._prjID);
	m_project = marine;
	getProjectFiles();
	m_updateFiles.clear();
}

void CMarineProject::getProjectFiles()
{
	m_vectorFiles.clear();
	m_rasterFiles.clear();
	m_surveyFiles.clear();
	m_archiveFiles.clear();
	if (!isValid()) return;

	GetMarinePrjFiles(m_project, MSFileEnum::VectorFile, m_vectorFiles);
	GetMarinePrjFiles(m_project, MSFileEnum::RasterFile, m_rasterFiles);
	GetMarinePrjFiles(m_project, MSFileEnum::SurveyFile, m_surveyFiles);
	GetMarinePrjFiles(m_project, MSFileEnum::ArchiveFile, m_archiveFiles);

	for (auto it = m_updateFiles.begin(); it != m_updateFiles.end(); ++it)
	{
		QString fileNO = it.key();
		MSFileStruct curFile = it.value();
		if (m_rasterFiles.find(fileNO) != m_rasterFiles.end())
		{
			m_rasterFiles[fileNO] = curFile;
		}
		if (m_surveyFiles.find(fileNO) != m_surveyFiles.end())
		{
			m_surveyFiles[fileNO] = curFile;
		}
		if (m_archiveFiles.find(fileNO) != m_archiveFiles.end())
		{
			m_archiveFiles[fileNO] = curFile;
		}
	}
}

bool isFileInList(const QString& file, const QMap<QString, MSFileStruct>& files, bool onlyCmpName)
{
	auto it = files.begin();
	for (; it != files.end(); ++it)
	{
		if (onlyCmpName)
		{
			if (QFileInfo(file).fileName().compare(it->fileName(), Qt::CaseInsensitive) == 0)
			{
				return true;
			}
		}
		else
		{
			if (file.compare(it->absolutePath(), Qt::CaseInsensitive) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

void CMarineProject::createVector(const QString& fileName, const QString& tempPath)
{
	QString mdbpath = vectorPath() + "/" + fileName;
	if (!mdbpath.endsWith(".mdb", Qt::CaseInsensitive))
		mdbpath += ".mdb";
	if (QFileInfo(mdbpath).exists())
	{
		if (QMessageBox::Yes != QMessageBox::information(NULL, "提示", "数据库已存在，是否覆盖？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
		{
			return;
		}
	}
	QString tmplate = MS_TEMPLATE_PATH;
	if (!tempPath.isEmpty()) tmplate = tempPath;
	bool isSuccess = false;// g_pInterface->newMDB(mdbpath, QString(), tmplate);
	if (isSuccess)
	{
		QMessageBox::information(NULL, "提示", "新建数据库成功！", QMessageBox::Ok, QMessageBox::Ok);
	}
	else
	{
		QMessageBox::information(NULL, "提示", "新建数据库失败！", QMessageBox::Ok, QMessageBox::Ok);
	}
	getProjectFiles();
}

void CMarineProject::addVectors(const QStringList& vectorPathArr)
{
	QStringList newlist = vectorPathArr;	//需要新增的文件列表
	QStringList overlayList;	//需要覆盖的文件列表
	QStringList failedList;		//覆盖失败文件列表
	for (int i = vectorPathArr.size() - 1; i >= 0; --i)
	{
		QString file = vectorPathArr.at(i);
		if (isFileInList(file, m_vectorFiles, true))
		{
			overlayList.insert(0, file);
			newlist.removeAt(i);
		}
	}
	if (overlayList.size() > 0)
	{
		QStringList fileNameList;
		for (int i = 0; i < overlayList.size(); ++i)
		{
			fileNameList << QFileInfo(overlayList.at(i)).fileName();
		}
		QString msg = QString("存在同名文件，是否覆盖？\n%1").arg(fileNameList.join("\n"));
		QMessageBox box(QMessageBox::Information, "提示", msg);
		box.setStandardButtons(QMessageBox::Yes | QMessageBox::Ok | QMessageBox::Cancel);
		box.setButtonText(QMessageBox::Yes, "跳过文件");
		box.setButtonText(QMessageBox::Ok, "覆盖文件");
		box.setButtonText(QMessageBox::Cancel, "取消");
		int rst = box.exec();
		if (rst == QMessageBox::Yes)
		{

		}
		else if (rst == QMessageBox::Ok)
		{
			//g_pInterface->iDataCreateProgress("文件覆盖中...");
			for (int i = 0; i < overlayList.size(); ++i)
			{
				//g_pInterface->iDataSetProgressValue(i, overlayList.size());
				QString from = overlayList.at(i);
				QString to = vectorPath() + "/" + QFileInfo(from).fileName();
				if (!MSCopyFileOrDir(from, to))
				{
					failedList << QFileInfo(from).fileName();
				}
			}
			//g_pInterface->iDataCloseProgress();
		}
		else
		{
			return;
		}
	}
	if (newlist.size() > 0)
	{
		//g_pInterface->iDataCreateProgress("文件拷贝中...");
		for (int i = 0; i < newlist.size(); ++i)
		{
			//g_pInterface->iDataSetProgressValue(i, newlist.size());
			QString from = newlist.at(i);
			QString to = vectorPath() + "/" + QFileInfo(from).fileName();
			MSCopyFileOrDir(from, to);
		}
		//g_pInterface->iDataCloseProgress();
	}

	getProjectFiles();

	if (failedList.size() > 0)
	{
		QString msg = QString("以下文件覆盖失败：\n%1").arg(failedList.join("\n"));
		QMessageBox::information(NULL, "提示", msg, QMessageBox::Ok, QMessageBox::Ok);
	}

}

void CMarineProject::deleteVectors(const QStringList& fileNameArr)
{
	QStringList failList;
	for (int i = 0; i < fileNameArr.size(); ++i)
	{
		QString filepath = vectorPath() + "/" + fileNameArr.at(i);
		if (!MSDelFileOrDir(filepath))
		{
			failList << fileNameArr.at(i);
		}
	}
	getProjectFiles();
	if (failList.size() == 0)
	{
		//QMessageBox::information(NULL, "提示", "删除成功！", QMessageBox::Ok, QMessageBox::Ok);
	}
	else
	{
		QString msg = QString("以下文件删除失败：\n%1").arg(failList.join("\n"));
		QMessageBox::information(NULL, "提示", msg, QMessageBox::Ok, QMessageBox::Ok);
	}
}

void CMarineProject::addRasters(const QStringList& rasterPathArr)
{
	QStringList newlist = rasterPathArr;	//需要新增的文件列表
	QStringList overlayList;	//需要覆盖的文件列表
	QStringList failedList;		//覆盖失败文件列表
	for (int i = rasterPathArr.size() - 1; i >= 0; --i)
	{
		QString file = rasterPathArr.at(i);
		if (isFileInList(file, m_rasterFiles, true))
		{
			overlayList.insert(0, file);
			newlist.removeAt(i);
		}
	}
	if (overlayList.size() > 0)
	{
		QStringList fileNameList;
		for (int i = 0; i < overlayList.size(); ++i)
		{
			fileNameList << QFileInfo(overlayList.at(i)).fileName();
		}
		QString msg = QString("存在同名文件，是否覆盖？\n%1").arg(fileNameList.join("\n"));
		QMessageBox box(QMessageBox::Information, "提示", msg);
		box.setStandardButtons(QMessageBox::Yes | QMessageBox::Ok | QMessageBox::Cancel);
		box.setButtonText(QMessageBox::Yes, "跳过文件");
		box.setButtonText(QMessageBox::Ok, "覆盖文件");
		box.setButtonText(QMessageBox::Cancel, "取消");
		int rst = box.exec();
		if (rst == QMessageBox::Yes)
		{

		}
		else if (rst == QMessageBox::Ok)
		{
			//g_pInterface->iDataCreateProgress("文件覆盖中...");
			for (int i = 0; i < overlayList.size(); ++i)
			{
				//g_pInterface->iDataSetProgressValue(i, overlayList.size());
				QString from = overlayList.at(i);
				QString to = rasterPath() + "/" + QFileInfo(from).fileName();
				if (!MSCopyFileOrDir(from, to))
				{
					failedList << QFileInfo(from).fileName();
				}
			}
			//g_pInterface->iDataCloseProgress();
		}
		else
		{
			return;
		}
	}

	AddFilesToMarinePrj(m_project, MSFileEnum::RasterFile, newlist);
	getProjectFiles();

	if (failedList.size() > 0)
	{
		QString msg = QString("以下文件覆盖失败：\n%1").arg(failedList.join("\n"));
		QMessageBox::information(NULL, "提示", msg, QMessageBox::Ok, QMessageBox::Ok);
	}
}

void CMarineProject::deleteRasters(const QStringList& rasterNOArr)
{
	bool flag = DeleteFilesFromMarinePrj(m_project, MSFileEnum::RasterFile, rasterNOArr);
	if (!flag)
	{
		QMessageBox::information(NULL, "提示", "删除文件出错！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	getProjectFiles();
}

void CMarineProject::addRasters(const QString& dirpath)
{
	//目标路径与源路径不能互相包含
	QString topath = GetAbsolutDirPath(m_project, MSFileEnum::RasterFile);
	if (topath.contains(dirpath, Qt::CaseInsensitive)
		|| dirpath.contains(topath, Qt::CaseInsensitive))
	{
		QMessageBox::information(NULL, "提示", "目标路径与源路径不能互相包含！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	if (QMessageBox::Yes != QMessageBox::information(NULL, "提示", QString("确认清空'栅格数据'文件夹并导入新文件？"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
	{
		return;
	}
	bool flag = DeleteFilesFromMarinePrj(m_project, MSFileEnum::RasterFile, QStringList());
	if (!flag)
	{
		QMessageBox::information(NULL, "提示", "删除文件出错！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	AddDirToMarinePrj(m_project, MSFileEnum::RasterFile, dirpath);
	getProjectFiles();
}

void CMarineProject::addSurveys(const QStringList& filePathArr, const MSurveyMethod& method)
{
	AddFilesToMarinePrj(m_project, MSFileEnum::SurveyFile, filePathArr, method);
	getProjectFiles();
}

void CMarineProject::deleteSurveys(const QStringList& fileNOArr, const MSurveyMethod& method)
{
	bool flag = DeleteFilesFromMarinePrj(m_project, MSFileEnum::SurveyFile, fileNOArr, method);
	if (!flag)
	{
		QMessageBox::information(NULL, "提示", "删除出错，可能存在文件被占用！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	getProjectFiles();
}

void CMarineProject::addSurveys(const QString& dirpath, const MSurveyMethod& method)
{
	//目标路径与源路径不能互相包含
	QString topath = GetAbsolutDirPath(m_project, MSFileEnum::SurveyFile, method);
	if (topath.contains(dirpath, Qt::CaseInsensitive)
		|| dirpath.contains(topath, Qt::CaseInsensitive))
	{
		QMessageBox::information(NULL, "提示", "目标路径与源路径不能互相包含！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	QString relapath = GetRelativeDirPath(MSFileEnum::SurveyFile, method);
	if (QMessageBox::Yes != QMessageBox::information(NULL, "提示", QString("确认清空'%1'文件夹并导入新文件？").arg(relapath),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
	{
		return;
	}
	bool flag = DeleteFilesFromMarinePrj(m_project, MSFileEnum::SurveyFile, QStringList(), method);
	if (!flag)
	{
		QMessageBox::information(NULL, "提示", "清空失败，可能存在文件被占用！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	AddDirToMarinePrj(m_project, MSFileEnum::SurveyFile, dirpath, method);
	getProjectFiles();
}

void CMarineProject::addArchives(const QStringList& filePathArr)
{
	AddFilesToMarinePrj(m_project, MSFileEnum::ArchiveFile, filePathArr);
	getProjectFiles();
}

void CMarineProject::deleteArchives(const QStringList& fileNOArr)
{
	bool flag = DeleteFilesFromMarinePrj(m_project, MSFileEnum::ArchiveFile, fileNOArr);
	if (!flag)
	{
		QMessageBox::information(NULL, "提示", "删除文件出错！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	getProjectFiles();
}

void CMarineProject::addArchives(const QString& dirpath)
{
	//目标路径与源路径不能互相包含
	QString topath = GetAbsolutDirPath(m_project, MSFileEnum::ArchiveFile);
	if (topath.contains(dirpath, Qt::CaseInsensitive)
		|| dirpath.contains(topath, Qt::CaseInsensitive))
	{
		QMessageBox::information(NULL, "提示", "目标路径与源路径不能互相包含！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	if (QMessageBox::Yes != QMessageBox::information(NULL, "提示", QString("确认清空'档案文件'文件夹并导入新文件？"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
	{
		return;
	}
	bool flag = DeleteFilesFromMarinePrj(m_project, MSFileEnum::ArchiveFile, QStringList());
	if (!flag)
	{
		QMessageBox::information(NULL, "提示", "清空失败，可能存在文件被占用！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	AddDirToMarinePrj(m_project, MSFileEnum::ArchiveFile, dirpath);
	getProjectFiles();
}

void CMarineProject::updateMSFile(const QString& fileNO, const QString& newName, const bool& isOpen/* = false*/)
{
	if (m_rasterFiles.find(fileNO) != m_rasterFiles.end())
	{
		m_rasterFiles[fileNO]._newName = newName;
		m_rasterFiles[fileNO]._isOpen = isOpen;
		//如果fileNO存在则会更新，不存在则会插入
		m_updateFiles[fileNO] = m_rasterFiles[fileNO];
	}
	if (m_surveyFiles.find(fileNO) != m_surveyFiles.end())
	{
		m_surveyFiles[fileNO]._newName = newName;
		m_updateFiles[fileNO] = m_surveyFiles[fileNO];
	}
	if (m_archiveFiles.find(fileNO) != m_archiveFiles.end())
	{
		m_archiveFiles[fileNO]._newName = newName;
		m_updateFiles[fileNO] = m_archiveFiles[fileNO];
	}
}

bool CMarineProject::commitMSFile()
{
	bool flag = UpdateFilesInMarinePrj(m_project, m_updateFiles);
	m_updateFiles.clear();
	return flag;
}

QStringList CMarineProject::getExtraFiles(const MSFileEnum& ftype, const MSurveyMethod& method /*= MSUnknown*/)
{
	QStringList extraList;
	if (ftype == MSFileEnum::VectorFile)
	{
		QStringList flist = MSGetFileList(vectorPath());
		for (int i = 0; i < flist.size(); ++i)
		{
			QString fpath = flist.at(i);
			if (!isFileInList(fpath, m_vectorFiles, false))
			{
				extraList << fpath.mid(projectPath().size() + 1);
			}
		}
	}
	else if (ftype == MSFileEnum::RasterFile)
	{
		QStringList flist = MSGetFileList(rasterPath());
		for (int i = 0; i < flist.size(); ++i)
		{
			QString fpath = flist.at(i);
			if (!isFileInList(fpath, m_rasterFiles, false)
				&& fpath.compare(rasterMDPath(), Qt::CaseInsensitive) != 0)
			{
				extraList << fpath.mid(projectPath().size() + 1);
			}
		}
	}
	else if (ftype == MSFileEnum::SurveyFile)
	{
		QStringList flist = MSGetFileList(surveyPath(method));
		for (int i = 0; i < flist.size(); ++i)
		{
			QString fpath = flist.at(i);
			if (!isFileInList(fpath, m_surveyFiles, false)
				&& fpath.compare(surveyMDPath(), Qt::CaseInsensitive) != 0)
			{
				extraList << fpath.mid(projectPath().size() + 1);
			}
		}
	}
	else if (ftype == MSFileEnum::ArchiveFile)
	{
		QStringList flist = MSGetFileList(archivePath());
		for (int i = 0; i < flist.size(); ++i)
		{
			QString fpath = flist.at(i);
			if (!isFileInList(fpath, m_archiveFiles, false)
				&& fpath.compare(archiveMDPath(), Qt::CaseInsensitive) != 0)
			{
				extraList << fpath.mid(projectPath().size() + 1);
			}
		}
	}
	return extraList;
}
