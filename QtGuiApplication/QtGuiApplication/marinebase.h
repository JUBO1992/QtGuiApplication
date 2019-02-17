#ifndef MARINEBASE_H
#define MARINEBASE_H

#include <QString>
#include <QMap>
#include <QStringList>
#include <QApplication>
#include <QFileInfo>
#include <QDate>
#include "commonoperate.h"

extern CommonOperate * g_COperate;

#define MS_MARINEROOTDIR QString(qApp->applicationDirPath() + "/MarineSurvey")	//海测项目根目录位置，默认在MarineSurvey文件夹下
#define MS_TEMPLATE_PATH QString(qApp->applicationDirPath() + "/system/TEMPLESETFILE/海测.db")	//海测模板路径
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

struct MarinePrjStruct
{
	//返回工程路径
	QString projectPath() const;

	//返回矢量数据所在路径
	QString vectorPath() const;

	//返回栅格数据所在路径
	QString rasterPath() const;

	//返回测量数据所在路径
	QString surveyPath(const MSurveyMethod& method = MSurveyMethod::MSUnknown) const;

	//返回档案数据所在路径
	QString archivePath() const;

	//返回栅格元数据库路径
	QString rasterMDPath() const;

	//返回测量元数据库路径
	QString surveyMDPath() const;

	//返回档案元数据库路径
	QString archiveMDPath() const;

	//返回.idata工程文件路径
	QString idataPrjPath() const;

	QString _prjID;					//项目编号
	QString _prjName;				//项目名称
	QString _prjType;				//项目类型
	QString _prjEngineer;			//项目工程师
	QString _surveyTeam;			//测量队伍
	QString _surveyCoor;			//测量坐标系
	QString _surveyPrj;				//测量投影
	QString _surveyType;			//测量性质
	QDate _surveySTime;				//测量起始时间
	QDate _surveyETime;				//测量终止时间
	QDate _chartSTime;				//制图起始时间
	QDate _chartETime;				//制图终止时间
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
	QString absolutePath() const;

	//返回文件名
	QString fileName() const;

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
	@brief 打印信息
	@param[in] msg 信息
*/
inline void MSMsgPrintf(const QString& msg)
{
	g_COperate->MsgPrint(msg);
}

/*
	@brief 删除文件或文件夹（文件夹实现了递归删除）
	@param[in] path为要删除文件夹路径
	@return 返回是否成功
*/
bool MSDelFileOrDir(const QString &path);

/*
	@brief 拷贝文件或文件夹（文件夹实现了递归拷贝）
	@param[in] from 源文件或文件夹路径
	@param[in] to 目标文件或文件夹路径
	@param[in] overlay 是否覆盖标记
	@return 返回是否成功
*/
bool MSCopyFileOrDir(const QString &from, const QString &to, bool overlay = true);

/*
	@brief 递归遍历文件夹，获取文件夹下所有文件
	@param[in] path 文件夹路径
	@return 返回文件列表（绝对路径）
*/
QStringList MSGetFileList(const QString &path);

/*
	@brief 根据条件获取目录相对路径（不包括项目目录名称）
*/
QString GetRelativeDirPath(const MSFileEnum& type, const MSurveyMethod& method = MSurveyMethod::MSUnknown);

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
