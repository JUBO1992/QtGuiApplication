#ifndef MARINEBASE_H
#define MARINEBASE_H

#include <QString>
#include <QMap>
#include <QStringList>
#include <QApplication>
#include <QFileInfo>

#pragma execution_character_set("utf-8")

#define MS_MARINEROOTDIR QString(qApp->applicationDirPath() + "/MarineSurvey")	//海测项目根目录位置，默认在MarineSurvey文件夹下
#define MS_TEMPLATE_NAME QString("海测.db")	//海测模板名称
#define MS_MDBSEEDNAME QString("arcgis10.mdt")

#define MS_PROJECTMENU_PATH QString(MS_MARINEROOTDIR + "/ProjectMenu.db")	//海测项目数据库路径
#define MS_PROJECTMENU_TABLE QString("PROJECTMENU")							//PROJECTMENU表名
#define MS_INDEX_TIFF_TABLE QString("Index_TIFF")							//栅格文件索引表名
#define MS_INDEX_DATA_TABLE QString("Index_Data")							//测量文件索引表名
#define MS_INDEX_FILE_TABLE QString("Index_File")							//档案文件索引表名

#define MS_VECTORDIR_NAME QString("矢量数据")								//矢量数据目录名称
#define MS_RASTERDIR_NAME QString("栅格数据")								//栅格数据目录名称
#define MS_SURVEYDIR_NAME QString("测量文件")								//测量文件目录名称
#define MS_ARCHIVEDIR_NAME QString("档案文件")								//档案文件目录名称

#define MS_METHOD_ADCP QString("ADCP验流")									//ADCP验流文件目录名称
#define MS_METHOD_GPS QString("GPS（全站仪）")
#define MS_METHOD_SEDSAMPLER QString("采泥器")
#define MS_METHOD_SIDESONAR QString("侧扫声呐")
#define MS_METHOD_MAGNETOMETER QString("磁力仪")
#define MS_METHOD_SINGLEBEAM QString("单波束数据")
#define MS_METHOD_MULTIBEAM QString("多波束数据")
#define MS_METHOD_SOUNDVMETER QString("声速仪")
#define MS_METHOD_TIDEGUAGE QString("验潮仪")

#define MS_RASTERMD_NAME QString("MetaData_Raster.db")						//栅格元数据库名称
#define MS_SURVEYMD_NAME QString("MetaData_Data.db")						//测量元数据库名称
#define MS_ARCHIVEMD_NAME QString("MetaData_File.db")						//档案元数据库名称

#define MS_RASTERMD_TABLE QString("MetaData_Raster")						//栅格元数据表名称
#define MS_SURVEYMD_TABLE QString("MetaData_Data")							//测量元数据表名称
#define MS_ARCHIVEMD_TABLE QString("MetaData_File")							//档案元数据表名称

#define MS_CHARTNO QString("ChartNO")		//栅格文件编号字段名称
#define MS_DATANO QString("DataNO")			//测量文件编号字段名称
#define MS_FILENO QString("FileNO")			//档案文件编号字段名称
#define MS_R QString("R")					//栅格文件编号标记
#define MS_D QString("D")					//测量文件编号标记
#define MS_F QString("F")					//档案文件编号标记


//文件类型
enum MSFileEnum
{
	//MultiBeamFile,
	//SingleBeamFile,
	//ADCPFile,
	//SideSonarFile,
	//TideGuageFile,
	//SoundFile,
	//SedimentFile,
	//GPSFile,
	//MagnetoFile,
	//SurveyFile = MultiBeamFile | SingleBeamFile | ADCPFile
	//| SideSonarFile | TideGuageFile | SoundFile 
	//| SedimentFile | GPSFile | MagnetoFile,			//测量
	VectorFile,			//矢量
	RasterFile,			//栅格
	SurveyFile,			//测量
	ArchiveFile			//档案
};

//海测方法
enum MSurveyMethod
{
	MSUnknown = -1,					//未知
	MultiBeam = 0,					//多波束测深
	SingleBeam = 1,					//单波束测深
	ADCP = 2,						//ADCP
	SideSonar = 4,					//侧扫声呐
	TideGuage = 8,					//验潮仪
	SoundVelocimeter = 16,			//声速仪
	SedimentSampler = 32,			//采泥器
	GPS = 64,						//GPS（全站仪）
	Magnetometer = 128				//磁力仪
};

/*
@brief 根据条件获取目录相对路径（不包括项目目录名称）
*/
QString GetRelativeDirPath(const MSFileEnum& type, const MSurveyMethod& method = MSurveyMethod::MSUnknown);

