#include "commonfunc.h"
#include "globalconfig.h"
#include "fileoperate.h"

BASEOPERATELIB_EXPORT EncodingType GetEncodingType0(const QString& filepath)
{
	return CodeDetect::GetEncodingType(filepath);
}

BASEOPERATELIB_EXPORT EncodingType GetEncodingType1(QFile& file)
{
	return CodeDetect::GetEncodingType(file);
}

BASEOPERATELIB_EXPORT void SetProperty(const QString& key, const QString& value)
{
	GlobalConfig::SetProperty(key, value);
}

BASEOPERATELIB_EXPORT QString GetProperty(const QString& key)
{
	return GlobalConfig::GetProperty(key);
}

BASEOPERATELIB_EXPORT bool CopyFileOrDir(const QString& from, const QString& to, bool coverFileIfExist /*= true*/)
{
	return FileOperate::CopyFileOrDir(from, to, coverFileIfExist);
}

BASEOPERATELIB_EXPORT bool DelFileOrDir(const QString& path)
{
	return FileOperate::DelFileOrDir(path);
}

BASEOPERATELIB_EXPORT QStringList SearchFilesInDir(const QString& folder)
{
	return FileOperate::SearchFilesInDir(folder);
}
