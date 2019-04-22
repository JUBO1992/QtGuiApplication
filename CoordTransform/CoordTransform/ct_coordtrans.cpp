#include "ct_coordtrans.h"

#pragma execution_character_set("utf-8")

GeocsProjcsTrans::GeocsProjcsTrans(ProjDefBase* pProjDef)
	: _pProjDef(pProjDef)
	, _projpj(NULL)
	, _geopj(NULL)
{
	if (!_pProjDef) return;

	QString pjStr = _pProjDef->ProjStr();
	QByteArray pjStr_barr = pjStr.toLatin1();
	if (_projpj) pj_free(_projpj);
	_projpj = pj_init_plus(pjStr_barr.constData());

	pjStr = _pProjDef->GeoStr();
	pjStr_barr = pjStr.toLatin1();
	if (_geopj) pj_free(_geopj);
	_geopj = pj_init_plus(pjStr_barr.constData());
}

GeocsProjcsTrans::~GeocsProjcsTrans()
{
	if (_projpj)
	{
		pj_free(_projpj);
	}
	if (_geopj)
	{
		pj_free(_geopj);
	}
}

bool GeocsProjcsTrans::forwardCoordTrans(double ix, double iy, double& ox, double& oy) const
{
	return geocs2projcs_1(ix, iy, ox, oy);
}

bool GeocsProjcsTrans::inverseCoordTrans(double ix, double iy, double& ox, double& oy) const
{
	return projcs2geocs_1(ix, iy, ox, oy);
}

bool GeocsProjcsTrans::geocs2projcs_1(double lon, double lat, double&x, double&y) const
{
	x = y = 0;
	if (!_projpj)return false;

	projUV temp_BL;
	temp_BL.u = lon*DEG_TO_RAD;//度转弧段
	temp_BL.v = lat*DEG_TO_RAD;//度转弧段

	projUV out_XY;
	out_XY = pj_fwd(temp_BL, _projpj);	//投影正变换(经纬度坐标转投影坐标)
	x = out_XY.u;
	y = out_XY.v;
	return true;
}

bool GeocsProjcsTrans::projcs2geocs_1(double x, double y, double& lon, double& lat) const
{
	lon = lat = 0;
	if (!_projpj)return false;

	projUV in_XY;
	in_XY.u = x;
	in_XY.v = y;

	projUV temp_BL;
	temp_BL = pj_inv(in_XY, _projpj);	//投影逆变换(投影坐标转经纬度坐标) 
	double tempL = temp_BL.u*RAD_TO_DEG;//弧段转度
	double tempB = temp_BL.v*RAD_TO_DEG;//弧段转度
	return true;
}

bool GeocsProjcsTrans::geocs2projcs_2(double lon, double lat, double&x, double&y) const
{
	x = y = 0;
	if (!_projpj || !_geopj)
	{
		return false;
	}
	projUV temp_BL;
	temp_BL.u = lon*DEG_TO_RAD;//度转弧段
	temp_BL.v = lat*DEG_TO_RAD;//度转弧段

	pj_transform(_geopj, _projpj, 1, 1, &lon, &lat, NULL);
	x = lon;
	y = lat;
	return true;
}

bool GeocsProjcsTrans::projcs2geocs_2(double x, double y, double& lon, double& lat) const
{
	lon = lat = 0;
	if (!_projpj || !_geopj)
	{
		return false;
	}
	projUV in_XY;
	in_XY.u = x;
	in_XY.v = y;
	pj_transform(_projpj, _geopj, 1, 1, &x, &y, NULL);
	lon = x*RAD_TO_DEG;//弧段转度
	lat = y*RAD_TO_DEG;//弧段转度

	return true;
}

