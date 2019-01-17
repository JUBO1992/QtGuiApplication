#include "ynsdbase.h"
#include <QDomDocument>
#include <QFile>
#include <QMap>
#include <QVariantMap>
#include <QPainterPath>
#include <QJsonDocument>
#include <QJsonObject>

#pragma execution_character_set("utf-8")

//GeometryFactory::unique_ptr factory = GeometryFactory::create();
//extern iDataInterface *g_pInterface;


bool readConfigFile(const QString& file, YNSDPrjInfo& prjInfo)
{
	QDomDocument doc;
	QFile profile(file); //cfgfile为xml文件路径  
	if (!profile.open(QIODevice::ReadOnly))
	{
		return false;
	}
	if (!doc.setContent(&profile))
	{
		profile.close();
		return false;
	}

	QDomElement root = doc.documentElement();
	if ("xmxx" != root.tagName().toLower())//判断是否项目信息文件
	{
		profile.close();
		return false;
	}
	prjInfo._xmid = root.firstChildElement("xmid").text();
	prjInfo._xmmc = root.firstChildElement("xmmc").text();
	prjInfo._xzqmc = root.firstChildElement("xzqmc").text();
	prjInfo._xzqdm = root.firstChildElement("xzqdm").text();
	prjInfo._xmjd = root.firstChildElement("xmjd").text();
	prjInfo._xmms = root.firstChildElement("xmms").text();
	prjInfo._cjsj = root.firstChildElement("cjsj").text();
	prjInfo._cjryid = root.firstChildElement("cjryid").text();
	prjInfo._cjry = root.firstChildElement("cjry").text();

	profile.close();
	return true;
}

bool readDataPakcetXml(const QString& file, QVariantMap& infoMap)
{
	QDomDocument doc;
	QFile profile(file); //file为xml文件路径  
	if (!profile.open(QIODevice::ReadOnly))
	{
		return false;
	}
	if (!doc.setContent(&profile))
	{
		profile.close();
		return false;
	}

	QDomElement root = doc.documentElement();
	if ("datapacket" != root.tagName().toLower())//判断是否datapacket.xml文件
	{
		profile.close();
		return false;
	}
	QString info;
	info = root.firstChildElement("version").text();
	infoMap.insert("version", info);
	info = root.firstChildElement("sbbh").text();
	infoMap.insert("sbbh", info);
	info = root.firstChildElement("scsj").text();
	infoMap.insert("scsj", info);
	info = root.firstChildElement("zyry").text();
	infoMap.insert("zyry", info);
	info = root.firstChildElement("bz").text();
	infoMap.insert("bz", info);

	profile.close();
	return true;
}

void readPackList(const QList<QVariant>& list, QList<DataPackInfo>& packList)
{
	for (int i = 0; i < list.size(); ++i)
	{
		DataPackInfo info;
		QVariantMap item = list.at(i).toMap();
		auto it = item.find("xmid");
		if (it != item.end())
			info._xmid = it.value().toString();
		it = item.find("xmmc");
		if (it != item.end())
			info._xmmc = it.value().toString();
		it = item.find("cjsj");
		if (it != item.end())
			info._cjsj = it.value().toString();
		it = item.find("sjbscsj");
		if (it != item.end())
			info._sjbscsj = it.value().toString();
		it = item.find("sjbdx");
		if (it != item.end())
			info._sjbdx = it.value().toString();
		it = item.find("wjmc");
		if (it != item.end())
			info._wjmc = it.value().toString();
		packList << info;
	}
}

void readCkFileList(const QList<QVariant>& list, QList<CheckFileInfo>& ckFileList)
{
	for (int i = 0; i < list.size(); ++i)
	{
		CheckFileInfo info;
		QVariantMap item = list.at(i).toMap();
		auto it = item.find("wjmc");
		if (it != item.end())
			info._wjmc = it.value().toString();
		it = item.find("scsj");
		if (it != item.end())
			info._scsj = it.value().toString();
		it = item.find("sjbb");
		if (it != item.end())
			info._sjbb = it.value().toString();
		it = item.find("zyry");
		if (it != item.end())
			info._zyry = it.value().toString();
		ckFileList << info;
	}
}


QString VariantMapToString(QVariantMap vtmap)
{
	QJsonDocument doc = QJsonDocument::fromVariant(QVariant(vtmap));
	QByteArray jba = doc.toJson();
	QString jsonString = QString(jba);
	return jsonString;
}

QVariantMap JsonStringToVariantMap(const QString& str)
{
	QByteArray njba = str.toUtf8();
	QJsonObject nobj = QJsonObject(QJsonDocument::fromJson(njba).object());
	QVariantMap query = nobj.toVariantMap();
	return query;
}


