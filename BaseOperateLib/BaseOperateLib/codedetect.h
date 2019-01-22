/*! \file codedetect.h
 *	Copyright (c) 2018,广东南方数码科技股份有限公司
 *	All rights reserved.
 *	\brief 文本文件编码方式检测类头文件
 *	参考：https://blog.csdn.net/ShiQW5696/article/details/78923882
 *	\author created by JUBO,bo.ju@southgis.com
 *	\version v1.0
 *	\date 2018年6月5日
 */
#ifndef __CODEDETECT_H__
#define __CODEDETECT_H__

#include <stdio.h>
#include <cstdlib>
#include <string>
#include <windows.h>
#include <tchar.h>
#include <windef.h>
#include "baseoperatelib_global.h"

using namespace std;

/*! \brief 编码格式枚举
*/ 
enum EncodingType {
	ENCODINGTYPE_UNKNOW = 0,	// Unknow
	ENCODINGTYPE_ANSI,			// ANSI
	ENCODINGTYPE_ULE,			// UCS Little Endian
	ENCODINGTYPE_UBE,			// UCS Big Endian
	ENCODINGTYPE_UTF8			// UTF-8 And UTF-8 With BOM
};

class QFile;

class BASEOPERATELIB_EXPORT CodeDetect
{
public:
	CodeDetect();
	~CodeDetect();

	/*! \brief 检测文本文件编码
	 *	\param [in] filepath 文件全路径
	 *	\return 返回文件编码枚举类型
	 */
	static EncodingType GetEncodingType(const QString& filepath);
	static EncodingType GetEncodingType(QFile& file);

private:

	/*! \brief 检测是否为UTF-8无BOM格式编码
	 *	\param [in] pBuffer 文本内容
	 *	\param [in] size 文本的长度
	 *	\return BOOL
	 */
	static BOOL CheckUTF8NoBOM(const void* pBuffer, long size);

	/*! \brief 从文本中获取编码格式
	 *	\param [in] src 文本内容
	 *	\param [in] len 文本的长度
	 *	\return EncodingType
	 */
	static EncodingType GetEncodingTypeFromStr(const void* src, long len);

	static wstring ANSIToUnicode(const string& strANSI);
	static wstring UTF8ToUnicode(const string& strUTF8);
	static string UnicodeToANSI(const wstring& strUnicode);
	static string UnicodeToUTF8(const wstring& strUnicode);

private:
	static wstring StrToWstr(UINT CodePage, const string& str);
	static string WstrToStr(UINT CodePage, const wstring& wStr);
};

#endif // __CODEDETECT_H__
