#include "projcsstr.h"

ProjcsStr::ProjcsStr(const QString& qsr)
:_geogcs(DTUndef)
{
	if (_parseProjcsStr(qsr))
	{
		_cs_type = PROJCS;
		_cs_unit = CoordSystemUint(_cs_type);
	}
}

ProjcsStr::ProjcsStr(const DatumType& type, const QString& name, const ProjectionDef& proj)
: _geogcs(type)
, _projcs_name(name)
, _projection(proj)
{
	_cs_type = PROJCS;
	_cs_unit = CoordSystemUint(_cs_type);
}

ProjcsStr::ProjcsStr(const GeogcsStr& geogcs, const QString& name, const ProjectionDef& proj)
: _geogcs(geogcs)
, _projcs_name(name)
, _projection(proj)
{
	_cs_type = PROJCS;
	_cs_unit = CoordSystemUint(_cs_type);
}

ProjcsStr::~ProjcsStr()
{

}

QString ProjcsStr::getCoordSystemStr() const
{
	QString geocsStr = _geogcs.getCoordSystemStr();
	QString projectionStr = QString("PROJECTION[\"%1\"],").arg(_projection._name);
	for (int i = 0; i < _projection._params.size(); ++i)
	{
		ProjParam param = _projection._params[i];
		projectionStr += QString("PARAMETER[\"%1\", %2],").arg(param._name).arg(param._value, 0, 'f', 1);
	}
	projectionStr.chop(1);
	QString unitStr = QString("UNIT[\"%1\", %2]").arg(_cs_unit._name).arg(_cs_unit._value, 0, 'f', 1);
	return QString("PROJCS[\"%1\",%2,%3,%4]")
		.arg(_projcs_name).arg(geocsStr).arg(projectionStr).arg(unitStr);
}

QString ProjcsStr::getXmlDefStr() const
{
	QString geocsStr = QString("GEOGCS[&quot;%1&quot;,DATUM[&quot;%2&quot;,SPHEROID[&quot;%3&quot;,%4,%5]],PRIMEM[&quot;%6&quot;,%7],UNIT[&quot;%8&quot;,%9]]")
		.arg(_geogcs.getName()).arg(_geogcs.getDatumName()).arg(_geogcs.getSpheroidName())
		.arg(_geogcs.getSpheroidA(), 0, 'f', 1).arg(_geogcs.getSpheroidDf(), 0, 'f', 10)
		.arg("Greenwich").arg("0.0")
		.arg(_geogcs.getUnit()._name).arg(_geogcs.getUnit()._value, 0, 'f', 16);
	QString projectionStr = QString("PROJECTION[&quot;%1&quot;],").arg(_projection._name);
	for (int i = 0; i < _projection._params.size(); ++i)
	{
		ProjParam param = _projection._params[i];
		projectionStr += QString("PARAMETER[&quot;%1&quot;,%2],").arg(param._name).arg(param._value, 0, 'f', 1);
	}
	projectionStr.chop(1);
	QString unitStr = QString("UNIT[&quot;%6&quot;,%7]").arg(_cs_unit._name).arg(_cs_unit._value, 0, 'f', 1);
	
	return QString("<SpatialReference xsi:type='typens:%5'><WKT>PROJCS[&quot;%1&quot;,%2,%3,%4]</WKT>")
		.arg(_projcs_name).arg(geocsStr).arg(projectionStr).arg(unitStr).arg("ProjectedCoordinateSystem");
}

bool ProjcsStr::_parseProjcsStr(const QString& qsr)
{
	QString sr = qsr;
	if (!sr.contains("PROJCS", Qt::CaseInsensitive))
	{
		return false;
	}

	CSParamObject * obj = parseQStr2CSPObj(qsr);
	QString str = parseCSPObj2QStr(obj);
	delete obj;
	obj = NULL;

	return false;
}