ProjcsProjcsTrans::ProjcsProjcsTrans(ProjDefBase* srcPrj, ProjDefBase* dstPrj)
	: _pSrcPjDef(srcPrj)
	, _pDstPjDef(dstPrj)
	, _srcpj(NULL)
	, _dstpj(NULL)
{
	if (_pSrcPjDef == _pDstPjDef)
	{//目标和源一致则无需转换
		return;
	}
	if (!_pSrcPjDef || !_pDstPjDef)
	{//目标或源没有设置投影定义
		return;
	}
	if (_pSrcPjDef->GetDatumType() != _pDstPjDef->GetDatumType())
	{//目标和源的基准面不一致
		return;
	}
	
	QString pjStr = _pSrcPjDef->ProjStr();
	QByteArray pjStr_barr = pjStr.toLatin1();
	if (_srcpj) pj_free(_srcpj);
	_srcpj = pj_init_plus(pjStr_barr.constData());

	pjStr = _pDstPjDef->ProjStr();
	pjStr_barr = pjStr.toLatin1();
	if (_dstpj) pj_free(_dstpj);
	_dstpj = pj_init_plus(pjStr_barr.constData());
}

ProjcsProjcsTrans::~ProjcsProjcsTrans()
{
	if (_srcpj)
	{
		pj_free(_srcpj);
	}
	if (_dstpj)
	{
		pj_free(_dstpj);
	}

}

bool ProjcsProjcsTrans::forwardCoordTrans(double ix, double iy, double& ox, double& oy) const
{
	return transform_1(ix, iy, ox, oy);
}

bool ProjcsProjcsTrans::inverseCoordTrans(double ix, double iy, double& ox, double& oy) const
{
	return inverse_1(ix, iy, ox, oy);
}

bool ProjcsProjcsTrans::transform_1(double ix, double iy, double& ox, double& oy) const
{
	ox = oy = 0.0;
	if (_srcpj == NULL || _dstpj == NULL)
	{
		return false;
	}

	projUV in_XY;
	in_XY.u = ix;
	in_XY.v = iy;

	projUV temp_BL;
	temp_BL = pj_inv(in_XY, _srcpj);	//投影逆变换(投影坐标转经纬度坐标) 
	//double tempL = temp_BL.u*RAD_TO_DEG;//弧段转度
	//double tempB = temp_BL.v*RAD_TO_DEG;//弧段转度

	projUV out_XY;
	out_XY = pj_fwd(temp_BL, _dstpj);	//投影正变换(经纬度坐标转投影坐标) 
	ox = out_XY.u;
	oy = out_XY.v;
	return true;
}

bool ProjcsProjcsTrans::inverse_1(double ix, double iy, double& ox, double& oy) const
{
	ox = oy = 0.0;
	if (_srcpj == NULL || _dstpj == NULL)
	{
		return false;
	}

	projUV in_XY;
	in_XY.u = ix;
	in_XY.v = iy;

	projUV temp_BL;
	temp_BL = pj_inv(in_XY, _dstpj);	//投影逆变换(投影坐标转经纬度坐标) 
	//double tempL = temp_BL.u*RAD_TO_DEG;//弧段转度
	//double tempB = temp_BL.v*RAD_TO_DEG;//弧段转度

	projUV out_XY;
	out_XY = pj_fwd(temp_BL, _srcpj);	//投影正变换(经纬度坐标转投影坐标) 
	ox = out_XY.u;
	oy = out_XY.v;
	return true;

}

bool ProjcsProjcsTrans::transform_2(double ix, double iy, double& ox, double& oy) const
{
	ox = oy = 0.0;
	if (_srcpj == NULL || _dstpj == NULL)
	{
		return false;
	}
	projUV in_XY;
	in_XY.u = ix;
	in_XY.v = iy;

	pj_transform(_srcpj, _dstpj, 1, 1, &ix, &iy, NULL);
	ox = ix;
	oy = iy;
	return true;
}

