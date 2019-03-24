#ifndef MSCOORDSTRUCT_H_
#define MSCOORDSTRUCT_H_

//!坐标类型
enum CoordType
{
	CTUndef,//未定义
	Plane,	//平面(x,y)
	Space,	//空间(x,y,z)
	Geocs,	//地理(L,B,H)
	Prjcs,	//投影(x,y,H)
};

//!基准面类型
enum DatumType
{
	DTUndef,//未定义
	WGS84,
	CGCS2000,
	Xian80,
	Beijing54
};

//!横轴墨卡托投影类型
enum TMercType
{
	Custom,			//自定义
	GuassKruger3,	//高斯克吕格3度投影
	GuassK3Zone,	//高斯克吕格3度投影带带号
	GuassKruger6,	//高斯克吕格6度投影
	GuassK6Zone,	//高斯克吕格6度投影带带号
	UTMNorth,		//北半球UTM投影
	UTMSouth		//南半球UTM投影
};

//!椭球结构体（默认CGCS2000椭球）
struct MSEllipsoid
{
	double _a;		//!<椭球参数：长半轴
	double _df;		//!<椭球参数：扁率倒数！非扁率，与_f对应

	double _f;		//!<椭球参数：扁率！非扁率倒数，与_df对应
	double _b;		//!<椭球参数：短半轴
	double _e12;	//!<椭球参数：第一偏心率平方
	double _e22;	//!<椭球参数：第二偏心率平方

	MSEllipsoid() :_a(6378137.0), _df(298.257222101){ _calcParams(); }
	MSEllipsoid(double a, double df) :_a(a), _df(df){ _calcParams(); }
	MSEllipsoid(DatumType dtype){
		switch (dtype)
		{
		case WGS84:
			_a = 6378137.0;
			_df = 298.257223563;
			break;
		case CGCS2000:
			_a = 6378137.0;
			_df = 298.257222101;
			break;
		case Xian80:
			_a = 6378140.0;
			_df = 298.257;
			break;
		case Beijing54:
			_a = 6378245.0;
			_df = 298.3;
			break;
		default:
			break;
		}
		_calcParams();
	}

private:
	void _calcParams()
	{
		_f = 1 / _df;
		_b = _a*(1 - _f);
		_e12 = 1 - (_b / _a)*(_b / _a);
		_e22 = (_a / _b)*(_a / _b) - 1;
	}
};

//struct QPointF
//{
//	double x;
//	double y;
//
//	QPointF() :x(0), y(0){}
//	QPointF(double _x, double _y) :x(_x), y(_y){}
//	QPointF(QPointF& pt){ x = pt.x; y = pt.y; }
//};

struct QPoint3D
{
	double x;
	double y;
	double z;

	QPoint3D() :x(0), y(0), z(0){}
	QPoint3D(double _x, double _y, double _z) :x(_x), y(_y), z(_z){}
	QPoint3D(QPoint3D& pt){ x = pt.x; y = pt.y; z = pt.z; }
};

//!平面四参数结构体
struct FourParams
{
	double _x;		//平移参数x东
	double _y;		//平移参数y北
	double _s;		//缩放参数，单位为1
	double _theta;	//旋转参数，逆时针为正，单位弧度
	FourParams() :_x(0), _y(0), _s(1), _theta(0){}
};

//!三维七参数结构体
struct SevenParams
{
	double _x;		//平移参数x
	double _y;		//平移参数y
	double _z;		//平移参数z
	double _rx;		//旋转参数，绕x轴，逆时针为正，单位弧度
	double _ry;		//旋转参数，绕y轴，逆时针为正，单位弧度
	double _rz;		//旋转参数，绕z轴，逆时针为正，单位弧度
	double _s;		//缩放参数，单位ppm，真是缩放因子为：1+s*1e-6
	SevenParams() :_x(0), _y(0), _z(0), _rx(0), _ry(0), _rz(0), _s(0){}
};

#endif
