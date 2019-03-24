#include "mscoordsystemstr.h"
#include <QStringList>
#pragma execution_character_set("utf-8")


MSCoordSystemStr::MSCoordSystemStr()
{
	initParams();
}

MSCoordSystemStr::~MSCoordSystemStr()
{

}

void MSCoordSystemStr::initParams()
{
	_coordsystemStr = "";
	_geocsStr = QString("GEOGCS[\"GCS_China_Geodetic_Coordinate_System_2000\",DATUM[\"D_China_2000\",SPHEROID[\"CGCS2000\", 6378137.0, 298.257222101]],PRIMEM[\"Greenwich\", 0.0],UNIT[\"Degree\", 0.0174532925199433]]");
	_xmldefStr = "";

	_geocsName = "GCS_China_Geodetic_Coordinate_System_2000";
	_datumName = "D_China_2000";
	_ellipseName = "CGCS2000";
	_a = 6378137.0;
	_f = 298.257222101;

	_prjcsName = "";
	_prjctionName = "";
	_centralMedian = 0;
	_falseEast = 0;
	_falseNorth = 0;

	_zoneWidth = 0;
	_zoneNumber = 0;
}

bool MSCoordSystemStr::parsePrjcsName(const QString& prjcsName)
{
	initParams();
	//根据_prjcsName解析其他参数
	QStringList params = prjcsName.split("_", QString::SkipEmptyParts);
	if (params.size() != 4 && params.size() != 6)
	{
		return false;
	}
	_prjcsName = prjcsName;
	if (params.size() == 4)//六度分带
	{
		_zoneWidth = 6;
		if (params.at(2).toLower() == "zone")
		{
			//形如：CGCS2000_GK_Zone_13
			_zoneNumber = params.last().toInt();
			_centralMedian = _zoneNumber * 6 - 3;
			_falseEast = _zoneNumber*1e6 + 500000;
		}
		else
		{
			//形如：CGCS2000_GK_CM_75E
			QString lon = params.last();
			lon.chop(1);
			_centralMedian = lon.toInt();
			_zoneNumber = (lon.toInt() + 3) / 6;
			_falseEast = 500000;
		}
	}
	else if (params.size() == 6)//三度分带
	{
		_zoneWidth = 3;
		if (params.at(4).toLower() == "zone")
		{
			//CGCS2000_3_Degree_GK_Zone_25
			_zoneNumber = params.last().toInt();
			_centralMedian = _zoneNumber * 3;
			_falseEast = _zoneNumber*1e6 + 500000;
		}
		else
		{
			//CGCS2000_3_Degree_GK_CM_75E
			QString lon = params.last();
			lon.chop(1);
			_centralMedian = lon.toInt();
			_zoneNumber = lon.toInt() / 3;
			_falseEast = 500000;
		}
	}
	_prjctionName = "Gauss_Kruger";
	_falseNorth = 0;
	return true;
}

void MSCoordSystemStr::SetCoordSystemParams(const QString& prjcsName)
{
	if (!parsePrjcsName(prjcsName))
	{
		return;
	}

	_coordsystemStr = QString("PROJCS[\"%1\",GEOGCS[\"GCS_China_Geodetic_Coordinate_System_2000\",DATUM[\"D_China_2000\",SPHEROID[\"CGCS2000\", 6378137.0, 298.257222101]],PRIMEM[\"Greenwich\", 0.0],UNIT[\"Degree\", 0.0174532925199433]],PROJECTION[\"%2\"],PARAMETER[\"False_Easting\", %3],PARAMETER[\"False_Northing\", %4],PARAMETER[\"Central_Meridian\", %5],PARAMETER[\"Scale_Factor\", 1],PARAMETER[\"Latitude_Of_Origin\", 0.0],UNIT[\"Meter\", 1.0]]")
		.arg(_prjcsName).arg(_prjctionName).arg(_falseEast).arg(_falseNorth).arg(_centralMedian);
}

void MSCoordSystemStr::SetCoordSystemParams(const QString& prjctionName, double centralMedian, double falseEast /*= 0*/, double falseNorth /*= 0*/)
{
	initParams();
	_prjcsName = "USER_DEFINED";
	_prjctionName = prjctionName;
	_centralMedian = centralMedian;
	_falseEast = falseEast;
	_falseNorth = falseNorth;

	_coordsystemStr = QString("PROJCS[\"%1\",GEOGCS[\"GCS_China_Geodetic_Coordinate_System_2000\",DATUM[\"D_China_2000\",SPHEROID[\"CGCS2000\", 6378137.0, 298.257222101]],PRIMEM[\"Greenwich\", 0.0],UNIT[\"Degree\", 0.0174532925199433]],PROJECTION[\"%2\"],PARAMETER[\"False_Easting\", %3],PARAMETER[\"False_Northing\", %4],PARAMETER[\"Central_Meridian\", %5],PARAMETER[\"Scale_Factor\", 1],PARAMETER[\"Latitude_Of_Origin\", 0.0],UNIT[\"Meter\", 1.0]]")
		.arg(_prjcsName).arg(_prjctionName).arg(_falseEast).arg(_falseNorth).arg(_centralMedian);
}

