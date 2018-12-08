#include "marinebase.h"
//#include "idatainterface.h"
#include <QSqlDatabase>
#include <QDir>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "marineproject.h"

//extern iDataInterface* g_pInterface;

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
		if (!query.exec(query_sql))
		{
			database.close();
			return;
		}
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
		if (isNewOpen)
		{
			database.close();
		}
	}
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
			DelFileOrDir(file.absoluteFilePath());
		}
	}
	return dir.rmpath(dir.absolutePath()); // 删除文件夹
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
	if (!targetDir.exists())
	{
		if (!targetDir.mkdir(targetDir.absolutePath()))
			return false;
	}
	QFileInfoList fileInfoList = sourceDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;
		if (overlay && QFile::exists(targetDir.filePath(fileInfo.fileName())))
		{
			QFile::remove(targetDir.filePath(fileInfo.fileName()));
		}
		if (!QFile::copy(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName())))
		{
			return false;
		}
	}
	return true;
}

bool CreateMarinePrj(const MarinePrjStruct& marine)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return false;
	}
	bool isSuccess = true;
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
	surveyDir.mkdir("ADCP验流");
	surveyDir.mkdir("GPS（全站仪）");
	surveyDir.mkdir("采泥器");
	surveyDir.mkdir("侧扫声呐");
	surveyDir.mkdir("磁力仪");
	surveyDir.mkdir("单波束数据");
	surveyDir.mkdir("多波束数据");
	surveyDir.mkdir("声速仪");
	surveyDir.mkdir("验潮仪");
	//拷贝元数据模板
	CopyFileOrDir(rootPath + "/" + MS_RASTERMD_NAME, marine.rasterPath() + "/" + MS_RASTERMD_NAME);
	CopyFileOrDir(rootPath + "/" + MS_SURVEYMD_NAME, marine.surveyPath() + "/" + MS_SURVEYMD_NAME);
	CopyFileOrDir(rootPath + "/" + MS_ARCHIVEMD_NAME, marine.archivePath() + "/" + MS_ARCHIVEMD_NAME);

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
		str_sql = QString("INSERT INTO [1%] (" + insert_keys.join(",") + ") VALUES (?,?,?,?,?,?,?,?) ").arg(MS_PROJECTMENU_TABLE);
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
	}
	if (isNewOpen)
	{
		database.close();
	}

	return isSuccess;
}

bool DeleteMarinePrj(const MarinePrjStruct& marine)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return false;
	}
	//需要删除ProjectMenu中存储的相关信息
	//此处存在问题，需要进一步考虑，删除需要先关闭所有数据再进行操作！！！
	QString path = marine.projectPath();


	return DelFileOrDir(path);
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
	return true;
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

bool UpdateMarinePrj(const MarinePrjStruct& marine, QString& marinePrjID)
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
		return false;
	}

	//项目编号或项目名称发生改变需要更新项目文件夹的名称
	if (marine._prjID != marinePrjID || marine._prjName != marinePrjName)
	{
		QString oldname = it.value().projectPath();
		QString newname = marine.projectPath();
		bool flag = QDir().rename(oldname, newname);
		if (!flag)
		{//如果文件夹重命名失败则直接返回，不进行数据库的更新
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
	if (marine._prjID != marinePrjID)
	{
	}
	return isSuccess;
}

void GetMarinePrjFiles(const MarinePrjStruct& marine, const MSFileEnum& type, QMap<QString, MSFileStruct>& files)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return;
	}
	QStringList fileNOList;	//文件编号列表
	QSqlDatabase MDdb;		//元数据库
	bool isNewOpen = false;
	QString str_sql;
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
	else if (type == MSFileEnum::RasterFile)
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
		str_sql = QString("SELECT ChartNO FROM [%1]").arg(MS_RASTERMD_TABLE);
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
		str_sql = QString("SELECT DataNO FROM [%1]").arg(MS_SURVEYMD_TABLE);
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
		str_sql = QString("SELECT FileNO FROM [%1]").arg(MS_ARCHIVEMD_TABLE);
	}
	if (MDdb.isOpen())
	{
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

	FindFilesInMarinePrj(type, fileNOList, files);
}