struct MarinePrjStruct
{
	//返回工程路径
	QString projectPath() const 
	{ 
		return MS_MARINEROOTDIR + "/" + _prjID + _prjName;
	}

	//返回矢量数据所在路径
	QString vectorPath() const
	{
		return projectPath() + "/" + MS_VECTORDIR_NAME;
	}
	//返回栅格数据所在路径
	QString rasterPath() const
	{
		return projectPath() + "/" + MS_RASTERDIR_NAME;
	}
	//返回测量数据所在路径
	QString surveyPath(const MSurveyMethod& method = MSurveyMethod::MSUnknown) const
	{
		QString relapath = GetRelativeDirPath(MSFileEnum::SurveyFile, method);
		return projectPath() + "/" + relapath;
	}
	//返回档案数据所在路径
	QString archivePath() const
	{
		return projectPath() + "/" + MS_ARCHIVEDIR_NAME;
	}

	//返回栅格元数据库路径
	QString rasterMDPath() const
	{
		return rasterPath() + "/" + MS_RASTERMD_NAME;
	}
	//返回测量元数据库路径
	QString surveyMDPath() const
	{
		return surveyPath() + "/" + MS_SURVEYMD_NAME;
	}
	//返回档案元数据库路径
	QString archiveMDPath() const
	{
		return archivePath() + "/" + MS_ARCHIVEMD_NAME;
	}

	QString _prjID;					//项目编号
	QString _prjName;				//项目名称
	QString _prjType;				//项目类型
	QString _prjEngineer;			//项目工程师
	QString _surveyTeam;			//测量队伍
	QString _surveyPrj;				//测量投影
	QString _surveyCoor;			//测量坐标系
	QString _surveyType;			//测量性质
};

//海测文件
struct MSFileStruct
{
	MSFileStruct(QString fileNO = "", QString index = "", bool isOpen = false)
		: _fileNO(fileNO)
		, _relativePath(index)
		, _isOpen(isOpen)
	{
		
	}
	QString _fileNO;		//文件编号，工程内唯一
	QString _relativePath;	//相对路径，工程内唯一，从项目文件夹开始（包括项目文件夹）
	bool _isOpen;			//是否打开

	QString _newName;		//新文件名（用于在文件改名时保存新文件名称）

	//返回文件绝对路径
	QString absolutePath() const
	{
		if (_relativePath.isEmpty())
		{
			return QString();
		}
		return MS_MARINEROOTDIR + "/" + _relativePath;
	}
	//返回文件名
	QString fileName() const
	{
		return QFileInfo(absolutePath()).fileName();
	}
	//返回文件是否存在
	bool isExist() const
	{
		return QFileInfo(absolutePath()).exists();
	}
	//返回是否有效
	bool isValid() const
	{
		return !_fileNO.isEmpty();
	}
};


/*
	@brief 读取ProjectMenu表，获取海测工程列表
	注意：该函数不会清空prjlist
	@param[out] prjlist为海测工程列表
*/
void ReadProjectMenu(QList<MarinePrjStruct>& prjlist);

/*
	@brief 读取ProjectMenu表，获取海测工程列表
	注意：该函数不会清空prjMap
	@param[out] prjMap为海测工程列表（key为海测工程编号，value为海测工程详细信息）
*/
void ReadProjectMenu(QMap<QString, MarinePrjStruct>& prjMap);

/*
	@brief 读取ProjectMenu表，更新海测工程列表
	注意：该函数会清空prjMap后重新追加
	@param[out] prjMap为海测工程列表（key为海测工程编号，value为海测工程详细信息）
*/
void UpdateProjectMenu(QMap<QString, MarinePrjStruct>& prjMap);

/*
	@brief 删除文件或文件夹（文件夹实现了递归删除）
	@param[in] path为要删除文件夹路径
	@return 返回是否成功
*/
bool DelFileOrDir(const QString &path);

/*
	@brief 拷贝文件或文件夹（文件夹实现了递归拷贝）
	@param[in] from 源文件或文件夹路径
	@param[in] to 目标文件或文件夹路径
	@param[in] overlay 是否覆盖标记
	@return 返回是否成功
*/
bool CopyFileOrDir(const QString &from, const QString &to, bool overlay = true);

/*
	@brief 递归遍历文件夹，获取文件夹下所有文件
	@param[in] path 文件夹路径
	@return 返回文件列表（绝对路径）
*/
QStringList GetFileList(const QString &path);

/*
	@brief 创建海测工程
	@param[in] marine为要创建的海测工程
	@return 返回是否成功
*/
bool CreateMarinePrj(const MarinePrjStruct& marine);

