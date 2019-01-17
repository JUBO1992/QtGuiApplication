#include "marinebase.h"
//#include "idatainterface.h"
#include <QSqlDatabase>
#include <QDir>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "marineproject.h"

#pragma execution_character_set("utf-8")

//extern iDataInterface* g_pInterface;

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

void ReadProjectMenu(QList<MarinePrjStruct>& prjlist)
{
	QMap<QString, MarinePrjStruct> prjMap;
	ReadProjectMenu(prjMap);
	prjlist << prjMap.values();
}

void ReadProjectMenu(QMap<QString, MarinePrjStruct>& prjMap)
{
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
		QStringList fields;
		fields << "PROJECTNO" << "PROJECTNAME" << "PROJECTTYPE" << "PROJECTENGINEER" << "SURVEYTEAM" << "SURVEYPROJECT" << "SURVEYCOOR" << "SURVEYTYPE";
		QString query_sql = QString("SELECT " + fields.join(",") + " FROM [%1]").arg(MS_PROJECTMENU_TABLE);
		if (query.exec(query_sql))
		{
			while (query.next())
			{
				MarinePrjStruct marine;
				marine._prjID = query.value(0).toString();
				marine._prjName = query.value(1).toString();
				marine._prjType = query.value(2).toString();
				marine._prjEngineer = query.value(3).toString();
				marine._surveyTeam = query.value(4).toString();
				marine._surveyPrj = query.value(5).toString();
				marine._surveyCoor = query.value(6).toString();
				marine._surveyType = query.value(7).toString();
				prjMap.insert(marine._prjID, marine);
			}
		}
		if (isNewOpen)
		{
			database.close();
		}
	}
}

void UpdateProjectMenu(QMap<QString, MarinePrjStruct>& prjMap)
{
	prjMap.clear();
	ReadProjectMenu(prjMap);
}

bool DelFileOrDir(const QString &path)
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
			DelFileOrDir(file.filePath());
		}
	}
	//这里似乎会返回false why???
	return dir.rmpath(dir.path()); // 删除文件夹
}

bool CopyFileOrDir(const QString &from, const QString &to, bool overlay /*= true*/)
{
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
	if (from.contains(to, Qt::CaseInsensitive))
	{
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
			if (!CopyFileOrDir(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()), overlay))
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

QStringList GetFileList(const QString &path)
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
			list << GetFileList(fileInfo.filePath());
		}
		else
		{
			list << fileInfo.filePath();
		}
	}
	return list;
}

bool CreateMarinePrj(const MarinePrjStruct& marine)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return false;
	}
	bool isSuccess = true;

	QSqlDatabase database = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	bool isNewOpen = false;
	if (!database.isOpen())
	{
		isNewOpen = true;
		database = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		database.setDatabaseName(MS_PROJECTMENU_PATH);
		database.open();
	}
	QStringList insert_keys;
	insert_keys << "PROJECTNO" << "PROJECTNAME" << "PROJECTTYPE" << "PROJECTENGINEER" << "SURVEYTEAM" << "SURVEYPROJECT" << "SURVEYCOOR" << "SURVEYTYPE";
	if (database.isOpen())
	{
		QSqlQuery query(database);
		QString str_sql;
		str_sql = QString("INSERT INTO [%1] (" + insert_keys.join(",") + ") VALUES (?,?,?,?,?,?,?,?) ").arg(MS_PROJECTMENU_TABLE);
		query.prepare(str_sql);
		query.addBindValue(marine._prjID);
		query.addBindValue(marine._prjName);
		query.addBindValue(marine._prjType);
		query.addBindValue(marine._prjEngineer);
		query.addBindValue(marine._surveyTeam);
		query.addBindValue(marine._surveyPrj);
		query.addBindValue(marine._surveyCoor);
		query.addBindValue(marine._surveyType);
		if (!query.exec())
		{
			QString errmsg = database.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
			isSuccess = false;
		}
		if (isNewOpen)
		{
			database.close();
		}
	}
	if (!isSuccess)
	{
		return false;
	}

	QString rootPath = MS_MARINEROOTDIR;
	QDir rootDir(rootPath);
	if (!rootDir.exists())
	{
		rootDir.mkpath(rootPath);
	}
	rootDir.mkdir(marine._prjID + marine._prjName);
	//新建数据目录
	QString prjPath = marine.projectPath();
	QDir prjDir(prjPath);
	prjDir.mkdir(MS_VECTORDIR_NAME);
	prjDir.mkdir(MS_RASTERDIR_NAME);
	prjDir.mkdir(MS_SURVEYDIR_NAME);
	prjDir.mkdir(MS_ARCHIVEDIR_NAME);
	QString surveyPath = marine.surveyPath();
	QDir surveyDir(surveyPath);
	//以下文件夹是否需要在新建工程是创建
	//surveyDir.mkdir(MS_METHOD_ADCP);
	//surveyDir.mkdir(MS_METHOD_GPS);
	//surveyDir.mkdir(MS_METHOD_SEDSAMPLER);
	//surveyDir.mkdir(MS_METHOD_SIDESONAR);
	//surveyDir.mkdir(MS_METHOD_MAGNETOMETER);
	//surveyDir.mkdir(MS_METHOD_SINGLEBEAM);
	//surveyDir.mkdir(MS_METHOD_MULTIBEAM);
	//surveyDir.mkdir(MS_METHOD_SOUNDVMETER);
	//surveyDir.mkdir(MS_METHOD_TIDEGUAGE);
	//拷贝元数据模板
	CopyFileOrDir(rootPath + "/" + MS_RASTERMD_NAME, marine.rasterPath() + "/" + MS_RASTERMD_NAME);
	CopyFileOrDir(rootPath + "/" + MS_SURVEYMD_NAME, marine.surveyPath() + "/" + MS_SURVEYMD_NAME);
	CopyFileOrDir(rootPath + "/" + MS_ARCHIVEMD_NAME, marine.archivePath() + "/" + MS_ARCHIVEMD_NAME);

	return true;
}

