#ifndef YNSDBASE_H
#define YNSDBASE_H

#include <QObject>
#include <QApplication>
//#include "iGeometric2d.h"
//#include "geos.h"
//
//using namespace geos::geom;

//云南三调临时文件夹路径
#define YNSD_TEMPPATH QString(qApp->applicationDirPath() + "/yunnantemp")
//云南三调模板文件名称
#define YNSD_TEMPLATENAME QString("三调.db")
//云南三调工作量统计excel模板路径
#define YNSD_GZLTJEXCEL QString(qApp->applicationDirPath() + "/system/内业解译工作量统计表.xlsx")


enum YNSDNetWorkEnum
{
	UnknownRequest = 0,
	GetProjectList = 1,		//获取项目列表
	DownloadDataPacket = 2,	//下载数据包
	UploadDataPacket = 3,	//上传数据包
	GetCkFileList = 4,		//获取检查文件列表
	DownloadCkFile = 5		//下载检查文件
};

//云南三调项目信息结构体
struct YNSDPrjInfo
{
	QString _xmid;		//项目id
	QString _xmmc;		//项目名称
	QString _xzqmc;		//行政区名称
	QString _xzqdm;		//行政区代码
	QString _xmjd;		//项目阶段
	QString _xmms;		//项目信息
	QString _cjsj;		//采集时间
	QString _cjryid;	//采集人员id
	QString _cjry;		//采集人员
	//
};
//读取'项目信息.xml'文件获取相关信息
bool readConfigFile(const QString& file, YNSDPrjInfo& prjInfo);
//读取'datapacket.xml'文件获取相关信息
bool readDataPakcetXml(const QString& file, QVariantMap& infoMap);

//项目数据包信息结构体
struct DataPackInfo
{
	QString _xmid;		//项目id
	QString _xmmc;		//项目名称
	QString _cjsj;		//项目创建时间
	QString _sjbscsj;	//数据包生成时间
	QString _sjbdx;		//数据包大小
	QString _wjmc;		//数据包文件名称
};
//从QList<QVariant>中获取项目列表
void readPackList(const QList<QVariant>& list, QList<DataPackInfo>& packList);

//检测结果文件信息结构体
struct CheckFileInfo
{
	//QString _wjid;		//文件id
	QString _wjmc;		//文件名称
	QString _scsj;		//生成时间
	QString _sjbb;		//数据版本
	QString _zyry;		//作业人员姓名
};
//从QList<QVariant>中获取检查文件列表
void readCkFileList(const QList<QVariant>& list, QList<CheckFileInfo>& ckFileList);

//工作量结构体
struct WorkloadStruct
{
	WorkloadStruct()
	{
		_xzqmj = 0.0;

		_qq_ghtbsl = 0;
		_qq_xc = 0.0;
		_qq_ghtbmj = 0.0;
		_qq_wcbfb = 0.0;

		_bq_ghtbsl = 0;
		_bq_xc = 0.0;
		_bq_ghtbmj = 0.0;
		_bq_wcbfb = 0.0;
	}
	QString _xzqdm;			//行政区代码
	QString _xzqmc;			//行政区名称
	double _xzqmj;			//行政区面积（平方米）

	qint64 _qq_ghtbsl;		//前期为止勾绘图斑数量（个）
	double _qq_xc;			//前期为止线长（米）
	double _qq_ghtbmj;		//前期为止勾绘图斑面积（平方米）
	double _qq_wcbfb;		//前期为止完成百分比（%）

	qint64 _bq_ghtbsl;		//本期为止勾绘图斑数量（个）
	double _bq_xc;			//本期为止线长（米）
	double _bq_ghtbmj;		//本期为止勾绘图斑面积（平方米）
	double _bq_wcbfb;		//本期为止完成百分比（%）

};

QString VariantMapToString(QVariantMap vtmap);
QVariantMap JsonStringToVariantMap(const QString& str);


//
///*
//	@brief 创建Geos环对象
//	@param nodes 传入的点串（是否需要闭合？）
//	@return 返回Geos环对象指针，需要自行销毁
//*/
//LinearRing* createGeosLinearRing(const QPoint2dArray& nodes);
//
///*
//	@brief 创建Geos线对象
//	@param nodes 传入的点串
//	@return 返回Geos线对象指针，需要自行销毁
//*/
//LineString* createGeosLineString(const QPoint2dArray& nodes);
//
///*
//	@brief 创建Geos面对象，返回结果可能为NULL，Polygon，MultiPolygon
//	@param nodesArr 节点数组，一般都是通过getNodes从iDataPolygon获取
//	@return 返回Geos面对象指针，需要自行销毁
//*/
//Geometry* createGeosGeometry(const QNodesArray& nodesArr);
//
///*
//	@brief QNodesArray转QPainterPath
//	@param nodesArr 节点数组，一般都是通过getNodes从iDataPolygon获取
//	@return 返回QPainterPath对象
//*/
//QPainterPath getPainterPathFromNodes(const QNodesArray& nodesArr);
//
///*
//	@brief Geos面对象（Polygon，MultiPolygon）转QNodesArray
//	由于polygon参数可能为MultiPolygon，也就意味着QNodesArray可能为多部件的面
//	@param polygon Geos面对象指针
//	@param cosntrol 环面积控制阈值
//	@return QNodesArray对象，如果是多部件的面可通过seperateNodes打散
//*/
//QNodesArray getNodesFromGeosPolygon(const Geometry* polygon, const double control = 0.001);
//
///*
//	@brief Geos线对象（LineString，MultiLineString）转QNodesArray
//	由于polyline参数可能为MultiLineString，也就意味着QNodesArray可能为多部件的环
//	@param polyline Geos线对象指针
//	@return QNodesArray对象
//*/
//QNodesArray getNodesFromGeosPolyline(const Geometry* polyline);


#endif // YNSDBASE_H