bool ProjcsProjcsTrans::inverse_2(double ix, double iy, double& ox, double& oy) const
{
	ox = oy = 0.0;
	if (_srcpj == NULL || _dstpj == NULL)
	{
		return false;
	}
	projUV in_XY;
	in_XY.u = ix;
	in_XY.v = iy;

	pj_transform(_dstpj, _srcpj, 1, 1, &ix, &iy, NULL);
	ox = ix;
	oy = iy;
	return true;
}

MSCoordTransform::MSCoordTransform()
	: _srcpj(NULL)
	, _dstpj(NULL)
	, _is4pValid(false)
	, _is7pValid(false)
	, _4phermert(NULL)
	, _7phermert(NULL)
	, _srcCType(CoordType::Prjcs)
	, _dstCType(CoordType::Prjcs)
{
	update4pHermert();
	update7pHermert();
}

MSCoordTransform::~MSCoordTransform()
{
	if (_srcpj) pj_free(_srcpj);
	if (_dstpj) pj_free(_dstpj);
	if (_4phermert) pj_free(_4phermert);
	if (_7phermert) pj_free(_7phermert);
}

bool MSCoordTransform::calc4p(const QList<QPointF>& srcCtrls, const QList<QPointF>& dstCtrls)
{
	update4pHermert();
	return false;
}

bool MSCoordTransform::calc7p(const ProjDefBase* srcPrj, const ProjDefBase* dstPrj, const QList<QPoint3D>& srcCtrls, const QList<QPoint3D>& dstCtrls)
{
	return calc7p(srcCtrls, dstCtrls);
}

void MSCoordTransform::update4pHermert()
{
	QString helmert = QString("+proj=helmert +convention=coordinate_frame +x=%1 +y=%2 +s=%3 +theta=%4")
		.arg(_4p._x, 0, 'f', 20).arg(_4p._y, 0, 'f', 20).arg(_4p._s, 0, 'f', 20).arg(_4p._theta, 0, 'f', 20);
	QByteArray barr = helmert.toLatin1();
	if (_4phermert) pj_free(_4phermert);
	_4phermert = pj_init_plus(barr.constData());
	if (_4phermert)
		_is4pValid = true;
	else
		_is4pValid = false;
}

void MSCoordTransform::update7pHermert()
{
	QString helmert = QString("+proj=helmert +convention=coordinate_frame +x=%1 +y=%2 +z=%3 +rx=%4 +ry=%5 +rz=%6 +s=%7")
		.arg(_7p._x, 0, 'f', 20).arg(_7p._y, 0, 'f', 20).arg(_7p._z, 0, 'f', 20)
		.arg(_7p._rx, 0, 'f', 20).arg(_7p._ry, 0, 'f', 20).arg(_7p._rz, 0, 'f', 20).arg(_7p._s, 0, 'f', 20);
	QByteArray barr = helmert.toLatin1();
	if (_7phermert) pj_free(_7phermert);
	_7phermert = pj_init_plus(barr.constData());
}

bool MSCoordTransform::transform4p(double ix, double iy, double& ox, double& oy) const
{
	ox = oy = 0;//先置空
	if (!_is4pValid) return false;
	projUV in_XY;
	in_XY.u = ix;
	in_XY.v = iy;
	projUV out_XY;
	out_XY = pj_fwd(in_XY, _4phermert);	//正算 
	ox = out_XY.u;
	oy = out_XY.v;
	return true;
}

