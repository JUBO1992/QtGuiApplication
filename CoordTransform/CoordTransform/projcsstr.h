#ifndef PROJCSSTR_H
#define PROJCSSTR_H

#include "coordtransform_global.h"
#include <QString>
#include <QList>
#include "mscoordstruct.h"
#include "coordsystemstr.h"
#include "geogcsstr.h"

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

	ProjectionDef() :_name(""){ }
	ProjectionDef(QString name, QList<ProjParam> params) :_name(name), _params(params){ }
};

//!投影坐标系字符串生成与解析类
class COORDTRANSFORM_EXPORT ProjcsStr :public CoordSystemStr
{
public:
	ProjcsStr(const QString& qsr);
	ProjcsStr(const DatumType& type, const QString& name, const ProjectionDef& proj);
	ProjcsStr(const GeogcsStr& geogcs, const QString& name, const ProjectionDef& proj);
	virtual ~ProjcsStr();

	//!获得投影坐标系名称
	virtual QString getName() const override{ return _projcs_name; }
	//!获取标准地理坐标系定义字符串
	virtual QString getCoordSystemStr() const override;
	//!获取xml格式坐标系统字符串
	virtual QString getXmlDefStr() const override;

	//!获取投影定义
	ProjectionDef getProjectDef() const { return _projection; }
	//!获取投影名称
	QString getProjectName() const { return _projection._name; }
	//!获取投影参数
	QList<ProjParam> getProjectParams() const { return _projection._params; }

private:
	//!解析地理坐标系字符串，成功返回true，失败返回false
	bool _parseProjcsStr(const QString& qsr);

private:
	GeogcsStr _geogcs;		//!<投影坐标系对应的地理坐标系

	QString _projcs_name;	//!<投影坐标系名称
	ProjectionDef _projection;//!<投影定义内容
};

#endif // PROJCSSTR_H