QStringList AddFilesToMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& files,
	const MSurveyMethod& method /*= MSurveyMethod::Unknown*/)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return QStringList();
	}
	QStringList fileNOList;
	QSqlDatabase MDdb;		//元数据库
	bool isNewOpen = false;
	QString str_sql;
	int maxNO = 0;			//当前数据库最大编号
	QSqlRecord record;
	if (type == MSFileEnum::VectorFile)
	{
		QString vectorDir = marine.vectorPath();
		for (int i = 0; i < files.size(); ++i)
		{
			QString vectorNO = marine._prjID + "V" + QString("%1").arg(1, 6, 10, QChar('0'));
			fileNOList << vectorNO;
			QString sourcePath = files.at(i);
			QString destPath = marine.vectorPath() + "/" + QFile(sourcePath).fileName();
		}
		return fileNOList;
	}
	else if (type == MSFileEnum::RasterFile)
	{
		QString rasterMD = marine.rasterMDPath();
		MDdb = QSqlDatabase::database(rasterMD, false);
		if (!MDdb.isOpen())
		{
			isNewOpen = true;
			MDdb = QSqlDatabase::addDatabase("QSQLITE", rasterMD);
			MDdb.setDatabaseName(rasterMD);
			MDdb.open();
			record = MDdb.record(MS_RASTERMD_TABLE);
		}
		str_sql = QString("select max(ChartNO) from [%1]").arg(MS_RASTERMD_TABLE);
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
			record = MDdb.record(MS_SURVEYMD_TABLE);
		}
		str_sql = QString("select max(DataNO) from [%1]").arg(MS_SURVEYMD_TABLE);
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
			record = MDdb.record(MS_ARCHIVEMD_TABLE);
		}
		str_sql = QString("select max(FileNO) from [%1]").arg(MS_ARCHIVEMD_TABLE);
	}
	if (MDdb.isOpen())
	{
		QSqlQuery query(MDdb);
		if (query.exec(str_sql))
		{
			while (query.next())
			{
				QString id = query.value(0).toString();
				if (type == MSFileEnum::RasterFile)
				{
					int rasterNO = id.mid(id.lastIndexOf("R") + 1).toInt();
					if (rasterNO > maxNO)
					{
						maxNO = rasterNO;
					}
				}
				else if (type == MSFileEnum::SurveyFile)
				{
					int surveyNO = id.mid(id.lastIndexOf("D") + 1).toInt();
					if (surveyNO > maxNO)
					{
						maxNO = surveyNO;
					}
				}
				else if (type == MSFileEnum::ArchiveFile)
				{
					int archiveNO = id.mid(id.lastIndexOf("F") + 1).toInt();
					if (archiveNO > maxNO)
					{
						maxNO = archiveNO;
					}
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
		QString insert_sql;
		if (type == RasterFile)
		{
			insert_sql = "INSERT INTO [" + MS_RASTERMD_TABLE + "] (" + fields.join(",")
				+ ") VALUES (" + markList.join(",") + ")";
		}
		else if (type == SurveyFile)
		{
			insert_sql = "INSERT INTO [" + MS_SURVEYMD_TABLE + "] (" + fields.join(",")
				+ ") VALUES (" + markList.join(",") + ")";
		}
		else if (type == ArchiveFile)
		{
			insert_sql = "INSERT INTO [" + MS_ARCHIVEMD_TABLE + "] (" + fields.join(",")
				+ ") VALUES (" + markList.join(",") + ")";
		}
		QSqlQuery insertQuery(MDdb);
		MDdb.transaction();
		for (int i = 0; i < files.size(); ++i)
		{
			maxNO++;
			QString FileNO;
			if (type == RasterFile)
			{
				FileNO = marine._prjID + "R" + QString("%1").arg(maxNO, 6, 10, QChar('0'));
			}
			else if (type == SurveyFile)
			{
				FileNO = marine._prjID + "D" + QString("%1").arg(maxNO, 6, 10, QChar('0'));
			}
			else if (type == ArchiveFile)
			{
				FileNO = marine._prjID + "F" + QString("%1").arg(maxNO, 6, 10, QChar('0'));
			}
			fileNOList << FileNO;
			insertQuery.prepare(insert_sql);
			for (int j = 0; j < record.count(); ++j)
			{
				QSqlField field = record.field(j);
				QString fieldname = field.name();
				if (fieldname.compare("ChartNO", Qt::CaseInsensitive) == 0
					|| fieldname.compare("DataNO", Qt::CaseInsensitive) == 0
					|| fieldname.compare("FileNO", Qt::CaseInsensitive) == 0)
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
				QString errmsg = MDdb.lastError().text();
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
			str_sql = QString("insert into [%1] (ChartNO,INDEX,ISOPN) values (?,?,?)").arg(MS_INDEX_TIFF_TABLE);
		}
		else if (type == MSFileEnum::SurveyFile)
		{
			str_sql = QString("insert into [%1] (DataNO,INDEX) value (?,?)").arg(MS_INDEX_DATA_TABLE);
		}
		else if (type == MSFileEnum::ArchiveFile)
		{
			str_sql = QString("insert into [%1] (FileNO,INDEX) value (?,?)").arg(MS_INDEX_FILE_TABLE);
		}
		QSqlQuery insertQuery(prjMenuDb);
		prjMenuDb.transaction();
		for (int i = 0; i < fileNOList.size(); ++i)
		{
			QString FileNO = fileNOList.at(i);
			QString relativePath = QFile(files.at(i)).fileName();//此处需要相对路径！！！！！！！！！！！
			bool isOpen = false;
			insertQuery.prepare(str_sql);
			insertQuery.addBindValue(QString(FileNO));
			insertQuery.addBindValue(QString(relativePath));
			if (type == RasterFile)
			{
				insertQuery.addBindValue(isOpen);
			}
			if (!insertQuery.exec())
			{
				QString errmsg = MDdb.lastError().text();
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

void FindFilesInMarinePrj(const MSFileEnum& type, const QStringList& fileNOList, QMap<QString, MSFileStruct>& files)
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
		if (type == MSFileEnum::RasterFile)
		{
			str_sql = QString("select ChartNO,INDEX,ISOPEN from [%1] where ChartNO in ('" + fileNOList.join("','") + "')").arg(MS_INDEX_TIFF_TABLE);
		}
		else if (type == MSFileEnum::SurveyFile)
		{
			str_sql = QString("select DataNO,INDEX from [%1] where DataNO in ('" + fileNOList.join("','") + "')").arg(MS_INDEX_DATA_TABLE);
		}
		else if (type == MSFileEnum::ArchiveFile)
		{
			str_sql = QString("select FileNO,INDEX from [%1] where FileNO in ('" + fileNOList.join("','") + "')").arg(MS_INDEX_FILE_TABLE);
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
		if (isNewOpen)
		{
			prjMenuDb.close();
		}
	}
}

void DeleteFilesFromMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& fileNOList)
{
	if (marine._prjID.isEmpty())
	{
		//g_pInterface->iDataPrintf("传入的项目编号为空！");
		return;
	}
	if (type == MSFileEnum::VectorFile)
	{
		return;
	}
	QMap<QString, MSFileStruct> files;
	FindFilesInMarinePrj(type, fileNOList, files);
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
			str_sql = QString("delete from [%1] where ChartNO in ('" + fileNOList.join("','") + "')").arg(MS_INDEX_TIFF_TABLE);
		}
		else if (type == MSFileEnum::SurveyFile)
		{
			str_sql = QString("delete from [%1] where DataNO in ('" + fileNOList.join("','") + "')").arg(MS_INDEX_DATA_TABLE);
		}
		else if (type == MSFileEnum::ArchiveFile)
		{
			str_sql = QString("delete from [%1] where FileNO in ('" + fileNOList.join("','") + "')").arg(MS_INDEX_FILE_TABLE);
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
		str_sql = QString("delete from [%1] where ChartNO in ('" + fileNOList.join("','") + "')").arg(MS_RASTERMD_TABLE);
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
		str_sql = QString("delete from [%1] where DataNO in ('" + fileNOList.join("','") + "')").arg(MS_SURVEYMD_TABLE);
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
		str_sql = QString("delete from [%1] where FileNO in ('" + fileNOList.join("','") + "')").arg(MS_ARCHIVEMD_TABLE);
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

	//下面删除文件
	for (auto it = files.begin(); it != files.end(); ++it)
	{
		QString filepath = it.value().absolutePath();
		DelFileOrDir(filepath);
	}
}