bool DeleteMarinePrj(const MarinePrjStruct& marine)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return false;
	}
	QString path = marine.projectPath();
	bool flag = true;
	if (QDir(path).exists())
	{
		//如果重命名成功说明说明文件夹没有被占用，理论上应该可以成功移除
		if (QDir().rename(path, path + "abcdefg"))
		{
			QDir().rename(path + "abcdefg", path);
			DelFileOrDir(path);
		}
		else
		{
			flag = false;
		}
	}
	if (!flag)
	{
		return false;
	}
	//下面删除ProjectMenu中相关信息
	bool isSuccess = true;
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
		QString str_sql;
		str_sql = QString("delete from [%1] where %2 like '%3'")
			.arg(MS_INDEX_TIFF_TABLE).arg(MS_CHARTNO).arg(marine._prjID + MS_R + "______");
		query.prepare(str_sql);
		if (!query.exec())
		{
			QString errmsg = database.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}
		str_sql = QString("delete from [%1] where %2 like '%3'")
			.arg(MS_INDEX_DATA_TABLE).arg(MS_DATANO).arg(marine._prjID + MS_D + "______");
		query.prepare(str_sql);
		if (!query.exec())
		{
			QString errmsg = database.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}
		str_sql = QString("delete from [%1] where %2 like '%3'")
			.arg(MS_INDEX_FILE_TABLE).arg(MS_FILENO).arg(marine._prjID + MS_F + "______");
		query.prepare(str_sql);
		if (!query.exec())
		{
			QString errmsg = database.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}
		str_sql = QString("delete from [%1] where %2='%3'")
			.arg(MS_PROJECTMENU_TABLE).arg("PROJECTNO").arg(marine._prjID);
		query.prepare(str_sql);
		if (!query.exec())
		{
			QString errmsg = database.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}		if (isNewOpen)
		{
			database.close();
		}
	}
	return true;
}

bool OpenMarinePrj(const MarinePrjStruct& marine)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return false;
	}
	//调用命令打开相应矢量及栅格文件
	CMarineProject project(marine);
	QMap<QString, MSFileStruct> vectorFiles;
	project.getVectorList(vectorFiles);
	if (vectorFiles.size() == 0)
	{
		//g_pInterface->iDataPrintf("该项目下没有矢量数据文件！");
		return false;
	}
	QString mdbpath = QString();
	for (auto it = vectorFiles.begin(); it != vectorFiles.end(); ++it)
	{
		QString filepath = it.value().absolutePath();
		if (QFileInfo(filepath).fileName().contains(marine._prjID))
		{
			mdbpath = filepath;
			break;
		}
	}
	if (mdbpath.isEmpty())
	{
		mdbpath = vectorFiles.begin().value().absolutePath();
	}
	QString templatepath = MS_TEMPLATE_NAME;
	bool isSuccess = false;//g_pInterface->OpenDoc(mdbpath, false, templatepath, 1);
	if (isSuccess)
	{
		//这里加载影像
		QMap<QString, MSFileStruct> rasterFiles;
		project.getRasterList(rasterFiles);
		for (auto it = rasterFiles.begin(); it != rasterFiles.end(); ++it)
		{
			if (it->_isOpen)
			{
				//g_pInterface->iDataAppendImage(it->absolutePath(), QPointF());
			}
		}
	}

	return isSuccess;
}

bool CloseMarinePrj(const MarinePrjStruct& marine)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return false;
	}
	//判断当前项目是否打开，若果打开则关闭打开的项目文件，释放文件资源
	return true;
}

