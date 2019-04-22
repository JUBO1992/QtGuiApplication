#include "cs_projparams.h"
#include <QStringList>

#pragma execution_character_set("utf-8")

//////////////////////////////////////////////////////////////////////////
//ProjParamsBase
ProjParamsBase::ProjParamsBase()
	: _type(NoneProj)
	, _false_easting(0)
	, _false_northing(0)
{

}

ProjParamsBase::~ProjParamsBase()
{

}

//////////////////////////////////////////////////////////////////////////
//MercProjParams
MercProjParams::MercProjParams(double x0, double y0, double lon0, double lat0)
{
	_type = MercProj;
	_false_easting = x0;
	_false_northing = y0;
	_central_meridian = lon0;
	_standard_parallel_1 = lat0;

	QList<ProjParam> list;
	list << ProjParam("False_Easting", _false_easting);
	list << ProjParam("False_Northing", _false_northing);
	list << ProjParam("Central_Meridian", _central_meridian);
	list << ProjParam("Standard_Parallel_1", _standard_parallel_1);

	_projection = ProjectionDef("Mercator", list);
}

MercProjParams::MercProjParams(ProjectionDef p)
{
	bool isok1, isok2, isok3, isok4;
	_false_easting = p.getParam("False_Easting", &isok1);
	_false_northing = p.getParam("False_Northing", &isok2);
	_central_meridian = p.getParam("Central_Meridian", &isok3);
	_standard_parallel_1 = p.getParam("Standard_Parallel_1", &isok4);

	if (!isok1 || !isok2 || !isok3 || !isok4)
	{
		initParams();
		return;
	}

	_type = MercProj;
	_projection = p;
}

MercProjParams::~MercProjParams()
{

}

void MercProjParams::initParams()
{
	_type = NoneProj;
	_false_easting = 0;
	_false_northing = 0;
	_central_meridian = 0;
	_standard_parallel_1 = 0;
	_projection = ProjectionDef();
}

//////////////////////////////////////////////////////////////////////////
//TMercProjParams
TMercProjParams::TMercProjParams(ProjectionDef p)
{
	initParams();

	bool isok1, isok2, isok3, isok4, isok5;
	_false_easting = p.getParam("False_Easting", &isok1);
	_false_northing = p.getParam("False_Northing", &isok2);
	_central_meridian = p.getParam("Central_Meridian", &isok3);
	_scale_factor = p.getParam("Scale_Factor", &isok4);
	_latitude_of_origin = p.getParam("Latitude_Of_Origin", &isok5);

	if (!isok1 || !isok2 || !isok3 || !isok4 || !isok5)
	{
		initParams();
		return;
	}

	_type = TMercProj;
	_projection = p;

	//if (fabs(_scale_factor - 1) < 1e-6)
	//{
	//	if (_false_easting > 500000)//带带号
	//	{
	//	}
	//	else//不带带号--无法区分是三度带投影还是六度带投影，只能根据投影名称来了
	//	{
	//		_tmerc_type = Custom;
	//	}
	//}
	//else if (fabs(_scale_factor - 0.9996) < 1e-6)
	//{
	//	if (_false_northing < 1)//北半球北偏为0
	//	{
	//		_tmerc_type = UTMNorth;
	//	}
	//	else//南半球北偏为10000000
	//	{
	//		_tmerc_type = UTMSouth;
	//	}
	//}
	//else
	//{
	//	_tmerc_type = Custom;
	//}
}

TMercProjParams::TMercProjParams(QString prjcsName)
{
	initParams();
	if (!parsePrjcsName(prjcsName))
	{
		initParams();
	}
}

TMercProjParams::TMercProjParams(double x0 /*= 0*/, double y0 /*= 0*/, double lon0 /*= 0*/, double scale /*= 0*/, double lat0 /*= 0*/, QString name /*= "Gauss_Kruger"*/)
{
	_type = TMercProj;
	_false_easting = x0;
	_false_northing = y0;
	_central_meridian = lon0;
	_scale_factor = scale;
	_latitude_of_origin = lat0;

	QList<ProjParam> list;
	list << ProjParam("False_Easting", _false_easting);
	list << ProjParam("False_Northing", _false_northing);
	list << ProjParam("Central_Meridian", _central_meridian);
	list << ProjParam("Scale_Factor", _scale_factor);
	list << ProjParam("Latitude_Of_Origin", _latitude_of_origin);

	_projection = ProjectionDef("name", list);
}

TMercProjParams::~TMercProjParams()
{

}

void TMercProjParams::initParams()
{
	_type = NoneProj;
	_tmerc_type = Custom;
	_false_easting = 0;
	_false_northing = 0;
	_central_meridian = 0;
	_scale_factor = 0;
	_latitude_of_origin = 0;
	_projection = ProjectionDef();
	_zone_width = 0;
	_zone_number = 0;
}

