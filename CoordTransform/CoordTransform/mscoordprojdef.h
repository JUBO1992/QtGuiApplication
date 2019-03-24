/*!	@file mscoordprojdef.h
*
*	@brief 坐标投影定义头文件，定义各种投影类型，用于proj4投影变换
*/

#ifndef MSCOORDPROJDEF_H
#define MSCOORDPROJDEF_H

#include "coordtransform_global.h"
#include <QString>
#include "mscoordstruct.h"

//!投影定义基类（默认椭球CGCS2000）
class COORDTRANSFORM_EXPORT ProjDefBase
{
public:
	ProjDefBase() :_datumType(CGCS2000), _ellip(_datumType){}
	virtual~ProjDefBase(){}

	void SetDatumType(DatumType type = CGCS2000)
	{
		_datumType = type;
		_ellip = MSEllipsoid(_datumType);
	}
	DatumType GetDatumType() const { return _datumType; }
	MSEllipsoid GetEllipsoid() const
	{
		return _ellip;
	}

	//!获取proj4格式的地理定义字符串
	QString GeoStr() const
	{
		return QString("+proj=longlat +a=%1 +f=%2 +no_defs")
			.arg(_ellip._a, 0, 'f', 20).arg(_ellip._f, 0, 'f', 20);
	}

	//!获取proj4格式的投影定义字符串，不同投影类型的子类需要实现此接口
	virtual QString ProjStr() const = 0;

protected:
	MSEllipsoid _ellip;	//!<椭球

	//double _a;		//!<椭球参数：长半轴
	//double _df;		//!<椭球参数：扁率倒数！非扁率，与_f对应

	//double _f;		//!<椭球参数：扁率！非扁率倒数，与_df对应
	//double _b;		//!<椭球参数：短半轴
	//double _e12;	//!<椭球参数：第一偏心率平方
	//double _e22;	//!<椭球参数：第二偏心率平方

	DatumType _datumType;	//!<基准面类型
};

//!无投影定义-地理坐标
class COORDTRANSFORM_EXPORT NoneProjDef :public ProjDefBase
{
public:
	NoneProjDef(){}
	virtual~NoneProjDef(){}
	virtual QString ProjStr() const override
	{
		return QString();
	}
};

//!墨卡托投影定义（角度不变，一般航海中使用）
class COORDTRANSFORM_EXPORT MercProjDef :public ProjDefBase
{
public:
	MercProjDef(){}
	virtual~MercProjDef(){}

	void SetParams(double lon = 0.0, double lat = 0.0, double x0 = 0.0, double y0 = 0.0)
	{
		_merc_lon_0 = lon;
		_merc_lat_ts = lat;
		_merc_x_0 = x0;
		_merc_y_0 = y0;
	}
	virtual QString ProjStr() const override
	{
		return QString("+proj=merc +a=%1 +f=%2 +lon_0=%3 +lat_ts=%4 +x_0=%6 +y_0=%7 +units=m +no_defs")
			.arg(_ellip._a, 0, 'f', 20).arg(_ellip._f, 0, 'f', 20)
			.arg(_merc_lon_0, 0, 'f', 20).arg(_merc_lat_ts, 0, 'f', 20)
			.arg(_merc_x_0, 0, 'f', 20).arg(_merc_y_0, 0, 'f', 20);
	}

private:
	double _merc_lon_0 = 0;		//投影中央经线
	double _merc_lat_ts = 0;	//基准纬线
	double _merc_x_0 = 0;		//东偏
	double _merc_y_0 = 0;		//北偏
};

//!横轴墨卡托投影定义（GuassKruger、UTM等）
class COORDTRANSFORM_EXPORT TMercProjDef :public ProjDefBase
{
public:
	TMercProjDef(){}
	virtual~TMercProjDef(){}

	void SetParams(double lon = 0.0, double lat = 0.0, double k = 1.0, double x0 = 0.0, double y0 = 0.0)
	{
		_tmerc_lon_0 = lon;
		_tmerc_lat_0 = lat;
		_tmerc_k_0 = k;
		_tmerc_x_0 = x0;
		_tmerc_y_0 = y0;
	}
	void SetParams(TMercType type = Custom, int zone = 0)
	{
		_tmerc_lon_0 = 0.0;
		_tmerc_lat_0 = 0.0;
		_tmerc_k_0 = 1.0;
		_tmerc_x_0 = 0.0;
		_tmerc_y_0 = 0.0;
		switch (type)
		{
		case Custom:
			break;
		case GuassKruger3:
			_tmerc_lon_0 = zone * 3;
			_tmerc_x_0 = 500000.0;
			break;
		case GuassK3Zone:
			_tmerc_lon_0 = zone * 3;
			_tmerc_x_0 = zone*1e6 + 500000.0;
			break;
		case GuassKruger6:
			_tmerc_lon_0 = zone * 6 - 3;
			_tmerc_x_0 = 500000.0;
			break;
		case GuassK6Zone:
			_tmerc_lon_0 = zone * 6 - 3;
			_tmerc_x_0 = zone*1e6 + 500000.0;
			break;
		case UTMNorth:
			_tmerc_lon_0 = zone * 6 - 183;
			_tmerc_x_0 = 500000.0;
			_tmerc_k_0 = 0.9996;
			break;
		case UTMSouth:
			_tmerc_lon_0 = zone * 6 - 183;
			_tmerc_x_0 = 500000.0;
			_tmerc_y_0 = 10000000.0;
			_tmerc_k_0 = 0.9996;
			break;
		default:
			break;
		}
	}

	virtual QString ProjStr() const override
	{
		return QString("+proj=tmerc +a=%1 +f=%2 +lon_0=%3 +lat_0=%4 +k_0=%5 +x_0=%6 +y_0=%7 +units=m +no_defs")
			.arg(_ellip._a, 0, 'f', 20).arg(_ellip._f, 0, 'f', 20)
			.arg(_tmerc_lon_0, 0, 'f', 20).arg(_tmerc_lat_0, 0, 'f', 20)
			.arg(_tmerc_k_0, 0, 'f', 20)
			.arg(_tmerc_x_0, 0, 'f', 20).arg(_tmerc_y_0, 0, 'f', 20);
	}

private:
	double _tmerc_lon_0 = 0;	//投影中央经线
	double _tmerc_lat_0 = 0;	//投影中央纬线
	double _tmerc_k_0 = 1;		//比例因子（UTM:0.9996,Guass-Kruger:1.0）
	double _tmerc_x_0 = 0;		//东偏
	double _tmerc_y_0 = 0;		//北偏
};

//!Web墨卡托投影定义（网络地图使用的投影）
class COORDTRANSFORM_EXPORT WebMercProjDef :public ProjDefBase
{
public:
	WebMercProjDef(){}
	virtual ~WebMercProjDef(){}

	void SetParams(double lon = 0.0, double x0 = 0.0, double y0 = 0.0)
	{
		_lon_0 = lon;
		_x_0 = x0;
		_y_0 = y0;
	}

	virtual QString ProjStr() const override
	{
		return QString("+proj=webmerc +a=%1 +f=%2 +lon_0=%3 +x_0=%6 +y_0=%7 +units=m +no_defs")
			.arg(_ellip._a, 0, 'f', 20).arg(_ellip._f, 0, 'f', 20)
			.arg(_lon_0, 0, 'f', 20).arg(_x_0, 0, 'f', 20).arg(_y_0, 0, 'f', 20);
	}

private:
	double _lon_0 = 0;		//投影中央经线
	double _x_0 = 0;		//东偏
	double _y_0 = 0;		//北偏
};

#endif // MSCOORDPROJDEF_H
