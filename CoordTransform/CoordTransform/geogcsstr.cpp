#include "geogcsstr.h"

GeogcsStr::GeogcsStr(const QString& qsr)
{
	if (_parseGeogcsStr(qsr))
	{
		_cs_type = GEOGCS;
		_cs_unit = CoordSystemUint(_cs_type);
	}
}

GeogcsStr::GeogcsStr(const DatumType& type)
{
	_geo_datum = GeoDatum(type);
	switch (type)
	{
	case DTUndef:
		_geogcs_name = "Unknown";
		break;
	case WGS84:
		_geogcs_name = "GCS_WGS_1984";
		_wkid = 4326;
		break;
	case CGCS2000:
		_geogcs_name = "GCS_China_Geodetic_Coordinate_System_2000";
		_wkid = 4490;
		break;
	case Xian80:
		_geogcs_name = "GCS_Xian_1980";
		_wkid = 4610;
		break;
	case Beijing54:
		_geogcs_name = "GCS_Beijing_1954";
		_wkid = 4214;
		break;
	default:
		_geogcs_name = "Unknown";
		break;
	}
	_cs_type = GEOGCS;
	_cs_unit = CoordSystemUint(_cs_type);
}

GeogcsStr::GeogcsStr(const QString& name, const GeoDatum& datum)
: _geogcs_name(name)
, _geo_datum(datum)
{
	_cs_type = GEOGCS;
	_cs_unit = CoordSystemUint(_cs_type);
}

GeogcsStr::~GeogcsStr()
{

}

QString GeogcsStr::getCoordSystemStr() const
{
	return QString("GEOGCS[\"%1\",DATUM[\"%2\",SPHEROID[\"%3\", %4, %5]],PRIMEM[\"%6\", %7],UNIT[\"%8\", %9]]")
		.arg(_geogcs_name).arg(_geo_datum._name).arg(_geo_datum._spheriod._name)
		.arg(_geo_datum._spheriod._a, 0, 'f', 1).arg(_geo_datum._spheriod._df, 0, 'f', 10)
		.arg("Greenwich").arg("0.0")
		.arg(_cs_unit._name).arg(_cs_unit._value, 0, 'f', 16);
}

QString GeogcsStr::getXmlDefStr() const
{
	return QString("<SpatialReference xsi:type='typens:%1'><WKT>GEOGCS[&quot;%1&quot;,DATUM[&quot;%2&quot;,SPHEROID[&quot;%3&quot;,%4,%5]],PRIMEM[&quot;%6&quot;,%7],UNIT[&quot;%8&quot;,%9]]</WKT>")
		.arg(_geogcs_name).arg(_geo_datum._name).arg(_geo_datum._spheriod._name)
		.arg(_geo_datum._spheriod._a, 0, 'f', 1).arg(_geo_datum._spheriod._df, 0, 'f', 10)
		.arg("Greenwich").arg("0.0")
		.arg(_cs_unit._name).arg(_cs_unit._value, 0, 'f', 16).arg("GeographicCoordinateSystem");
}

bool GeogcsStr::_parseGeogcsStr(const QString& qsr)
{
	QString sr = qsr;
	if (!sr.contains("GEOGCS", Qt::CaseInsensitive))
	{
		return false;
	}
	int index_begin = 0;
	int index_end = 0;

	//由里到外一层层取
	QString spheroidStr;
	index_begin = sr.indexOf("SPHEROID", 0, Qt::CaseInsensitive);
	index_end = sr.indexOf("]", index_begin);
	spheroidStr = sr.mid(index_begin, index_end - index_begin + 1);
	sr = sr.replace(spheroidStr, "#");

	QString datumStr;
	index_begin = sr.indexOf("DATUM", 0, Qt::CaseInsensitive);
	index_end = sr.indexOf("]", index_begin);
	datumStr = sr.mid(index_begin, index_end - index_begin + 1);
	sr = sr.replace(datumStr, "#");

	QString primemStr;
	index_begin = sr.indexOf("PRIMEM", 0, Qt::CaseInsensitive);
	index_end = sr.indexOf("]", index_begin);
	primemStr = sr.mid(index_begin, index_end - index_begin + 1);
	sr = sr.replace(primemStr, "#");

	QString unitStr;
	index_begin = sr.indexOf("UNIT", 0, Qt::CaseInsensitive);
	index_end = sr.indexOf("]", index_begin);
	unitStr = sr.mid(index_begin, index_end - index_begin + 1);
	sr = sr.replace(unitStr, "#");

	QString geogcsStr;
	index_begin = sr.indexOf("GEOGCS", 0, Qt::CaseInsensitive);
	index_end = sr.indexOf("]", index_begin);
	geogcsStr = sr.mid(index_begin, index_end - index_begin + 1);

	return false;
}