//更新表的prjid，在项目文件夹重命名成功后调用才有效
bool updateTablePrjID(const MarinePrjStruct& marine, const QString& marinePrjID, const MSFileEnum& type)
{
	bool isNewOpen = false;
	QString MDpath;		//元数据库路径
	QString MDTable;	//元数据表名
	QString NOField;	//编号字段名称
	QString mdMark;		//元数据类型标记，R(栅格)D(测量)F(档案)
	QString indexTable;	//index表名
	if (type == MSFileEnum::RasterFile)
	{ 
		MDpath = marine.rasterMDPath();
		MDTable = MS_RASTERMD_TABLE;
		NOField = MS_CHARTNO;
		mdMark = MS_R;
		indexTable = MS_INDEX_TIFF_TABLE;
	}
	else if (type == MSFileEnum::SurveyFile)
	{
		MDpath = marine.surveyMDPath();
		MDTable = MS_SURVEYMD_TABLE;
		NOField = MS_DATANO;
		mdMark = MS_D;
		indexTable = MS_INDEX_DATA_TABLE;
	}
	else if (type == MSFileEnum::ArchiveFile)
	{
		MDpath = marine.archiveMDPath();
		MDTable = MS_ARCHIVEMD_TABLE;
		NOField = MS_FILENO;
		mdMark = MS_F;
		indexTable = MS_INDEX_FILE_TABLE;
	}
	else
	{
		return false;
	}
	QSqlDatabase MDdb = QSqlDatabase::database(MDpath, false);
	if (!MDdb.isOpen())
	{
		isNewOpen = true;
		MDdb = QSqlDatabase::addDatabase("QSQLITE", MDpath);
		MDdb.setDatabaseName(MDpath);
		MDdb.open();
	}
	if (MDdb.isOpen())
	{
		QString str_sql = QString("select [%1] from [%2]").arg(NOField).arg(MDTable);
		QSqlQuery query(MDdb);
		if (query.exec(str_sql))
		{
			MDdb.transaction();
			QSqlQuery update_query(MDdb);
			while (query.next())
			{
				QString oldNO = query.value(0).toString();
				QString newNO = marine._prjID + oldNO.mid(oldNO.lastIndexOf(mdMark));
				str_sql = QString("update [%1] set [%2]='%3' where %4='%5'")
					.arg(MDTable).arg(NOField).arg(newNO).arg(NOField).arg(oldNO);
				update_query.prepare(str_sql);
				if (!update_query.exec())
				{
					QString errmsg = MDdb.lastError().text();
					//g_pInterface->iDataPrintf(errmsg);
				}
			}
			MDdb.commit();
		}
		else
		{
			QString errmsg = MDdb.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}
		if (isNewOpen)
		{
			MDdb.close();
		}
	}
	isNewOpen = false;
	QSqlDatabase prjMenuDb = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	if (!prjMenuDb.isOpen())
	{
		isNewOpen = true;
		prjMenuDb = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		prjMenuDb.setDatabaseName(MS_PROJECTMENU_PATH);
		prjMenuDb.open();
	}
	if (prjMenuDb.isOpen())
	{
		QString str_sql = QString("select [%1],[%2] from [%3] where %4 like '%5'")
			.arg(NOField).arg("INDEX").arg(indexTable).arg(NOField).arg(marinePrjID+mdMark+"______");
		QSqlQuery query(prjMenuDb);
		if (query.exec(str_sql))
		{
			prjMenuDb.transaction();
			QSqlQuery update_query(prjMenuDb);
			while (query.next())
			{
				QString oldNO = query.value(0).toString();
				QString newNO = marine._prjID + oldNO.mid(oldNO.lastIndexOf(mdMark));
				QString oldIndex = query.value(1).toString();
				QString newIndex = marine._prjID + marine._prjName;
				if (oldIndex.indexOf("/") > 0)
				{
					newIndex += oldIndex.mid(oldIndex.indexOf("/"));
				}
				str_sql = QString("update [%1] set [%2]='%3',[%4]='%5' where %6='%7'")
					.arg(indexTable).arg(NOField).arg(newNO).arg("INDEX").arg(newIndex).arg(NOField).arg(oldNO);
				update_query.prepare(str_sql);
				if (!update_query.exec())
				{
					QString errmsg = prjMenuDb.lastError().text();
					//g_pInterface->iDataPrintf(errmsg);
				}
			}
			prjMenuDb.commit();
		}
		else
		{
			QString errmsg = prjMenuDb.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}
		if (isNewOpen)
		{
			prjMenuDb.close();
		}
	}

	return true;
}

bool UpdateMarinePrj(const MarinePrjStruct& marine, const QString& marinePrjID)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return false;
	}
	QString marinePrjName;	//项目名称
	QMap<QString, MarinePrjStruct> projectMap;
	ReadProjectMenu(projectMap);
	auto it = projectMap.find(marinePrjID);
	if (it != projectMap.end())
	{
		marinePrjName = it.value()._prjName;
	}
	else
	{
		//g_pInterface->iDataPrintf("待更新项目不存在！");
		return false;
	}

	//项目编号或项目名称发生改变需要更新项目文件夹的名称
	if (marine._prjID != marinePrjID || marine._prjName != marinePrjName)
	{
		QString oldname = it.value().projectPath();
		QString newname = marine.projectPath();
		bool flag = QDir().rename(oldname, newname);
		if (!flag)
		{
			//如果文件夹重命名失败则直接返回，不进行数据库的更新
			//后续操作必须在文件夹重命名成功的基础上进行
			return false;
		}
	}

	bool isSuccess = true;
	QSqlDatabase database = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	bool isNewOpen = false;
	if (!database.isOpen())
	{
		isNewOpen = true;
		database = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		database.setDatabaseName(MS_PROJECTMENU_PATH);
		database.open();
	}
	QStringList insert_keys;
	insert_keys << "PROJECTNO" << "PROJECTNAME" << "PROJECTTYPE" << "PROJECTENGINEER" << "SURVEYTEAM" << "SURVEYPROJECT" << "SURVEYCOOR" << "SURVEYTYPE";
	if (database.isOpen())
	{
		QSqlQuery query(database);
		QString str_sql;
		str_sql = QString("UPDATE [%1] SET " + insert_keys.join("=?,") + "=? WHERE PROJECTNO='%2'")
			.arg(MS_PROJECTMENU_TABLE).arg(marinePrjID);
		query.prepare(str_sql);
		query.addBindValue(marine._prjID);
		query.addBindValue(marine._prjName);
		query.addBindValue(marine._prjType);
		query.addBindValue(marine._prjEngineer);
		query.addBindValue(marine._surveyTeam);
		query.addBindValue(marine._surveyPrj);
		query.addBindValue(marine._surveyCoor);
		query.addBindValue(marine._surveyType);
		if (!query.exec())
		{
			QString errmsg = database.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
			isSuccess = false;
		}
		if (isNewOpen)
		{
			database.close();
		}
	}

	//项目编号发生改变需要更新相关数据库中的记录
	if (marine._prjID != marinePrjID || marine._prjName != marinePrjName)
	{
		updateTablePrjID(marine, marinePrjID, MSFileEnum::RasterFile);
		updateTablePrjID(marine, marinePrjID, MSFileEnum::SurveyFile);
		updateTablePrjID(marine, marinePrjID, MSFileEnum::ArchiveFile);
	}
	return isSuccess;
}

