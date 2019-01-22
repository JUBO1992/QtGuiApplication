#include "commonfunc.h"

BASEOPERATELIB_EXPORT void SetProperty(const QString& key, const QString& value)
{
	GlobalConfig::SetProperty(key, value);
}

BASEOPERATELIB_EXPORT QString GetProperty(const QString& key)
{
	return GlobalConfig::GetProperty(key);
}