///*------------------------------------Geos相关函数-------------------------------------------*/
//
//LinearRing* createGeosLinearRing(const QPoint2dArray& nodes)
//{
//	CoordinateArraySequence *cas = new CoordinateArraySequence(); //构建点序列
//	for (int i = 0; i < nodes.size(); ++i)
//	{
//		cas->add(Coordinate(nodes[i].x(), nodes[i].y()));
//	}
//	LinearRing *lr = NULL;
//	try
//	{
//		lr = factory->createLinearRing(cas);
//	}
//	catch (geos::util::IllegalArgumentException e)
//	{
//		//错误提示不友好，故去掉
//		//g_pInterface->iDataPrintf(e.what());
//		//g_pInterface->iDataPrintf(QString("此点(%1,%2)处有异常图形，请处理！").arg(nodes[0].x(), 0, 'f', 6).arg(nodes[0].y(), 0, 'f', 6));
//	}
//	return lr;
//}
//
//LineString* createGeosLineString(const QPoint2dArray &nodes)
//{
//	CoordinateArraySequence *cas = new CoordinateArraySequence(); //构建点序列
//	for (int i = 0; i < nodes.size(); ++i)
//	{
//		cas->add(Coordinate(nodes[i].x(), nodes[i].y()));
//	}
//	LineString *lr = NULL;
//	try
//	{
//		lr = factory->createLineString(cas);
//	}
//	catch (geos::util::IllegalArgumentException e)
//	{
//		//错误提示不友好，故去掉
//		//g_pInterface->iDataPrintf(e.what());
//		//g_pInterface->iDataPrintf(QString("此点(%1,%2)处有异常图形，请处理！").arg(nodes[0].x(), 0, 'f', 6).arg(nodes[0].y(), 0, 'f', 6));
//	}
//	return lr;
//}
//
////创建Geos简单面对象，返回结果可能为NULL，Polygon
////nodesArr需要排好序且不为多部件
//Polygon* createGeosPolygon(const QNodesArray& nodesArr)
//{
//	if (nodesArr.size() == 0)
//	{
//		return NULL;
//	}
//	LinearRing *lr = createGeosLinearRing(nodesArr[0]);
//	if (lr == NULL)
//	{
//		return NULL;
//	}
//	std::vector<Geometry*>* holes = new std::vector<Geometry*>;//岛
//	for (int i = 1; i < nodesArr.size(); ++i)
//	{
//		LinearRing *templr = createGeosLinearRing(nodesArr[i]);
//		if (templr == NULL)
//		{
//			return NULL;
//		}
//		holes->push_back(templr);
//	}
//	Polygon *poly = factory->createPolygon(lr, holes);
//	if (poly != NULL && (!poly->isValid() || !poly->isSimple()))
//	{
//		Polygon *polybuffer = dynamic_cast<Polygon*>(poly->buffer(0.0));
//		delete poly;
//		return polybuffer;
//	}
//	return  poly;
//}
//
////创建Geos多部件面对象，返回结果可能为NULL，MultiPolygon
////nodesList需要合法，否则可能出现异常
//MultiPolygon* createGeosPolygon(const QList<QNodesArray> nodesList)
//{
//	if (nodesList.size() == 0)
//	{
//		return NULL;
//	}
//	std::vector<Geometry *> newPolys;
//	for (int i = 0; i < nodesList.size(); ++i)
//	{
//		Geometry* geom = createGeosPolygon(nodesList.at(i));
//		if (geom != NULL)
//		{
//			newPolys.push_back(geom);
//		}
//	}
//	MultiPolygon* multiPoly = factory->createMultiPolygon(&newPolys);
//	if (multiPoly != NULL && !multiPoly->isValid())
//	{
//		MultiPolygon *polybuffer = dynamic_cast<MultiPolygon*>(multiPoly->buffer(0.0));
//		delete multiPoly;
//		return polybuffer;
//	}
//	return multiPoly;
//}
//
//Geometry * createGeosGeometry(const QNodesArray& nodesArr)
//{
//	if (nodesArr.size() == 0)
//	{
//		return NULL;
//	}
//	Geometry* poly = NULL;
//	QList<QNodesArray> nodesList;
//	seperateNodes(nodesArr, nodesList);
//	if (nodesList.size() == 1)
//	{
//		poly = createGeosPolygon(nodesList.at(0));
//	}
//	else if (nodesList.size() > 1)
//	{//多部件
//		poly = createGeosPolygon(nodesList);
//	}
//	return  poly;
//}
//
//QPainterPath getPainterPathFromNodes(const QNodesArray& nodesArr)
//{
//	QPainterPath path;
//	for (int i = 0; i < nodesArr.size(); ++i)
//	{
//		for (int j = 0; j < nodesArr[i].size(); ++j)
//		{
//			double x = QString::number(nodesArr[i][j].x(), 'f', 6).toDouble();
//			double y = QString::number(nodesArr[i][j].y(), 'f', 6).toDouble();
//			if (j == 0)
//			{
//				path.moveTo(x, y);
//			}
//			else
//			{
//				path.lineTo(x, y);
//			}
//		}
//	}
//	return path;
//}
//
//QNodesArray getNodesFromGeosPolygon(const Geometry* polygon, const double control/* = 0.001*/)
//{
//	if (polygon == NULL)
//	{
//		return QNodesArray();
//	}
//	QNodesArray nodesout;
//	std::string typestring = polygon->getGeometryType();
//	if (typestring == "MultiPolygon" || typestring == "GeometryCollection")
//	{
//		int mutinum = polygon->getNumGeometries();
//		for (int i = 0; i < mutinum; ++i)
//		{
//			const Geometry *onepoly = polygon->getGeometryN(i);
//			std::string a_type = onepoly->getGeometryType();
//			if (a_type != "Polygon")
//			{
//				continue;
//			}
//
//			Polygon *p_poly = dynamic_cast<Polygon*>(const_cast<Geometry*>(onepoly));
//			const CoordinateSequence* coods = p_poly->getExteriorRing()->getCoordinatesRO();
//			size_t nodesize = coods->size();
//
//			QPoint2dArray nodestmp;
//			for (int j = 0; j < nodesize; ++j)
//			{
//				nodestmp << QPointF(coods->getAt(j).x, coods->getAt(j).y);
//			}
//			if (fabs(GetNodesArea(nodestmp)) <= control)
//			{
//				continue;
//			}
//			nodesout << nodestmp;
//
//			size_t hole_count = p_poly->getNumInteriorRing();
//			for (int j = 0; j < hole_count; j++)
//			{
//				coods = p_poly->getInteriorRingN(j)->getCoordinatesRO();
//				nodesize = coods->size();
//				nodestmp.clear();
//				for (int i = 0; i < nodesize; ++i)
//				{
//					nodestmp << QPointF(coods->getAt(i).x, coods->getAt(i).y);
//				}
//				if (fabs(GetNodesArea(nodestmp)) <= control)
//				{
//					continue;
//				}
//				if (IsDeasil(nodestmp))
//				{
//					reverse(nodestmp);
//				}
//				nodesout << nodestmp;
//			}
//		}
//	}
//	else if (typestring == "Polygon")
//	{
//		Geometry* p_geom = const_cast<Geometry*>(polygon);
//		Polygon* p_poly = dynamic_cast<Polygon*>(p_geom);
//		QNodesArray nodesout;
//		const CoordinateSequence* coods = p_poly->getExteriorRing()->getCoordinatesRO();
//		size_t nodesize = coods->size();
//
//		QPoint2dArray nodestmp;
//		for (int j = 0; j < nodesize; ++j)
//		{
//			nodestmp << QPointF(coods->getAt(j).x, coods->getAt(j).y);
//		}
//		if (fabs(GetNodesArea(nodestmp)) <= control)
//		{
//			return QNodesArray();
//		}
//		nodesout << nodestmp;
//
//		size_t hole_count = p_poly->getNumInteriorRing();
//		for (int j = 0; j < hole_count; j++)
//		{
//			coods = p_poly->getInteriorRingN(j)->getCoordinatesRO();
//			nodesize = coods->size();
//			nodestmp.clear();
//			for (int i = 0; i < nodesize; ++i)
//			{
//				nodestmp << QPointF(coods->getAt(i).x, coods->getAt(i).y);
//			}
//			if (fabs(GetNodesArea(nodestmp)) <= control)
//			{
//				continue;
//			}
//			if (IsDeasil(nodestmp))
//			{
//				reverse(nodestmp);
//			}
//			nodesout << nodestmp;
//		}
//	}
//	return nodesout;
//}
//
//QNodesArray getNodesFromGeosPolyline(const Geometry* polyline)
//{
//	if (polyline == NULL)
//	{
//		return QNodesArray();
//	}
//	QNodesArray nodesout;
//	std::string typestring = polyline->getGeometryType();
//	if (typestring == "MultiLineString" || typestring == "GeometryCollection")
//	{
//		int mutinum = polyline->getNumGeometries();
//		for (int i = 0; i < mutinum; ++i)
//		{
//			Geometry *p_geom = factory->createGeometry(polyline->getGeometryN(i));
//			std::string a_type = p_geom->getGeometryType();
//			if (a_type != "LineString")
//			{
//				delete p_geom;
//				continue;
//			}
//			LineString *p_line = dynamic_cast<LineString*>(p_geom);
//
//			const CoordinateSequence* coods = p_line->getCoordinatesRO();
//			size_t nodesize = coods->size();
//
//			QPoint2dArray nodestmp;
//			for (int j = 0; j < nodesize; ++j)
//			{
//				nodestmp << QPointF(coods->getAt(j).x, coods->getAt(j).y);
//			}
//			nodesout << nodestmp;
//			factory->destroyGeometry(p_geom);
//		}
//	}
//	else if (typestring == "LineString")
//	{
//		Geometry* p_geom = const_cast<Geometry*>(polyline);
//		LineString* p_line = dynamic_cast<LineString*>(p_geom);
//		const CoordinateSequence* coods = p_line->getCoordinatesRO();
//		size_t nodesize = coods->size();
//
//		QPoint2dArray nodestmp;
//		for (int j = 0; j < nodesize; ++j)
//		{
//			nodestmp << QPointF(coods->getAt(j).x, coods->getAt(j).y);
//		}
//		nodesout << nodestmp;
//	}
//	return nodesout;
//}
