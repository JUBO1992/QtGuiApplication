/*!	@file cs_projparams.h
*	@brief 投影参数定义类头文件，定义各种投影类型的参数
*/

#ifndef CS_PROJPARAMS_H
#define CS_PROJPARAMS_H

#include <QString>
#include <QList>
#include <QMap>
#include "coordstruct.h"

enum ProjType//有新的投影方式加入可进行扩展
{
	NoneProj,	//无投影定义或未知投影定义
	MercProj,	//墨卡托投影（一般航海中使用，角度不变）
	TMercProj,	//横轴墨卡托投影 GuassKruger UTM
	WebMercProj,//Web墨卡托投影（网络地图投影方式）
	//...
};

//!投影坐标系参数
struct ProjParam
{
	QString _name;	//!<参数名称-例：False_Easting
	double _value;	//!<值-例：0.0

	ProjParam() :_name(""), _value(0){ }
	ProjParam(QString name, double value) :_name(name), _value(value){ }
};

//!投影定义结构体
struct ProjectionDef
{
	QString _name;	//!<投影名称
	QList<ProjParam> _params;//!<参数列表
	//QMap<QString, double> _params;//!<参数列表

	ProjectionDef() :_name(""){ }
	ProjectionDef(QString name, QList<ProjParam> params) :_name(name), _params(params){ }
	//ProjectionDef(QString name, QMap<QString, double> params) :_name(name), _params(params){ }

	//判断是否存在指定名称（不区分大小写）的参数
	bool hasParam(QString pname)
	{
		for (int i = 0; i < _params.size(); ++i)
		{
			if (_params[i]._name.toUpper() == pname.toUpper())
			{
				return true;
			}
		}
		return false;
	}

	//获取指定名称（不区分大小写）的参数值，如果列表中不存在该参数值则ok为false
	double getParam(QString pname, bool *ok = 0)
	{
		if(ok)*ok = false;
		double value = 0;
		for (int i = 0; i < _params.size(); ++i)
		{
			if (_params[i]._name.toUpper() == pname.toUpper())
			{
				if(ok)*ok = true;
				value = _params[i]._value;
				break;
			}
		}
		return value;
	}

	//不区分大小写，如果同名param已存在则更新其值，pname不能为空
	void addParam(QString pname, double pvalue)
	{
		if (pname.trimmed().isEmpty())
		{
			return;
		}
		bool flag = true;
		for (int i = 0; i < _params.size(); ++i)
		{
			if (_params[i]._name.toUpper() == pname.toUpper())
			{
				flag = false;
				_params[i]._value = pvalue;
				break;
			}
		}
		if (flag)
		{
			_params << ProjParam(pname, pvalue);
		}
	}
	void addParam(ProjParam param)
	{
		addParam(param._name, param._value);
	}

	//移除指定名称（不区分大小写）的参数
	void delParam(QString pname)
	{
		for (int i = 0; i < _params.size(); ++i)
		{
			if (_params[i]._name.toUpper() == pname.toUpper())
			{
				_params.removeAt(i);
				--i;
			}
		}
	}
};

class ProjParamsBase
{
public:
	ProjParamsBase();
	virtual ~ProjParamsBase();

	bool isValid() const { return _type != NoneProj; }

	ProjType getProjType() const { return _type; }

	double getFalseEasting() const { return _false_easting; }
	double getFalseNorthing() const { return _false_northing; }

	ProjectionDef getProjectionDef() const { return _projection; }

protected:
	virtual void initParams() = 0;
protected:
	ProjType _type;			//投影类型
	double _false_easting;	//东偏（好像每种投影至少都有这两个参数）
	double _false_northing;	//北偏（好像每种投影至少都有这两个参数）
	ProjectionDef _projection;
};

class MercProjParams :public ProjParamsBase
{
public:
	MercProjParams(double x0 = 0, double y0 = 0, double lon0 = 0, double lat0 = 0);
	MercProjParams(ProjectionDef p);
	virtual ~MercProjParams();

	double getCentralMeridian() const { return _central_meridian; }
	double getStandardParallel1() const{ return _standard_parallel_1; }

private:
	virtual void initParams() override;

private:
	double _central_meridian;
	double _standard_parallel_1;
};

class TMercProjParams :public ProjParamsBase
{
public:
	//!此情况下不讨论TMercType及带宽带号信息
	TMercProjParams(ProjectionDef p);
	TMercProjParams(double x0 = 0, double y0 = 0, double lon0 = 0, double scale = 0, double lat0 = 0, QString name = "Gauss_Kruger");//
	//!根据标准投影命名（下划线连接）解析参数，形如：CGCS2000_3_Degree_GK_Zone_34等（暂时只支持解析2000的高斯投影和84的UTM投影格式名称）
	TMercProjParams(QString prjcsName);
	virtual ~TMercProjParams();

	double getCentralMeridian() const { return _central_meridian; }
	double getScaleFactor() const{ return _scale_factor; }
	double getLatitudeOfOrigin() const{ return _latitude_of_origin; }

	TMercType getTMercType()const{ return _tmerc_type; }
	int getZoneWidth()const{ return _zone_width; }
	int getZoneNumber()const{ return _zone_number; }

private:
	virtual void initParams() override;
	bool parsePrjcsName(const QString& prjcsName);

private:
	TMercType _tmerc_type;//根据参数还没法完全确定TMercType
	double _central_meridian;
	double _scale_factor;
	double _latitude_of_origin;
	int _zone_width;
	int _zone_number;
};

class WebMercProjParams :public ProjParamsBase
{
public:
	WebMercProjParams(double x0 = 0, double y0 = 0, double lon0 = 0, double lat0 = 0, double auxiliary_sphere_type = 0);
	WebMercProjParams(ProjectionDef p);
	virtual ~WebMercProjParams();

	double getCentralMeridian() const { return _central_meridian; }
	double getStandardParallel1() const{ return _standard_parallel_1; }
	double getAuxiliarySphereType() const{ return _auxiliary_sphere_type; }

private:
	virtual void initParams() override;

private:
	double _central_meridian;
	double _standard_parallel_1;
	double _auxiliary_sphere_type;
};

#endif // CS_PROJPARAMS_H