QString MSCoordSystemStr::GetXmldefStr()
{
	if (_coordsystemStr.isEmpty())
		return QString();
	QString sPrimer = QString();
	sPrimer += QString("PARAMETER[&quot;%1&quot;,%2],").arg("Central_Meridian").arg(_centralMedian, 0, 'f', 1);
	sPrimer += QString("PARAMETER[&quot;%1&quot;,%2],").arg("False_Easting").arg(_falseEast, 0, 'f', 1);
	sPrimer += QString("PARAMETER[&quot;%1&quot;,%2],").arg("False_Northing").arg(_falseNorth, 0, 'f', 1);
	sPrimer += QString("PARAMETER[&quot;%1&quot;,%2],").arg("Latitude_Of_Origin").arg(0.0, 0, 'f', 1);
	sPrimer += QString("PARAMETER[&quot;%1&quot;,%2],").arg("Scale_Factor").arg(1.0, 0, 'f', 1);
	QString sDefs = QString("GEOGCS[&quot;%1&quot;,DATUM[&quot;%2&quot;,SPHEROID[&quot;%3&quot;,%4,%5]],PRIMEM[&quot;%6&quot;,%7],UNIT[&quot;%8&quot;,%9]]")
		.arg(_geocsName).arg(_datumName).arg(_ellipseName).arg("6378137.0").arg("298.257222101").arg("Greenwich").arg("0.0").arg("Degree").arg("0.0174532925199433");
	return QString("<SpatialReference xsi:type='typens:%1'><WKT>PROJCS[&quot;%2&quot;,%3,PROJECTION[&quot;%4&quot;],%5UNIT[&quot;%6&quot;,%7]]</WKT>")
		.arg("ProjectedCoordinateSystem").arg(_prjcsName).arg(sDefs).arg(_prjctionName).arg(sPrimer).arg("Meter").arg(1.0, 0, 'f', 12);
}

void MSCoordSystemStr::SetCoordSystemStr(const QString& qsr)
{
	initParams();
	_coordsystemStr = qsr;
	//根据qsr解析其他参数
	QString sr = qsr;
	QString prjcsName = "";
	if (!sr.contains("PROJCS", Qt::CaseInsensitive))
	{
		return;
	}
	int index_begin = sr.indexOf("PROJCS[", 0, Qt::CaseInsensitive) + 7;
	int index_end = sr.indexOf(",", index_begin);
	prjcsName = sr.mid(index_begin, index_end - index_begin);
	prjcsName.remove("\"");
	if (!parsePrjcsName(prjcsName))
	{
		_prjcsName = prjcsName;

		index_begin = sr.indexOf("GEOGCS[", index_end, Qt::CaseInsensitive) + 7;
		index_end = sr.indexOf(",", index_begin);
		_geocsName = sr.mid(index_begin, index_end - index_begin);
		_geocsName.remove("\"");

		index_begin = sr.indexOf("DATUM[", index_end, Qt::CaseInsensitive) + 6;
		index_end = sr.indexOf(",", index_begin);
		_datumName = sr.mid(index_begin, index_end - index_begin);
		_datumName.remove("\"");

		index_begin = sr.indexOf("SPHEROID[", index_end, Qt::CaseInsensitive) + 9;
		index_end = sr.indexOf(",", index_begin);
		_ellipseName = sr.mid(index_begin, index_end - index_begin);
		_ellipseName.remove("\"");

		index_begin = sr.indexOf("PROJECTION[", index_end, Qt::CaseInsensitive) + 11;
		index_end = sr.indexOf("]", index_begin);
		_prjctionName = sr.mid(index_begin, index_end - index_begin);
		_prjctionName.remove("\"");

		index_begin = sr.indexOf("False_Easting", index_end, Qt::CaseInsensitive);
		index_begin = sr.indexOf(",", index_begin) + 1;
		index_end = sr.indexOf("]", index_begin);
		QString tempStr = sr.mid(index_begin, index_end - index_begin).trimmed();
		_falseEast = tempStr.toDouble();

		index_begin = sr.indexOf("False_Northing", index_end, Qt::CaseInsensitive);
		index_begin = sr.indexOf(",", index_begin) + 1;
		index_end = sr.indexOf("]", index_begin);
		tempStr = sr.mid(index_begin, index_end - index_begin).trimmed();
		_falseNorth = tempStr.toDouble();

		index_begin = sr.indexOf("Central_Meridian", index_end, Qt::CaseInsensitive);
		index_begin = sr.indexOf(",", index_begin) + 1;
		index_end = sr.indexOf("]", index_begin);
		tempStr = sr.mid(index_begin, index_end - index_begin).trimmed();
		_centralMedian = tempStr.toDouble();

		_zoneWidth = 0;
		_zoneNumber = 0;
	}
}
