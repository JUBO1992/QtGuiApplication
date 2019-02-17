#include "marinebase.h"
#include <QDir>
#include <QSqlDatabase>
#include <QSqlField>
#include <QSqlError>
#include <QSqlQuery>
#include "marineproject.h"

#pragma execution_character_set("utf-8")


QString MarinePrjStruct::projectPath() const
{
	return MS_MARINEROOTDIR + "/" + _prjID + _prjName;
}

QString MarinePrjStruct::vectorPath() const
{
	return projectPath() + "/" + MS_VECTORDIR_NAME;
}

QString MarinePrjStruct::rasterPath() const
{
	return projectPath() + "/" + MS_RASTERDIR_NAME;
}

QString MarinePrjStruct::surveyPath(const MSurveyMethod& method /*= MSurveyMethod::MSUnknown*/) const
{
	QString relapath = GetRelativeDirPath(MSFileEnum::SurveyFile, method);
	return projectPath() + "/" + relapath;
}

QString MarinePrjStruct::archivePath() const
{
	return projectPath() + "/" + MS_ARCHIVEDIR_NAME;
}

QString MarinePrjStruct::rasterMDPath() const
{
	return rasterPath() + "/" + MS_RASTERMD_NAME;
}

QString MarinePrjStruct::surveyMDPath() const
{
	return surveyPath() + "/" + MS_SURVEYMD_NAME;
}

QString MarinePrjStruct::archiveMDPath() const
{
	return archivePath() + "/" + MS_ARCHIVEMD_NAME;
}


QString MarinePrjStruct::idataPrjPath() const
{
	return projectPath() + "/" + _prjID + ".idata";
}

QString MSFileStruct::absolutePath() const
{
	if (_relativePath.isEmpty())
	{
		return QString();
	}
	return MS_MARINEROOTDIR + "/" + _relativePath;
}

QString MSFileStruct::fileName() const
{
	return QFileInfo(absolutePath()).fileName();
}

QString GetRelativeDirPath(const MSFileEnum& type, const MSurveyMethod& method /*= MSurveyMethod::MSUnknown*/)
{
	QString relapath;
	if (type == MSFileEnum::VectorFile)
	{
		relapath = MS_VECTORDIR_NAME;
	}
	else if (type == MSFileEnum::RasterFile)
	{
		relapath = MS_RASTERDIR_NAME;
	}
	else if (type == MSFileEnum::SurveyFile)
	{
		if (method == MSurveyMethod::ADCP)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_ADCP;
		}
		else if (method == MSurveyMethod::GPS)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_GPS;
		}
		else if (method == MSurveyMethod::SedimentSampler)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_SEDSAMPLER;
		}
		else if (method == MSurveyMethod::SideSonar)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_SIDESONAR;
		}
		else if (method == MSurveyMethod::Magnetometer)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_MAGNETOMETER;
		}
		else if (method == MSurveyMethod::SingleBeam)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_SINGLEBEAM;
		}
		else if (method == MSurveyMethod::MultiBeam)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_MULTIBEAM;
		}
		else if (method == MSurveyMethod::SoundVelocimeter)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_SOUNDVMETER;
		}
		else if (method == MSurveyMethod::TideGuage)
		{
			relapath = MS_SURVEYDIR_NAME + "/" + MS_METHOD_TIDEGUAGE;
		}
		else
		{
			relapath = MS_SURVEYDIR_NAME;
		}
	}
	else if (type == MSFileEnum::ArchiveFile)
	{
		relapath = MS_ARCHIVEDIR_NAME;
	}
	return relapath;
}

bool MSDelFileOrDir(const QString &path)
{
	QFileInfo info(path);
	if (!info.exists())
	{
		return true;
	}
	// if file
	if (info.isFile())
	{
		return QFile::remove(path);
	}
	// if dir
	QDir dir(path);
	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
	QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
	foreach(QFileInfo file, fileList)
	{ //遍历文件信息
		if (file.isFile())
		{ // 是文件，删除
			if (!file.dir().remove(file.fileName()))
			{// 删除失败则返回false
				return false;
			}
		}
		else
		{ // 递归删除
			MSDelFileOrDir(file.filePath());
		}
	}
	bool isSuccess = QFileInfo(path).dir().rmdir(dir.dirName()); // 删除本级文件夹
	return isSuccess;
}

