#include "urlconfig.h"
#include <QString>
#include <QSettings>
#include <QApplication>

QString s_login_url = "";
QString s_get_data_list_url = "http://120.79.63.81:8080/api/project/datapacket/list?user=%1";
QString s_download_data_url = "http://120.79.63.81:8080/api/project/datapacket/download/%2?user=%1";
QString s_upload_data_url = "http://120.79.63.81:8080/api/project/datapacket/upload/%2?user=%1";
QString s_get_file_list_url = "http://120.79.63.81:8080/api/project/check/%2/list?user=%1";
QString s_download_file_url = "http://120.79.63.81:8080/api/project/check/%2/download?wjmc=%3&user=%1";

void loadUrlConfigureFile(const QString &filename)
{
	//QSettings setting(qApp->applicationDirPath() + "/system/ynbigdataconf.ini", QSettings::IniFormat);
	QSettings setting(filename, QSettings::IniFormat);
	setting.beginGroup("config");

	QString datalist = setting.value("getDataList").toString();
	if (!datalist.isEmpty())
	{
		s_get_data_list_url = datalist.replace("{user}", "%1");
	}

	QString download = setting.value("downloadData").toString();
	if (!download.isEmpty())
	{
		download = download.replace("{user}", "%1");
		s_download_data_url = download.replace("{xmid}", "%2");
	}

	QString upload = setting.value("uploadData").toString();
	if (!upload.isEmpty())
	{
		upload = upload.replace("{user}", "%1");
		s_upload_data_url = upload.replace("{xmid}", "%2");
	}

	QString filelist = setting.value("getFileList").toString();
	if (!filelist.isEmpty())
	{
		filelist = filelist.replace("{user}", "%1");
		s_get_file_list_url = filelist.replace("{xmid}", "%2");
	}

	QString dloadfile = setting.value("downloadFile").toString();
	if (!dloadfile.isEmpty())
	{
		dloadfile = dloadfile.replace("{user}", "%1");
		dloadfile = dloadfile.replace("{xmid}", "%2");
		s_download_file_url = dloadfile.replace("{wjmc}", "%3");
	}

	setting.endGroup();
}