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

	//key-value中不能有#
	static void SetProperty(const QString& key, const QString& value);
	static QString GetProperty(const QString& key);

private:

};

#endif // GLOBALCONFIG_H