bool MSCopyFileOrDir(const QString &from, const QString &to, bool overlay /*= true*/)
{
	if (from.compare(to,Qt::CaseInsensitive) == 0)
	{
		return true;
	}
	QFileInfo fromInfo(from);
	QFileInfo toInfo(to);
	if (!fromInfo.exists())
	{
		return false;
	}
	// if file
	if (fromInfo.isFile())
	{
		if (overlay && toInfo.isFile())
		{
			QFile::remove(to);
		}
		return QFile::copy(from, to);
	}
	//if dir
	QDir sourceDir(from);
	QDir targetDir(to);
	if (to.contains(from, Qt::CaseInsensitive))
	{
		//g_pInterface->iDataPrintf("目标路径不能包含源路径！");
		MSMsgPrintf("目标路径不能包含源路径！");
		return false;
	}
	if (!targetDir.exists())
	{
		if (!targetDir.mkpath(to))
			return false;
	}
	sourceDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		if (fileInfo.isDir())
		{    
			/* 当为目录时，递归的进行copy */
			if (!MSCopyFileOrDir(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()), overlay))
			{
				return false;
			}
		}
		else
		{   
			/* 当允许覆盖操作时，将旧文件进行删除操作 */
			if (overlay && targetDir.exists(fileInfo.fileName()))
			{
				targetDir.remove(fileInfo.fileName());
			}
			// 进行文件copy  
			if (!QFile::copy(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName())))
			{
				return false;
			}
		}
	}
	return true;
}

QStringList MSGetFileList(const QString &path)
{
	QStringList list;
	QFileInfo info(path);
	if (info.isFile())
	{
		return QStringList();
	}
	QDir dir(path);
	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
	QFileInfoList fileInfoList = dir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		if (fileInfo.isDir())
		{
			list << MSGetFileList(fileInfo.filePath());
		}
		else
		{
			list << fileInfo.filePath();
		}
	}
	return list;
}

QString GetRelativeDirPath(const MarinePrjStruct& marine, const MSFileEnum& type, const MSurveyMethod& method /*= MSurveyMethod::MSUnknown*/)
{
	QString relapath = GetRelativeDirPath(type, method);
	if (relapath.isEmpty())
	{
		return QString();
	}
	return marine._prjID + marine._prjName + "/" + relapath;
}

QString GetAbsolutDirPath(const MarinePrjStruct& marine, const MSFileEnum& type, const MSurveyMethod& method /*= MSurveyMethod::MSUnknown*/)
{
	QString relapath = GetRelativeDirPath(marine, type, method);
	if (relapath.isEmpty())
	{
		return QString();
	}
	return MS_MARINEROOTDIR + "/" + relapath;
}

QVariant MSGetProperty(const QString& name)
{
	QVariant value;
	QSqlDatabase database = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	bool isNewOpen = false;
	if (!database.isOpen())
	{
		isNewOpen = true;
		database = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		database.setDatabaseName(MS_PROJECTMENU_PATH);
		database.open();
	}
	if (database.isOpen())
	{
		QSqlQuery query(database);
		QString query_sql = QString("select [Name],[type],[SYSVALUE],[USE] from [PROVALUE] where [Name]='%1'").arg(name);
		if (query.exec(query_sql))
		{
			if (query.next())
			{
				int type = query.value(1).toInt();
				QString v = query.value(2).toString();
				switch (type)
				{
				case QVariant::Int:
					value = v.toInt();
					break;
				case QVariant::Double:
					value = v.toDouble();
					break;
				case QVariant::Char:
				case QVariant::String:
					value = v;
					break;
				default:
					value = v;
					break;
				}
			}
		}
		if (isNewOpen)
		{
			database.close();
		}
	}
	return value;
}

bool MSSetProperty(const QString& name, const QVariant& value)
{
	bool isSuccess = false;
	QSqlDatabase database = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	bool isNewOpen = false;
	if (!database.isOpen())
	{
		isNewOpen = true;
		database = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		database.setDatabaseName(MS_PROJECTMENU_PATH);
		database.open();
	}
	if (database.isOpen())
	{
		QSqlQuery query(database);
		QString str_sql = QString("select [Name],[type],[SYSVALUE],[USE] from [PROVALUE] where [Name]='%1'").arg(name);
		if (query.exec(str_sql))
		{
			int type = 0;
			QString vstr = value.toString();
			switch (value.type())
			{
			case QVariant::Int:
				type = QVariant::Int;
				break;
			case QVariant::Double:
				type = QVariant::Double;
				break;
			case QVariant::Char:
			case QVariant::String:
				type = QVariant::String;
			default:
				type = QVariant::String;
				break;
			}
			if (query.next())
			{
				str_sql = QString("update [PROVALUE] set [Name]=?,[type]=?,[SYSVALUE]=? where [Name]='%1'").arg(name);
			}
			else
			{
				str_sql = QString("insert into [PROVALUE] ([Name],[type],[SYSVALUE]) values (?,?,?)");
			}
			query.clear();
			query.prepare(str_sql);
			query.addBindValue(name);
			query.addBindValue(type);
			query.addBindValue(vstr);
			if (query.exec())
			{
				isSuccess = true;
			}
			else
			{
				QString errmsg = database.lastError().text();
				//g_pInterface->iDataPrintf(errmsg);
				MSMsgPrintf(errmsg);
			}
		}
		if (isNewOpen)
		{
			database.close();
		}
	}
	return isSuccess;
}