/*
	@brief 删除海测工程
	@param[in] marine为要删除的海测工程
	@return 返回是否成功
*/
bool DeleteMarinePrj(const MarinePrjStruct& marine);

/*
	@brief 打开海测工程
	@param[in] marine为要打开的海测工程
	@return 返回是否成功
*/
bool OpenMarinePrj(const MarinePrjStruct& marine);

/*
	@brief 关闭海测工程
	@param[in] marine为要关闭的海测工程
	@return 返回是否成功
*/
bool CloseMarinePrj(const MarinePrjStruct& marine);

/*
	@brief 更新海测工程
	@param[in] marine记录更新信息
	@param[out] marinePrjID为待更新的工程编号
	@return 返回是否成功
*/
bool UpdateMarinePrj(const MarinePrjStruct& marine, const QString& marinePrjID);

/*
	@brief 获取海测工程下的文件列表
	注意：该函数不会清空files
	@param[in] marine
	@param[in] type 文件类型
	@param[out] files 读取到的文件列表
	@param[in] method type == SurveyFile时有效
*/
void GetMarinePrjFiles(const MarinePrjStruct& marine, const MSFileEnum& type, QMap<QString, MSFileStruct>& files,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 向海测工程添加文件，返回添加文件的编号列表
	注意：1该函数对VectorFile类型无效；2该函数内部对文件是否存在不做判断，会尝试覆盖已有文件
	@param[in] marine
	@param[in] type 添加的文件类型，当type == SurveyFile时需要指定method参数
	@param[in] files 要添加的文件列表（记录文件全路径），这些文件将被拷贝到海测工程下
	@param[in] method 当添加测量文件时，用于指定测量文件由何种测量手段获得
	@return 返回文件编号列表，如果添加失败则返回空列表
*/
QStringList AddFilesToMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& files, 
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 向海测工程添加文件，返回添加文件的编号列表
	注意：1该函数对VectorFile类型无效；2该函数内部对文件是否存在不做判断，会尝试覆盖已有文件
	@param[in] marine
	@param[in] type 添加的文件类型，当type == SurveyFile时需要指定method参数
	@param[in] dirpath 要添加的文件所在目录路径，该目录下所有文件及文件夹将被拷贝到目标路径下
	@param[in] method 当添加测量文件时，用于指定测量文件由何种测量手段获得
	@return 返回文件编号列表，如果添加失败则返回空列表
*/
QStringList AddDirToMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QString& dirpath,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 查询海测工程文件
	注意：1该函数不会清空files；2该函数对VectorFile类型无效
	@param[in] marine 
	@param[in] type 要查询的文件类型
	@param[in] fileNOList 要查询的文件编号（如果为空则获取该类型的所有文件类别）
	@param[out] files 查询到的文件信息
	@param[in] method 如果type == SurveyFile且fileNOList为空则需要指定method，如果不指定则获取'测量文件'目录下所有内容
*/
void FindFilesInMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& fileNOList, QMap<QString, MSFileStruct>& files,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 从海测工程删除文件
	注意：该函数对VectorFile类型无效
	@param[in] marine
	@param[in] type 要删除的文件类型
	@param[in] fileNOList 要删除的文件编号（如果为空则清空相应目录下文件及文件夹）
	@param[in] method 如果type == SurveyFile且fileNOList为空则需要指定method，如果不指定则删除'测量文件'目录下所有内容
	@return 返回是否删除成功
*/
bool DeleteFilesFromMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& fileNOList = QStringList(),
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 更新海测工程中文件信息，此处只针对Index表进行信息更新（如果文件名变化同时修改文件名），不支持修改文件编号
	@param[in] marine
	@param[in] files 要更新的文件列表，key为文件编号，value为更新信息
	@return 
*/
bool UpdateFilesInMarinePrj(const MarinePrjStruct& marine, const QMap<QString, MSFileStruct>& files);

/*
	@brief 根据条件获取目录相对路径（包括项目目录名称）
*/
QString GetRelativeDirPath(const MarinePrjStruct& marine, const MSFileEnum& type,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 根据条件获取目录绝对路径
*/
QString GetAbsolutDirPath(const MarinePrjStruct& marine, const MSFileEnum& type,
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);


/*
	@brief 获取属性
	@param[in] name 属性名称
	@return 属性值
*/
QVariant MSGetProperty(const QString& name);

/*
	@brief 设置属性
	@param[in] name 属性名称
	@param[in] value 属性值
*/
bool MSSetProperty(const QString& name, const QVariant& value);

#endif //MARINEBASE_H
