#ifndef MSCOORDSYSTEMSTR_H
#define MSCOORDSYSTEMSTR_H
#include "coordtransform_global.h"
#include <QString>

//!投影坐标系字符串的解析和生成，默认基准面为CGCS2000
class COORDTRANSFORM_EXPORT MSCoordSystemStr
{
public:
	MSCoordSystemStr();
	virtual ~MSCoordSystemStr();

	//!设置投影坐标系字符串
	void SetCoordSystemStr(const QString& qsr);
	//!prjcsName需要传入形如CGCS2000_3_Degree_GK_Zone_34的标准格式
	void SetCoordSystemParams(const QString& prjcsName);
	//!自定义设置
	void SetCoordSystemParams(const QString& prjctionName, double centralMedian, double falseEast = 0, double falseNorth = 0);
	//!获取投影坐标系定义字符串（设置完坐标系参数后可用）
	QString GetCoordSystemStr(){ return _coordsystemStr; }
	//!获取地理坐标系定义字符串
	QString GetGeocsStr(){ return _geocsStr; }
	//!获取投影字符串的xml格式
	QString GetXmldefStr();

	//!地理坐标系名称
	QString GetGeocdName(){ return _geocsName; }
	//!基准面名称
	QString GetDatumName(){ return _datumName; }
	//!椭球名称
	QString GetEllipseName(){ return _ellipseName; }

	//!投影坐标系名称
	QString GetPrjcsName(){ return _prjcsName; }
	//!投影方式名称
	QString GetProjectionName(){ return _prjctionName; }
	//!中央经线
	double GetCentralMedian(){ return _centralMedian; }
	//!东偏
	double GetFalseEast(){ return _falseEast; }
	//!北偏
	double GetFalseNorth(){ return _falseNorth; }

	//!带宽
	int GetZoneWidth(){ return _zoneWidth; }
	//!带号
	int GetZoneNumber(){ return _zoneNumber; }

private:
	void initParams();
	bool parsePrjcsName(const QString& prjcsName);

private:
	QString _coordsystemStr;//!<投影坐标系字符串
	QString _geocsStr;		//!<地理坐标系字符串
	QString _xmldefStr;		//!<xml格式坐标系字符串

	//!地理坐标相关定义
	QString _geocsName;		//!<地理坐标系名称
	QString _datumName;		//!<基准面名称
	QString _ellipseName;	//!<椭球名称
	double _a;				//!<椭球参数：长半轴
	double _f;				//!<椭球参数：椭球扁平程度倒数

	//!投影坐标相关定义
	QString _prjcsName;		//!<投影坐标系名称
	QString _prjctionName;	//!<投影方式名称
	double _centralMedian;	//!<中央经线
	double _falseEast;		//!<东偏
	double _falseNorth;		//!<北偏

	int _zoneWidth;			//!<带宽
	int _zoneNumber;		//!<带号
};


#endif // MSCOORDSYSTEMSTR_H
