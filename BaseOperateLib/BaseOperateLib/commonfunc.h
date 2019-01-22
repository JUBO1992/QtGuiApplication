#ifndef COMMONFUNC_H
#define COMMONFUNC_H

#include "baseoperatelib_global.h"
#include "globalconfig.h"

#ifdef __cplusplus
extern "C"{
#endif

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


#ifdef __cplusplus
}
#endif

#endif // COMMONFUNC_H