void GetMarinePrjFiles(const MarinePrjStruct& marine, const MSFileEnum& type, QMap<QString, MSFileStruct>& files,
	const MSurveyMethod& method)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return;
	}
	if (type == MSFileEnum::VectorFile)
	{
		//读取 矢量数据 目录下的mdb文件列表
		QString vectorDir = marine.vectorPath();
		QDir dir(vectorDir);
		QStringList vectorFiles;
		vectorFiles = dir.entryList(QStringList() << "*.mdb", QDir::Files | QDir::Readable, QDir::Name);
		for (int i = 0; i < vectorFiles.size(); ++i)
		{
			QString vectorNO = marine._prjID + "V" + QString("%1").arg(i+1, 4, 10, QChar('0'));
			QString relativepath = marine._prjID + marine._prjName + "/" + MS_VECTORDIR_NAME + "/" + vectorFiles.at(i);
			files.insert(vectorNO, MSFileStruct(vectorNO, relativepath));
		}
		return;
	}
	QStringList fileNOList;	//文件编号列表
	bool isNewOpen = false;
	QString MDpath;		//元数据库路径
	QString MDTable;	//元数据表名
	QString NOField;	//编号字段名称
	QString mdMark;		//元数据类型标记，R(栅格)D(测量)F(档案)
	if (type == MSFileEnum::RasterFile)
	{
		MDpath = marine.rasterMDPath();
		MDTable = MS_RASTERMD_TABLE;
		NOField = MS_CHARTNO;
		mdMark = MS_R;
	}
	else if (type == MSFileEnum::SurveyFile)
	{
		MDpath = marine.surveyMDPath();
		MDTable = MS_SURVEYMD_TABLE;
		NOField = MS_DATANO;
		mdMark = MS_D;
	}
	else if (type == MSFileEnum::ArchiveFile)
	{
		MDpath = marine.archiveMDPath();
		MDTable = MS_ARCHIVEMD_TABLE;
		NOField = MS_FILENO;
		mdMark = MS_F;
	}

	QSqlDatabase MDdb = QSqlDatabase::database(MDpath, false);
	if (!MDdb.isOpen())
	{
		isNewOpen = true;
		MDdb = QSqlDatabase::addDatabase("QSQLITE", MDpath);
		MDdb.setDatabaseName(MDpath);
		MDdb.open();
	}
	if (MDdb.isOpen())
	{
		QString str_sql = QString("SELECT %1 FROM [%2]").arg(NOField).arg(MDTable);
		QSqlQuery query(MDdb);
		if (query.exec(str_sql))
		{
			while (query.next())
			{
				QString id = query.value(0).toString();
				if (!id.isEmpty())
				{
					fileNOList << id;
				}
			}
		}
		if (isNewOpen)
		{
			MDdb.close();
		}
	}

	FindFilesInMarinePrj(marine, type, fileNOList, files);
}

