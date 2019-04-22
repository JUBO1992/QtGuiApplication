#ifndef GEOGCSSTR_H
#define GEOGCSSTR_H
#include "coordtransform_global.h"
#include <QString>
#include "coordstruct.h"
#include "cs_coordsystemstr.h"

//!地理坐标系参考椭球
struct GeoSpheroid
{
	QString _name;	//!<椭球名称
	double _a;		//!<椭球参数：长半轴
	double _df;		//!<椭球参数：扁率倒数！非扁率，与_f对应

	double _f;		//!<椭球参数：扁率！非扁率倒数，与_df对应
	double _b;		//!<椭球参数：短半轴
	double _e12;	//!<椭球参数：第一偏心率平方
	double _e22;	//!<椭球参数：第二偏心率平方

	GeoSpheroid() :_name(""), _a(0), _df(0){ _calcParams(); }
	GeoSpheroid(QString name, double a, double df) :_name(name), _a(a), _df(df){ _calcParams(); }

private:
	void _calcParams()
	{
		if (_df < 1.0)
		{
			_a = _df = _f = _b = _e12 = _e22 = 0.0;
			return;
		}
		_f = 1 / _df;
		_b = _a*(1 - _f);
		_e12 = 1 - (_b / _a)*(_b / _a);
		_e22 = (_a / _b)*(_a / _b) - 1;
	}

};
//!地理坐标系大地基准
struct GeoDatum
{
	DatumType _type;		//!<大地基准类型
	QString _name;			//!<大地基准名称
	GeoSpheroid _spheriod;	//!<对应椭球

	GeoDatum() :_type(DTUndef), _name(""), _spheriod(GeoSpheroid()){ }
	GeoDatum(QString name, GeoSpheroid spheriod) :_type(DTUndef), _name(name), _spheriod(spheriod){ }
	GeoDatum(DatumType dtype) : _type(dtype){
		switch (_type)
		{
		case DTUndef:
			_name = "";
			_spheriod = GeoSpheroid();
			break;
		case WGS84:
			_name = "D_WGS_1984";
			_spheriod = GeoSpheroid("WGS_1984", 6378137.0, 298.257223563);
			break;
		case CGCS2000:
			_name = "D_China_2000";
			_spheriod = GeoSpheroid("CGCS2000", 6378137.0, 298.257222101);
			break;
		case Xian80:
			_name = "D_Xian_1980";
			_spheriod = GeoSpheroid("Xian_1980", 6378140.0, 298.257);
			break;
		case Beijing54:
			_name = "D_Beijing_1954";
			_spheriod = GeoSpheroid("Krasovsky_1940", 6378245.0, 298.3);
			break;
		default:
			break;
		}
	}
};

//!地理坐标系字符串生成与解析类
class COORDTRANSFORM_EXPORT GeogcsStr :public CoordSystemStr
{
public:
	GeogcsStr();
	GeogcsStr(const QString& qsr);
	GeogcsStr(const DatumType& type);
	GeogcsStr(const QString& name, const GeoDatum& datum);
	virtual ~GeogcsStr();

	void setCoordSystemStr(const QString& qsr);

	//!获得地理坐标系名称
	virtual QString getName() const override{ return _geogcs_name; }
	//!获取标准地理坐标系定义字符串
	virtual QString getCoordSystemStr() const override;
	//!获取xml格式坐标系统字符串
	virtual QString getXmlDefStr() const override;

	//!获得大地基准面
	GeoDatum getGeoDatum() const { return _geo_datum; }
	//!获得基准面名称
	QString getDatumName() const { return _geo_datum._name; }

	//!获得参考椭球
	GeoSpheroid getGeoSpheroid() const { return _geo_datum._spheriod; }
	//!获得参考椭球名称
	QString getSpheroidName() const { return _geo_datum._spheriod._name; }
	//!获得参考椭球长半轴
	double getSpheroidA() const { return _geo_datum._spheriod._a; }
	//!获得参考椭球扁率倒数
	double getSpheroidDf() const { return _geo_datum._spheriod._df; }

private:
	//!解析地理坐标系字符串，成功返回true，失败返回false
	bool _parseGeogcsStr(const QString& qsr);

private:
	QString _geogcs_name;	//!<地理坐标系名称
	GeoDatum _geo_datum;	//!<大地基准面定义
};

#endif // GEOGCSSTR_H
