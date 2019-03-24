#ifndef MSCOORDTRANS_H
#define MSCOORDTRANS_H
#include "coordtransform_global.h"
#include "proj_api.h"
#include <QString>
#include <QPoint>
#include "mscoordprojdef.h"

//!地理坐标和投影坐标相互转换
class COORDTRANSFORM_EXPORT GeocsProjcsTrans
{
public:
	GeocsProjcsTrans(ProjDefBase* pProjDef);
	virtual ~GeocsProjcsTrans();

	//利用正反算方式转换（地理坐标输入输出均为十进制度）
	bool geocs2projcs_1(double lon, double lat, double&x, double&y)const;
	bool projcs2geocs_1(double x, double y, double&lon, double&lat)const;
	//利用pj_transform转换（地理坐标输入输出均为十进制度）
	bool geocs2projcs_2(double lon, double lat, double&x, double&y)const;
	bool projcs2geocs_2(double x, double y, double&lon, double&lat)const;

private:
	ProjDefBase* _pProjDef;
	projPJ _projpj;			//投影坐标proj定义
	projPJ _geopj;			//地理坐标proj定义
};

//!投影坐标到投影坐标的转换（要求同基准面，否则无法进行转换）
class ProjcsProjcsTrans
{
public:
	ProjcsProjcsTrans(ProjDefBase* srcPrj, ProjDefBase* dstPrj);
	virtual ~ProjcsProjcsTrans();

	//利用正反算分步转换
	bool transform_1(double ix, double iy, double& ox, double& oy) const;
	//利用pj_transform一步转换
	bool transform_2(double ix, double iy, double& ox, double& oy) const;

private:
	ProjDefBase* _pSrcPjDef;
	ProjDefBase* _pDstPjDef;
	projPJ _srcpj;
	projPJ _dstpj;
};

/*!
*	@brief 坐标转换类，实现四参数平面坐标转换和七参数空间坐标转换
*	七参数空间坐标转换主要用于进行不同基准面的坐标转换
*/
class MSCoordTransform
{
public:
	MSCoordTransform();
	virtual ~MSCoordTransform();

	//验证是否可以执行四参数转换
	bool is4pValid() const { return _is4pValid; }
	//验证是否可以执行七参数转换
	bool is7pValid() const { return _is7pValid; }

	void set4p(const FourParams& p){ _4p = p; update4pHermert(); }
	void set7p(const SevenParams& p){ _7p = p; update7pHermert(); }
	void set4p(double x = 0, double y =0, double s = 1, double theta = 0)
	{
		FourParams p;
		p._x = x;
		p._y = y;
		p._s = s;
		p._theta = theta;
		set4p(p);
	}
	void set7p(double x = 0, double y = 0, double z = 0, double rx = 0, double ry = 0, double rz = 0, double s = 0)
	{
		SevenParams p;
		p._x = x;
		p._y = y;
		p._z = z;
		p._rx = rx;
		p._ry = ry;
		p._rz = rz;
		p._s = s;
		set7p(p);
	}

	FourParams get4p() const { return _4p; }
	SevenParams get7p() const { return _7p; }

	/*!
	*	@brief 根据控制点计算四参数（最小二乘）-暂未实现
	*/
	bool calc4p(const QList<QPointF>& srcCtrls, const QList<QPointF>& dstCtrls);
	/*!
	*	@brief 根据控制点计算七参数（最小二乘）-暂未实现
	*	@param[in] srcPrj 起始坐标投影定义
	*	@param[in] dstPrj 目标坐标投影定义
	*	@param[in] srcCtrls 起始投影坐标系下控制点坐标（x,y,H）xy大地高
	*	当srcPrj为NoneProjDef类型时，其坐标为（L,B,H）经纬度大地高
	*	当srcPrj为NULL时，其坐标为（X,Y,Z）三维空间坐标
	*	@param[in] dstCtrls 目标投影坐标系下控制点坐标（x,y,H）xy大地高
	*	当dstPrj为NoneProjDef类型时，其坐标为（L,B,H）经纬度大地高
	*	当dstPrj为NULL时，其坐标为（X,Y,Z）三维空间坐标
	*/
	bool calc7p(const ProjDefBase* srcPrj, const ProjDefBase* dstPrj,
		const QList<QPoint3D>& srcCtrls, const QList<QPoint3D>& dstCtrls);

	//四参数坐标转换，仅当_is4pValid为true时可用
	bool transform4p(double ix, double iy, double& ox, double& oy) const;
	/*
		@brief 设置七参数起始和目标投影定义，需要在进行7参数转换前进行设置
		@param[in] srcPrj
		当srcPrj为投影定义类型时，其坐标为（x,y,H）xy大地高
		当srcPrj为NoneProjDef类型时，其坐标为（L,B,H）经纬度大地高
		当srcPrj为NULL时，其坐标为（X,Y,Z）三维空间坐标
		@param[in] dstPrj
		当dstPrj为投影定义类型时，其坐标为（x,y,H）xy大地高
		当dstPrj为NoneProjDef类型时，其坐标为（L,B,H）经纬度大地高
		当dstPrj为NULL时，其坐标为（X,Y,Z）三维空间坐标
	*/
	void setSrcDstPrj(const ProjDefBase* srcPrj, const ProjDefBase* dstPrj);
	//七参数坐标转换，仅当_is7pValid为true时可用
	bool transform7p(double ix, double iy, double iz, double& ox, double& oy, double& oz) const;

private:
	bool calc7p(const QList<QPoint3D>& srcCtrls, const QList<QPoint3D>& dstCtrls){
		update7pHermert();
		return false;
	}

	void update4pHermert();
	void update7pHermert();

	//三维空间坐标七参数转换
	bool transform7p_xyz(double ix, double iy, double iz, double& ox, double& oy, double& oz) const;

private:
	FourParams _4p;		//四参数
	SevenParams _7p;	//七参数
	bool _is4pValid;
	bool _is7pValid;

	projPJ _4phermert;	//四参Hermert
	projPJ _7phermert;	//七参Hermert

	//以下变量七参数转换时使用
	CoordType _srcCType;
	CoordType _dstCType;
	projPJ _srcpj;
	projPJ _dstpj;
	MSEllipsoid _srcEllip;
	MSEllipsoid _dstEllip;
};

#endif // MSCOORDTRANS_H