//向表添加记录
QStringList addTableRecords(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& files, const QString& dirpath /*= QString()*/,
	const MSurveyMethod& method /*= MSurveyMethod::MSUnknown*/)
{
	QStringList fileNOList;
	bool isNewOpen = true;
	QString MDpath;		//元数据库路径
	QString MDTable;	//元数据表名
	QString NOField;	//编号字段名称
	QString mdMark;		//元数据类型标记，R(栅格)D(测量)F(档案)
	QString indexTable;	//index表名
	if (type == MSFileEnum::RasterFile)
	{
		MDpath = marine.rasterMDPath();
		MDTable = MS_RASTERMD_TABLE;
		NOField = MS_CHARTNO;
		mdMark = MS_R;
		indexTable = MS_INDEX_TIFF_TABLE;
	}
	else if (type == MSFileEnum::SurveyFile)
	{
		MDpath = marine.surveyMDPath();
		MDTable = MS_SURVEYMD_TABLE;
		NOField = MS_DATANO;
		mdMark = MS_D;
		indexTable = MS_INDEX_DATA_TABLE;
	}
	else if (type == MSFileEnum::ArchiveFile)
	{
		MDpath = marine.archiveMDPath();
		MDTable = MS_ARCHIVEMD_TABLE;
		NOField = MS_FILENO;
		mdMark = MS_F;
		indexTable = MS_INDEX_FILE_TABLE;
	}
	else
	{
		return QStringList();
	}
	QSqlDatabase MDdb = QSqlDatabase::database(MDpath, false);
	QSqlRecord record;
	int maxNO = 0;			//当前数据库最大编号
	if (!MDdb.isOpen())
	{
		isNewOpen = true;
		MDdb = QSqlDatabase::addDatabase("QSQLITE", MDpath);
		MDdb.setDatabaseName(MDpath);
		MDdb.open();
		record = MDdb.record(MDTable);
	}
	if (MDdb.isOpen())
	{
		QSqlQuery query(MDdb);
		QString str_sql = QString("select max(%1) from [%2]").arg(NOField).arg(MDTable);
		if (query.exec(str_sql))
		{
			while (query.next())
			{
				QString id = query.value(0).toString();
				int rasterNO = id.mid(id.lastIndexOf(mdMark) + 1).toInt();
				if (rasterNO > maxNO)
				{
					maxNO = rasterNO;
				}
			}
		}
		query.clear();
		QStringList fields;
		QStringList markList;
		int count = record.count();
		for (int i = 0; i < record.count(); ++i)
		{
			fields << record.fieldName(i);
		}
		for (int i = 0; i < fields.size(); ++i)
		{
			markList.append("?");
		}
		QString insert_sql = QString("insert into [%1] (%2) values (%3)")
			.arg(MDTable).arg(fields.join(",")).arg(markList.join(","));
		QSqlQuery insertQuery(MDdb);
		MDdb.transaction();
		for (int i = 0; i < files.size(); ++i)
		{
			maxNO++;
			QString FileNO = marine._prjID + mdMark + QString("%1").arg(maxNO, 6, 10, QChar('0'));
			fileNOList << FileNO;
			insertQuery.prepare(insert_sql);
			for (int j = 0; j < record.count(); ++j)
			{
				QSqlField field = record.field(j);
				QString fieldname = field.name();
				if (fieldname.compare(NOField, Qt::CaseInsensitive) == 0)
				{
					insertQuery.addBindValue(QString(FileNO));
				}
				else
				{
					insertQuery.addBindValue(QString(" "));
				}
			}
			if (!insertQuery.exec())
			{
				QString errmsg = insertQuery.lastError().text();
				//g_pInterface->iDataPrintf(errmsg);
			}
		}
		MDdb.commit();

		if (isNewOpen)
		{
			MDdb.close();
		}
	}

	isNewOpen = false;
	QSqlDatabase prjMenuDb = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	if (!prjMenuDb.isOpen())
	{
		isNewOpen = true;
		prjMenuDb = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		prjMenuDb.setDatabaseName(MS_PROJECTMENU_PATH);
		prjMenuDb.open();
	}
	if (prjMenuDb.isOpen())
	{
		QString insert_sql;
		if (type == MSFileEnum::RasterFile)
		{
			insert_sql = QString("insert into [%1] (%2,[INDEX],ISOPEN) values (?,?,?)").arg(MS_INDEX_TIFF_TABLE).arg(MS_CHARTNO);
		}
		else if (type == MSFileEnum::SurveyFile)
		{
			insert_sql = QString("insert into [%1] (%2,[INDEX]) values (?,?)").arg(MS_INDEX_DATA_TABLE).arg(MS_DATANO);
		}
		else if (type == MSFileEnum::ArchiveFile)
		{
			insert_sql = QString("insert into [%1] (%2,[INDEX]) values (?,?)").arg(MS_INDEX_FILE_TABLE).arg(MS_FILENO);
		}
		QSqlQuery insertQuery(prjMenuDb);
		prjMenuDb.transaction();
		for (int i = 0; i < fileNOList.size(); ++i)
		{
			QString FileNO = fileNOList.at(i);
			QString relativedir = GetRelativeDirPath(marine, type, method);
			QString relativePath; 
			if (dirpath.isEmpty())
			{
				relativePath = relativedir + "/" + QFileInfo(files.at(i)).fileName();
			}
			else
			{
				relativePath = relativedir + files.at(i).mid(dirpath.size());
			}
			QString isOpen = "0";
			if (!insertQuery.prepare(insert_sql))
			{
				QString errmsg = insertQuery.lastError().text();
				//g_pInterface->iDataPrintf(errmsg);
			}
			insertQuery.addBindValue(QString(FileNO));
			insertQuery.addBindValue(QString(relativePath));
			if (type == MSFileEnum::RasterFile)
			{
				insertQuery.addBindValue(QString(isOpen));
			}
			if (!insertQuery.exec())
			{
				QString errmsg = insertQuery.lastError().text();
				//g_pInterface->iDataPrintf(errmsg);
			}
		}
		prjMenuDb.commit();

		if (isNewOpen)
		{
			prjMenuDb.close();
		}
	}
	return fileNOList;
}