bool TMercProjParams::parsePrjcsName(const QString& prjcsName)
{
	if (!prjcsName.startsWith("WGS_1984_UTM",Qt::CaseInsensitive)
		&& !prjcsName.startsWith("CGCS2000", Qt::CaseInsensitive))
	{
		return false;
	}
	//根据_prjcsName解析其他参数
	QStringList params = prjcsName.split("_", QString::SkipEmptyParts);
	//if (params.size() != 4 && params.size() != 6 && params.size() !=5)
	//{
	//	return false;
	//}
	if (params.size() == 4 || params.size() == 6)//2000六度分带、三度分带
	{
		_projection._name = "Gauss_Kruger";
		_false_northing = 0;
		if (params.size() == 4)
		{
			_zone_width = 6;
			if (params.at(2).toLower() == "zone")
			{
				_tmerc_type = GuassK6Zone;
				//形如：CGCS2000_GK_Zone_13
				_zone_number = params.last().toInt();
				_central_meridian = _zone_number * 6 - 3;
				_false_easting = _zone_number*1e6 + 500000;
			}
			else
			{
				_tmerc_type = GuassKruger6;
				//形如：CGCS2000_GK_CM_75E
				QString lon = params.last();
				lon.chop(1);
				_central_meridian = lon.toInt();
				_zone_number = (lon.toInt() + 3) / 6;
				_false_easting = 500000;
			}
		}
		else
		{
			_zone_width = 3;
			if (params.at(4).toLower() == "zone")
			{
				_tmerc_type = GuassK3Zone;
				//CGCS2000_3_Degree_GK_Zone_25
				_zone_number = params.last().toInt();
				_central_meridian = _zone_number * 3;
				_false_easting = _zone_number*1e6 + 500000;
			}
			else
			{
				_tmerc_type = GuassKruger3;
				//CGCS2000_3_Degree_GK_CM_75E
				QString lon = params.last();
				lon.chop(1);
				_central_meridian = lon.toInt();
				_zone_number = lon.toInt() / 3;
				_false_easting = 500000;
			}
		}
	}
	else if (params.size() == 5)//UTM投影
	{
		_projection._name = "Transverse_Mercator";
		_false_easting = 500000;
		_zone_width = 6;//UTM均为6度分带
		QString zone = params.last();
		zone.chop(1);
		_zone_number = zone.toInt();
		_central_meridian = _zone_number * 6 - 183;
		if (prjcsName.endsWith("N"))//北半球
		{
			_tmerc_type = UTMNorth;
			_false_northing = 0;
		}
		else//南半球
		{
			_tmerc_type = UTMSouth;
			_false_northing = 10000000;
		}
	}

	_projection.addParam("False_Easting", _false_easting);
	_projection.addParam("False_Northing", _false_northing);
	_projection.addParam("Central_Meridian", _central_meridian);
	_projection.addParam("Scale_Factor", _scale_factor);
	_projection.addParam("Latitude_Of_Origin", _latitude_of_origin);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//WebMercProjParams
WebMercProjParams::WebMercProjParams(double x0, double y0, double lon0, double lat0, double auxiliary_sphere_type)
{
	_type = WebMercProj;
	_false_easting = x0;
	_false_northing = y0;
	_central_meridian = lon0;
	_standard_parallel_1 = lat0;
	_auxiliary_sphere_type = auxiliary_sphere_type;

	QList<ProjParam> list;
	list << ProjParam("False_Easting", _false_easting);
	list << ProjParam("False_Northing", _false_northing);
	list << ProjParam("Central_Meridian", _central_meridian);
	list << ProjParam("Standard_Parallel_1", _standard_parallel_1);
	list << ProjParam("Auxiliary_Sphere_Type", -_auxiliary_sphere_type);

	_projection = ProjectionDef("Mercator_Auxiliary_Sphere", list);
}

WebMercProjParams::WebMercProjParams(ProjectionDef p)
{
	bool isok1, isok2, isok3, isok4, isok5;
	_false_easting = p.getParam("False_Easting", &isok1);
	_false_northing = p.getParam("False_Northing", &isok2);
	_central_meridian = p.getParam("Central_Meridian", &isok3);
	_standard_parallel_1 = p.getParam("Standard_Parallel_1", &isok4);
	_auxiliary_sphere_type = p.getParam("Auxiliary_Sphere_Type", &isok5);

	if (!isok1 || !isok2 || !isok3 || !isok4 || !isok5)
	{
		initParams();
		return;
	}

	_type = WebMercProj;
	_projection = p;
}

WebMercProjParams::~WebMercProjParams()
{

}

void WebMercProjParams::initParams()
{
	_type = NoneProj;
	_false_easting = 0;
	_false_northing = 0;
	_central_meridian = 0;
	_standard_parallel_1 = 0;
	_auxiliary_sphere_type = 0;
	_projection = ProjectionDef();
}
