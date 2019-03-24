#ifndef COORDSYSTEMSTR_H
#define COORDSYSTEMSTR_H
#include "coordtransform_global.h"
#include <QStringList>
#include "mscoordstruct.h"

//!坐标系统类型
enum CoordSystemType
{
	NONECS = 0,	//!<无定义或未知定义
	GEOGCS = 1,	//!<地理坐标系
	PROJCS = 2,	//!<投影坐标系
};
//!坐标系统单位定义
struct CoordSystemUint
{
	QString _name;	//单位名称
	double _value;	//值

	CoordSystemUint() :_name(""), _value(0){ }
	CoordSystemUint(CoordSystemType type){
		_name = "";
		_value = 0;
		switch (type)
		{
		case NONECS:
			break;
		case GEOGCS:
			_name = "Degree";
			_value = 0.0174532925199433;//16位小数
			break;
		case PROJCS:
			_name = "Meter";
			_value = 1.0;
			break;
		default:
			break;
		}
	}
	CoordSystemUint(QString name, double value) :_name(name), _value(value){ }
};
//!坐标系统参数对象（GEOGCS、DATUM、SPHEROID等都对应一个CSParamObject）
struct CSParamObject
{
	QString _mark;			//!<标记，例如：GEOGCS
	QString _name;			//!<名称，例如：
	CSParamObject* _parent;	//!<父亲节点，为空表示是根节点
	QList<CSParamObject*> _children;//!<兄弟节点，为空表示是叶子节点
	QStringList _values;	//!<具体值列表，只有叶子节点才存在

	CSParamObject() :_parent(NULL){ }
	~CSParamObject()
	{
		for (int i = 0; i < _children.size(); ++i)
		{
			if (_children[i])
			{
				delete _children[i];
				_children[i] = NULL;
			}
		}
		//qDeleteAll(_children);
	}
};

//!坐标系字符串生成与解析基类
class COORDTRANSFORM_EXPORT CoordSystemStr
{
public:
	CoordSystemStr();
	virtual ~CoordSystemStr();

	//!坐标系统名称
	virtual QString getName() const = 0;
	//!标准坐标系统字符串
	virtual QString getCoordSystemStr() const = 0;
	//!xml格式坐标系统字符串
	virtual QString getXmlDefStr() const = 0;
	
	//!坐标系统定义是否有效
	bool isValid() const { return _cs_type != NONECS; }

	void setWKID(int wkid){ _wkid = wkid; }
	int getWKID() const { return _wkid; }

	CoordSystemType getType() const { return _cs_type; }
	CoordSystemUint getUnit() const { return _cs_unit; }

protected:
	int _wkid;					//!<EPSG WKID值
	CoordSystemType _cs_type;	//!<坐标系类型
	CoordSystemUint _cs_unit;	//!<坐标系单位
};

//!将坐标系字符串解析为CSParamObject对象
CSParamObject* parseQStr2CSPObj(const QString& str);
//!将CSParamObject对象转换为坐标系字符串
QString parseCSPObj2QStr(const CSParamObject* obj);

#endif // COORDSYSTEMSTR_H