QStringList AddFilesToMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& files,
	const MSurveyMethod& method /*= MSurveyMethod::MSUnknown*/)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return QStringList();
	}
	if (type == MSFileEnum::VectorFile || files.isEmpty())
	{
		return QStringList();
	}

	QStringList fileNOList = addTableRecords(marine, type, files, QString(), method);

	//下面拷贝数据文件到相应工程目录下
	QString destDirPath = GetAbsolutDirPath(marine, type, method);
	if (QDir(destDirPath).exists())
	{
		//g_pInterface->iDataCreateProgress("文件导入中...");
		for (int i = 0; i < files.size(); ++i)
		{
			//g_pInterface->iDataSetProgressValue(i, files.size());
			QString destFile = destDirPath + "/" + QFileInfo(files.at(i)).fileName();
			CopyFileOrDir(files.at(i), destFile);
		}
		//g_pInterface->iDataCloseProgress();
	}
	return fileNOList;
}

QStringList AddDirToMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QString& dirpath,
	const MSurveyMethod& method /*= MSurveyMethod::MSUnknown*/)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return QStringList();
	}
	if (type == MSFileEnum::VectorFile)
	{
		return QStringList();
	}

	QStringList files = GetFileList(dirpath);
	if (files.isEmpty())
	{
		return QStringList();
	}

	QStringList fileNOList = addTableRecords(marine, type, files, dirpath, method);

	//下面拷贝数据文件到相应工程目录下
	QString destDirPath = GetAbsolutDirPath(marine, type, method);
	//g_pInterface->iDataCreateProgress("文件导入中...");
	//g_pInterface->progressOperator(1, "文件导入中...");
	//此处暂时加不了进度
	//g_pInterface->iDataSetProgressValue(24, 100);
	//g_pInterface->progressOperator(2, "文件导入中...");
	CopyFileOrDir(dirpath, destDirPath);
	//g_pInterface->iDataCloseProgress();
	//g_pInterface->progressOperator(3, "文件导入中...");

	return fileNOList;
}

void FindFilesInMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& fileNOList, QMap<QString, MSFileStruct>& files,
	const MSurveyMethod& method)
{
	if (type == MSFileEnum::VectorFile)
	{
		return;
	}
	bool isNewOpen = false;
	QSqlDatabase prjMenuDb = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	if (!prjMenuDb.isOpen())
	{
		isNewOpen = true;
		prjMenuDb = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		prjMenuDb.setDatabaseName(MS_PROJECTMENU_PATH);
		prjMenuDb.open();
	}
	if (prjMenuDb.isOpen())
	{
		QSqlQuery query(prjMenuDb);
		QString str_sql;
		if (!fileNOList.isEmpty())
		{
			if (type == MSFileEnum::RasterFile)
			{
				str_sql = QString("select %1,[INDEX],ISOPEN from [%2] where %3 in ('" + fileNOList.join("','") + "')")
					.arg(MS_CHARTNO).arg(MS_INDEX_TIFF_TABLE).arg(MS_CHARTNO);
			}
			else if (type == MSFileEnum::SurveyFile)
			{
				str_sql = QString("select %1,[INDEX] from [%2] where %3 in ('" + fileNOList.join("','") + "')")
					.arg(MS_DATANO).arg(MS_INDEX_DATA_TABLE).arg(MS_DATANO);
			}
			else if (type == MSFileEnum::ArchiveFile)
			{
				str_sql = QString("select %1,[INDEX] from [%2] where %3 in ('" + fileNOList.join("','") + "')")
					.arg(MS_FILENO).arg(MS_INDEX_FILE_TABLE).arg(MS_FILENO);
			}
		}
		else
		{
			if (type == MSFileEnum::RasterFile)
			{
				str_sql = QString("select %1,[INDEX],ISOPEN from [%2] where %3 like '%4'")
					.arg(MS_CHARTNO).arg(MS_INDEX_TIFF_TABLE).arg(MS_CHARTNO).arg(marine._prjID + MS_R + "______");
			}
			else if (type == MSFileEnum::SurveyFile)
			{
				QString relapath = GetRelativeDirPath(marine, type, method);
				str_sql = QString("select %1,[INDEX] from [%2] where %3 like '%4' and [INDEX] like '%5%'")
					.arg(MS_DATANO).arg(MS_INDEX_DATA_TABLE).arg(MS_DATANO).arg(marine._prjID + MS_D + "______").arg(relapath);
			}
			else if (type == MSFileEnum::ArchiveFile)
			{
				str_sql = QString("select %1,[INDEX] from [%2] where %3 like '%4'")
					.arg(MS_FILENO).arg(MS_INDEX_FILE_TABLE).arg(MS_FILENO).arg(marine._prjID + MS_F + "______");
			}
		}
		if (query.exec(str_sql))
		{
			while (query.next())
			{
				QString id = query.value(0).toString();
				QString path = query.value(1).toString();
				bool isopen = false;
				if (type == MSFileEnum::RasterFile)
				{
					isopen = query.value(2).toBool();
				}
				files.insert(id, MSFileStruct(id, path, isopen));
			}
		}
		else
		{
			QString errmsg = query.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}
		if (isNewOpen)
		{
			prjMenuDb.close();
		}
	}
}

