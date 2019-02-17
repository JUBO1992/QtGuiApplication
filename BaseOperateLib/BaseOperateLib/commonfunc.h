#ifndef COMMONFUNC_H
#define COMMONFUNC_H

#include "baseoperatelib_global.h"
#include <QStringList>
#include "codedetect.h"

#ifdef __cplusplus
extern "C"{
#endif

/*! \brief 检测文本文件编码
 *	\param [in] filepath 文件全路径
 *	\return 返回文件编码枚举类型
 */
BASEOPERATELIB_EXPORT EncodingType GetEncodingType0(const QString& filepath);
BASEOPERATELIB_EXPORT EncodingType GetEncodingType1(QFile& file);

/*
	@brief 设置属性值，key-value形式，key-value中不能有#
	@param[in] key 属性名称
	@param[in] value 属性值
*/
BASEOPERATELIB_EXPORT void SetProperty(const QString& key, const QString& value);

/*
	@brief 获取属性值
	@param[in] key 属性名称
	@return 属性值
*/
BASEOPERATELIB_EXPORT QString GetProperty(const QString& key);

/*! \brief 拷贝文件或目录
 *  \param [in] from源文件或目录
 *  \param [in] to目标文件或目录
 *  \param [in] coverFileIfExist是否覆盖
 *  \return 返回是否成功
 */
BASEOPERATELIB_EXPORT bool CopyFileOrDir(const QString& from, const QString& to, bool coverFileIfExist = true);

/*! \brief 删除文件或目录
 *  \param [in] path文件或目录路径
 *  \return 返回是否成功
 */
BASEOPERATELIB_EXPORT bool DelFileOrDir(const QString& path);

/*! \brief 搜索目录下所有文件
 *  \param [in] folder搜索路径
 *  \return 返回搜索到的文件列表
 */
BASEOPERATELIB_EXPORT QStringList SearchFilesInDir(const QString& folder);


#ifdef __cplusplus
}
#endif

#endif // COMMONFUNC_H
