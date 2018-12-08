#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QObject>
#include <QMap>

class GlobalConfig : public QObject
{
	Q_OBJECT

public:
	GlobalConfig(QObject *parent);
	~GlobalConfig();

	static void SetProperty(QString key, QString value);
	static QString GetProperty(QString key);

private:

};

#endif // GLOBALCONFIG_H
