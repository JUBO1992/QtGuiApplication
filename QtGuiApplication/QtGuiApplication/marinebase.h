#ifndef MARINEBASE_H
#define MARINEBASE_H

#include <QString>
#include <QMap>
#include <QStringList>
#include <QApplication>
#include <QFile>

#pragma execution_character_set("utf-8")

#define MS_MARINEROOTDIR QString(qApp->applicationDirPath() + "/MarineSurvey")	//海测项目根目录位置，默认在MarineSurvey文件夹下

#define MS_PROJECTMENU_PATH QString(MS_MARINEROOTDIR + "/ProjectMenu.db")	//海测项目数据库路径
#define MS_PROJECTMENU_TABLE QString("PROJECTMENU")							//PROJECTMENU表名
#define MS_INDEX_TIFF_TABLE QString("Index_TIFF")							//栅格文件索引表名
#define MS_INDEX_DATA_TABLE QString("Index_Data")							//测量文件索引表名
#define MS_INDEX_FILE_TABLE QString("Index_File")							//档案文件索引表名

#define MS_VECTORDIR_NAME QString("矢量数据")								//矢量数据目录名称
#define MS_RASTERDIR_NAME QString("栅格数据")								//栅格数据目录名称
#define MS_SURVEYDIR_NAME QString("测量文件")								//测量文件目录名称
#define MS_ARCHIVEDIR_NAME QString("档案文件")								//档案文件目录名称

//surveyDir.mkdir("ADCP验流");
//surveyDir.mkdir("GPS（全站仪）");
//surveyDir.mkdir("采泥器");
//surveyDir.mkdir("侧扫声呐");
//surveyDir.mkdir("磁力仪");
//surveyDir.mkdir("单波束数据");
//surveyDir.mkdir("多波束数据");
//surveyDir.mkdir("声速仪");
//surveyDir.mkdir("验潮仪");

#define MS_RASTERMD_NAME QString("MetaData_Raster.db")						//栅格元数据库名称
#define MS_SURVEYMD_NAME QString("MetaData_Data.db")						//测量元数据库名称
#define MS_ARCHIVEMD_NAME QString("MetaData_File.db")						//档案元数据库名称

#define MS_RASTERMD_TABLE QString("MetaData_Raster")						//栅格元数据表名称
#define MS_SURVEYMD_TABLE QString("MetaData_Data")							//测量元数据表名称
#define MS_ARCHIVEMD_TABLE QString("MetaData_File")							//档案元数据表名称


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
	QString surveyPath() const
	{
		return projectPath() + "/" + MS_SURVEYDIR_NAME;
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
	GPS = 64,						//GPS/全站仪
	Magnetometer = 128				//磁力仪
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

	//返回文件绝对路径
	QString absolutePath()
	{
		return MS_MARINEROOTDIR + "/" + _relativePath;
	}
	//返回文件名
	QString fileName()
	{
		return QFile(absolutePath()).fileName();
	}
};

//文件类型
enum MSFileEnum
{
	VectorFile = 0,			//矢量
	RasterFile = 1,			//栅格
	SurveyFile = 2,			//测量
	ArchiveFile = 4			//档案
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
	@brief 删除文件或文件夹（文件夹实现了递归删除）
	@param[in] path为要删除文件夹路径
	@return 返回是否成功
*/
bool DelFileOrDir(const QString &path);

/*
	@brief 拷贝文件或文件夹（文件夹未实现递归拷贝）
	@param[in] from 源文件或文件夹路径
	@param[in] to 目标文件或文件夹路径
	@param[in] overlay 是否覆盖标记
	@return 返回是否成功
*/
bool CopyFileOrDir(const QString &from, const QString &to, bool overlay = true);

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
bool UpdateMarinePrj(const MarinePrjStruct& marine, QString& marinePrjID);

/*
	@brief 获取海测工程下的文件列表
	注意：该函数不会清空files
	@param[in] marine
	@param[in] type 文件类型
	@param[out] files 读取到的文件列表
*/
void GetMarinePrjFiles(const MarinePrjStruct& marine, const MSFileEnum& type, QMap<QString, MSFileStruct>& files);

/*
	@brief 向海测工程添加文件，返回添加文件的编号列表
	@param[in] marine
	@param[in] type 添加的文件类型，当type == SurveyFile是需要指定method参数
	@param[in] files 要添加的文件列表（记录文件全路径），这些文件将被拷贝到海测工程下
	@param[in] method 当添加测量文件时，用于指定测量文件由何种测量手段获得
	@return 返回文件编号列表
*/
QStringList AddFilesToMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& files, 
	const MSurveyMethod& method = MSurveyMethod::MSUnknown);

/*
	@brief 查询海测工程文件
	注意：1该函数不会清空files；2该函数对VectorFile类型无效
	@param[in] type 要查询的文件类型
	@param[in] fileNOList 要查询的文件编号
	@param[out] files 查询到的文件信息
*/
void FindFilesInMarinePrj(const MSFileEnum& type, const QStringList& fileNOList, QMap<QString, MSFileStruct>& files);

/*
	@brief 从海测工程删除文件
	注意：该函数对VectorFile类型无效
	@param[in] marine
	@param[in] type 要删除的文件类型
	@param[in] fileNOList 要删除的文件编号
*/
void DeleteFilesFromMarinePrj(const MarinePrjStruct& marine, const MSFileEnum& type, const QStringList& fileNOList);

#endif //MARINEBASE_H