bool DeleteFilesFromMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& fileNOList,
	const MSurveyMethod& method)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return false;
	}
	if (type == MSFileEnum::VectorFile)
	{
		return false;
	}

	//先删除文件，如果删除失败直接return false不删除数据库中记录
	bool flag = true;
	QMap<QString, MSFileStruct> files;
	QStringList delList;	//待删除FileNO列表
	FindFilesInMarinePrj(marine, type, fileNOList, files, method);
	if (!fileNOList.isEmpty())
	{
		delList = fileNOList;
		for (auto it = files.begin(); it != files.end(); ++it)
		{
			QString filepath = it.value().absolutePath();
			flag = flag & DelFileOrDir(filepath);
		}
	}
	else
	{
		delList = files.keys();
		//清空path下除.db外的所有内容
		QString path = GetAbsolutDirPath(marine, type, method);
		if (QDir(path).exists())
		{
			QDir dir(path);
			dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
			QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
			foreach(QFileInfo file, fileList)
			{
				if (file.filePath().endsWith(".db", Qt::CaseInsensitive))
				{
					continue;
				}
				flag = flag & DelFileOrDir(file.filePath());
			}
		}
	}
	if (!flag)
	{
		return false;
	}

	bool isNewOpen = false;
	QSqlDatabase prjMenuDb = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	if (!prjMenuDb.isOpen())
	{
		isNewOpen = true;
		prjMenuDb = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		prjMenuDb.setDatabaseName(MS_PROJECTMENU_PATH);
		prjMenuDb.open();
	}
	if (prjMenuDb.isOpen())
	{
		QSqlQuery query(prjMenuDb);
		QString str_sql;
		if (type == MSFileEnum::RasterFile)
		{
			str_sql = QString("delete from [%1] where %2 in ('" + delList.join("','") + "')").arg(MS_INDEX_TIFF_TABLE).arg(MS_CHARTNO);
		}
		else if (type == MSFileEnum::SurveyFile)
		{
			str_sql = QString("delete from [%1] where %2 in ('" + delList.join("','") + "')").arg(MS_INDEX_DATA_TABLE).arg(MS_DATANO);
		}
		else if (type == MSFileEnum::ArchiveFile)
		{
			str_sql = QString("delete from [%1] where %2 in ('" + delList.join("','") + "')").arg(MS_INDEX_FILE_TABLE).arg(MS_FILENO);
		}
		query.prepare(str_sql);
		if (!query.exec())
		{
			QString errmsg = prjMenuDb.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}
		if (isNewOpen)
		{
			prjMenuDb.close();
		}
	}

	QSqlDatabase MDdb;		//元数据库
	isNewOpen = false;
	QString str_sql;
	if (type == MSFileEnum::RasterFile)
	{
		QString rasterMD = marine.rasterMDPath();
		MDdb = QSqlDatabase::database(rasterMD, false);
		if (!MDdb.isOpen())
		{
			isNewOpen = true;
			MDdb = QSqlDatabase::addDatabase("QSQLITE", rasterMD);
			MDdb.setDatabaseName(rasterMD);
			MDdb.open();
		}
		str_sql = QString("delete from [%1] where %2 in ('" + delList.join("','") + "')").arg(MS_RASTERMD_TABLE).arg(MS_CHARTNO);
	}
	else if (type == MSFileEnum::SurveyFile)
	{
		QString surveyMD = marine.surveyMDPath();
		MDdb = QSqlDatabase::database(surveyMD, false);
		if (!MDdb.isOpen())
		{
			isNewOpen = true;
			MDdb = QSqlDatabase::addDatabase("QSQLITE", surveyMD);
			MDdb.setDatabaseName(surveyMD);
			MDdb.open();
		}
		str_sql = QString("delete from [%1] where %2 in ('" + delList.join("','") + "')").arg(MS_SURVEYMD_TABLE).arg(MS_DATANO);
	}
	else if (type == MSFileEnum::ArchiveFile)
	{
		QString archiveMD = marine.archiveMDPath();
		MDdb = QSqlDatabase::database(archiveMD, false);
		if (!MDdb.isOpen())
		{
			isNewOpen = true;
			MDdb = QSqlDatabase::addDatabase("QSQLITE", archiveMD);
			MDdb.setDatabaseName(archiveMD);
			MDdb.open();
		}
		str_sql = QString("delete from [%1] where %2 in ('" + delList.join("','") + "')").arg(MS_ARCHIVEMD_TABLE).arg(MS_FILENO);
	}
	if (MDdb.isOpen())
	{
		QSqlQuery query(MDdb);
		query.prepare(str_sql);
		if (!query.exec())
		{
			QString errmsg = prjMenuDb.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}
		if (isNewOpen)
		{
			MDdb.close();
		}
	}
	return true;
}

