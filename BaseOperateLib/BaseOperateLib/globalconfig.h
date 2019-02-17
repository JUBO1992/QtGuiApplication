#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QObject>
#include "baseoperatelib_global.h"

 class BASEOPERATELIB_EXPORT GlobalConfig : public QObject
{
	Q_OBJECT

public:
	GlobalConfig(QObject *parent);
	~GlobalConfig();

	/*
	@brief 设置属性值，key-value形式，key-value中不能有#
	@param[in] key 属性名称
	@param[in] value 属性值
	*/
	static void SetProperty(const QString& key, const QString& value);
	/*
	@brief 获取属性值
	@param[in] key 属性名称
	@return 属性值
	*/
	static QString GetProperty(const QString& key);

private:

};

#endif // GLOBALCONFIG_H
