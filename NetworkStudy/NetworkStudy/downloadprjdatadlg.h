#pragma once

#include <QDialog>

#include "mysoap.h"
#include "ynsdbase.h"

namespace Ui { class DownloadPrjDataDlg; };
class QListWidgetItem;

class DownloadPrjDataDlg : public QDialog, public MySoapManagerCallback
{
	Q_OBJECT

public:
	enum DataDlgPage
	{
		DloadPacketPage,	//下载项目包页面
		DloadCkFilePage,	//下载检查文件页面
		UloadPacketPage		//上传项目包页面
	};

public:
	DownloadPrjDataDlg(QWidget *parent = Q_NULLPTR, DataDlgPage pageMode = DataDlgPage::DloadPacketPage);
	~DownloadPrjDataDlg();

	virtual void onNetworkResponse(QString body);
	virtual void onNetworkError(QNetworkReply::NetworkError code);
	virtual void onNetworkdownloadProgress(qint64 bytesSent, qint64 bytesTotal);

private slots:
	void on_pushButton_close_clicked();
	void on_pushButton_download_clicked();
	void on_pushButton_reflash_clicked();
	void on_listWidget_itemClicked(QListWidgetItem *item);
	void on_listWidget_2_itemClicked(QListWidgetItem *item);
	void on_tabWidget_currentChanged(int idx);
	void on_pushButton_upload_clicked();
	void on_pushButton_scan_clicked();

private:
	void processPrjListRequest(const QVariantMap& map);
	void processDownloadDataPacket(const QVariantMap& map);
	void processUploadDataPacket(const QVariantMap& map);
	void processCkFileRequest(const QVariantMap& map);
	void processDownloadCkFile(const QVariantMap& map);

	void updateWnd();
	void updateListWidget();
	void updateUploadPage(const QString& packpath);

	void showEvent(QShowEvent *);

	//创建datapacket.xml
	bool createDataPacketXml(const QString& dirpath);
	//项目包打包
	bool compressZip();

private:
	Ui::DownloadPrjDataDlg *ui;
	bool _isbusy;
	YNSDNetWorkEnum m_requestType;

	QList<DataPackInfo> m_packInfoList;
	DataPackInfo m_curPackInfo;

	QList<CheckFileInfo> m_ckFileInfoList;
	CheckFileInfo m_curCkInfo;

	DataDlgPage m_pageMode;

	QString m_curDownloadFilepath;

	YNSDPrjInfo m_curPrjInfo;
};