void MSCoordTransform::setSrcDstPrj(const ProjDefBase* srcPrj, const ProjDefBase* dstPrj)
{
	_is7pValid = false;

	if (!srcPrj) _srcCType = Space;
	else if (srcPrj->ProjStr().isEmpty())
	{
		_srcCType = Geocs;
		_srcEllip = srcPrj->GetEllipsoid();
	}
	else
	{
		_srcCType = Prjcs;
		QString pjStr = srcPrj->ProjStr();
		QByteArray pjStr_barr = pjStr.toLatin1();
		if (_srcpj) pj_free(_srcpj);
		_srcpj = pj_init_plus(pjStr_barr.constData());
		if (!_srcpj)return;
		_srcEllip = srcPrj->GetEllipsoid();
	}

	if (!dstPrj) _dstCType = Space;
	else if (dstPrj->ProjStr().isEmpty())
	{
		_dstCType = Geocs;
		_dstEllip = dstPrj->GetEllipsoid();
	}
	else
	{
		_dstCType = Prjcs;
		QString pjStr = dstPrj->ProjStr();
		QByteArray pjStr_barr = pjStr.toLatin1();
		if (_dstpj) pj_free(_dstpj);
		_dstpj = pj_init_plus(pjStr_barr.constData());
		if (!_dstpj)return;
		_dstEllip = dstPrj->GetEllipsoid();
	}
	_is7pValid = true;
}

bool MSCoordTransform::transform7p(double ix, double iy, double iz, double& ox, double& oy, double& oz) const
{
	ox = oy = oz = 0;//先置空
	if (!_is7pValid) return false;
	//将ix,iy,iz转换为XYZ三维空间坐标
	double iX = 0, iY = 0, iZ = 0;
	if (_srcCType == Prjcs)
	{
		projUV in_XY;
		in_XY.u = ix;
		in_XY.v = iy;
		projUV out_XY;
		out_XY = pj_inv(in_XY, _srcpj);	// 反算
		ix = out_XY.u;// *RAD_TO_DEG;
		iy = out_XY.v;// *RAD_TO_DEG;
		pj_geodetic_to_geocentric(_srcEllip._a, _srcEllip._e12, 1, 1, &ix, &iy, &iz);
		iX = ix;
		iY = iy;
		iZ = iz;
	}
	else if (_srcCType == Geocs)
	{
		pj_geodetic_to_geocentric(_srcEllip._a, _srcEllip._e12, 1, 1, &ix, &iy, &iz);
		iX = ix;
		iY = iy;
		iZ = iz;
	}
	else if (_srcCType == Space)
	{
		iX = ix;
		iY = iy;
		iZ = iz;
	}
	//进行七参数三维空间转换
	double oX = 0, oY = 0, oZ = 0;
	if (!transform7p_xyz(iX, iY, iZ, oX, oY, oZ))
		return false;
	//转换得到的XYZ三维空间坐标转为目标坐标
	if (_dstCType == Space)
	{
		ox = oX;
		oy = oY;
		oz = oZ;
	}
	else if (_dstCType == Geocs)
	{
		pj_geocentric_to_geodetic(_dstEllip._a, _dstEllip._e12, 1, 1, &oX, &oY, &oZ);
		ox = oX*RAD_TO_DEG;
		oy = oY*RAD_TO_DEG;
		oz = oZ;
	}
	else if (_dstCType == Prjcs)
	{
		pj_geocentric_to_geodetic(_dstEllip._a, _dstEllip._e12, 1, 1, &oX, &oY, &oZ);
		projUV in_XY;
		in_XY.u = oX;// *RAD_TO_DEG;
		in_XY.v = oY;// *RAD_TO_DEG;
		projUV out_XY;
		out_XY = pj_fwd(in_XY, _dstpj);	// 正算
		ox = out_XY.u;
		oy = out_XY.v;
		oz = oZ;
	}
	return true;
}

bool MSCoordTransform::transform7p_xyz(double ix, double iy, double iz, double& ox, double& oy, double& oz) const
{
	ox = oy = oz = 0;
	projUVW in_XYZ;
	in_XYZ.u = ix;
	in_XYZ.v = iy;
	in_XYZ.w = iz;
	projUVW out_XYZ = in_XYZ;
	out_XYZ = pj_fwd3d(in_XYZ, _7phermert);	//正算
	ox = out_XYZ.u;
	oy = out_XYZ.v;
	oz = out_XYZ.w;
	return true;
}