bool UpdateFilesInMarinePrj(const MarinePrjStruct& marine, const QMap<QString, MSFileStruct>& files)
{
	if (files.size() == 0)
	{
		return true;
	}
	QStringList fileNOList = files.keys();	//待更新文件编号列表
	bool isNewOpen = false;
	QSqlDatabase prjMenuDb = QSqlDatabase::database(MS_PROJECTMENU_PATH, false);
	if (!prjMenuDb.isOpen())
	{
		isNewOpen = true;
		prjMenuDb = QSqlDatabase::addDatabase("QSQLITE", MS_PROJECTMENU_PATH);
		prjMenuDb.setDatabaseName(MS_PROJECTMENU_PATH);
		prjMenuDb.open();
	}
	if (prjMenuDb.isOpen())
	{
		QSqlQuery query(prjMenuDb);
		QString str_sql;
		str_sql = QString("select [%1],[%2],[%3] from [%4] where %5 in ('" + fileNOList.join("','") + "')")
			.arg(MS_CHARTNO).arg("INDEX").arg("ISOPEN").arg(MS_INDEX_TIFF_TABLE).arg(MS_CHARTNO);
		if (query.exec(str_sql))
		{
			QSqlQuery updateQuery(prjMenuDb);
			prjMenuDb.transaction();
			while (query.next())
			{
				QString fileNO = query.value(0).toString();
				MSFileStruct newFile = files.find(fileNO).value();
				QString index = newFile._relativePath;
				if (!newFile._newName.isEmpty())
				{
					QFile(newFile.absolutePath()).rename(newFile._newName);
					index = newFile._relativePath.left(newFile._relativePath.lastIndexOf("/") + 1) + newFile._newName;
				}
				str_sql = QString("update [%1] set [%2]=?,[%3]=?,[%4]=? where [%5]='%6'")
					.arg(MS_INDEX_TIFF_TABLE).arg(MS_CHARTNO).arg("INDEX").arg("ISOPEN").arg(MS_CHARTNO).arg(fileNO);
				updateQuery.prepare(str_sql);
				updateQuery.addBindValue(fileNO);
				updateQuery.addBindValue(index);
				if (newFile._isOpen)
				{
					updateQuery.addBindValue("1");
				}
				else
				{
					updateQuery.addBindValue("0");
				}
				if (!updateQuery.exec())
				{
					QString errmsg = prjMenuDb.lastError().text();
					//g_pInterface->iDataPrintf(errmsg);
				}
			}
			prjMenuDb.commit();
		}
		else
		{
			QString errmsg = prjMenuDb.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}

		str_sql = QString("select [%1],[%2] from [%3] where %4 in ('" + fileNOList.join("','") + "')")
			.arg(MS_DATANO).arg("INDEX").arg(MS_INDEX_DATA_TABLE).arg(MS_DATANO);
		if (query.exec(str_sql))
		{
			QSqlQuery updateQuery(prjMenuDb);
			prjMenuDb.transaction();
			while (query.next())
			{
				QString fileNO = query.value(0).toString();
				MSFileStruct newFile = files.find(fileNO).value();
				QString index = newFile._relativePath;
				if (!newFile._newName.isEmpty())
				{
					QFile(newFile.absolutePath()).rename(newFile._newName);
					index = newFile._relativePath.left(newFile._relativePath.lastIndexOf("/") + 1) + newFile._newName;
				}
				str_sql = QString("update [%1] set [%2]=?,[%3]=? where [%4]='%5'")
					.arg(MS_INDEX_DATA_TABLE).arg(MS_DATANO).arg("INDEX").arg(MS_DATANO).arg(fileNO);
				updateQuery.prepare(str_sql);
				updateQuery.addBindValue(fileNO);
				updateQuery.addBindValue(index);
				if (!updateQuery.exec())
				{
					QString errmsg = prjMenuDb.lastError().text();
					//g_pInterface->iDataPrintf(errmsg);
				}
			}
			prjMenuDb.commit();
		}
		else
		{
			QString errmsg = prjMenuDb.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}

		str_sql = QString("select [%1],[%2] from [%3] where %4 in ('" + fileNOList.join("','") + "')")
			.arg(MS_FILENO).arg("INDEX").arg(MS_INDEX_FILE_TABLE).arg(MS_FILENO);
		if (query.exec(str_sql))
		{
			QSqlQuery updateQuery(prjMenuDb);
			prjMenuDb.transaction();
			while (query.next())
			{
				QString fileNO = query.value(0).toString();
				MSFileStruct newFile = files.find(fileNO).value();
				QString index = newFile._relativePath;
				if (!newFile._newName.isEmpty())
				{
					QFile(newFile.absolutePath()).rename(newFile._newName);
					index = newFile._relativePath.left(newFile._relativePath.lastIndexOf("/") + 1) + newFile._newName;
				}
				str_sql = QString("update [%1] set [%2]=?,[%3]=? where [%4]='%5'")
					.arg(MS_INDEX_FILE_TABLE).arg(MS_FILENO).arg("INDEX").arg(MS_FILENO).arg(fileNO);
				updateQuery.prepare(str_sql);
				updateQuery.addBindValue(fileNO);
				updateQuery.addBindValue(index);
				if (!updateQuery.exec())
				{
					QString errmsg = prjMenuDb.lastError().text();
					//g_pInterface->iDataPrintf(errmsg);
				}
			}
			prjMenuDb.commit();
		}
		else
		{
			QString errmsg = prjMenuDb.lastError().text();
			//g_pInterface->iDataPrintf(errmsg);
		}

		if (isNewOpen)
		{
			prjMenuDb.close();
		}
	}
	return true;
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
			{
				QString errmsg = database.lastError().text();
				//g_pInterface->iDataPrintf(errmsg);
			}
		}
		if (isNewOpen)
		{
			database.close();
		}
	}
	return isSuccess;
}
