#ifndef MYCXFSOAPMANAGER_H
#define MYCXFSOAPMANAGER_H

#include <QObject>
#include <QEventLoop>
#include "mynetmanager.h"

class MySoapManagerCallback
{
public:
	~MySoapManagerCallback(){}
	virtual void onNetworkResponse(QString body) = 0;
	virtual void onNetworkError(QNetworkReply::NetworkError code) = 0;
	virtual void onNetworkdownloadProgress(qint64 bytesSent, qint64 bytesTotal) = 0;
};

class MySoapManager : public QObject
{
	Q_OBJECT

public:
	explicit MySoapManager(QObject *parent);
	~MySoapManager();

	void setHost(const QString &host, int port = 80);
	void setProxy(const QString &host, int port);

	bool commonGet(const QString &fullUrl, bool blocking = true);
	bool commonGetFile(const QString &fullUrl, const QString &saveFilename);
	bool commonPost(const QString &path, const QString &parameter, bool blocking = true);

	bool uploadFileByForm(const QString &fullUrl, const QVariantMap &formData, const QString &fileFormKey, const QString &filename);

	QString getLastError();
	bool isErrorOccurred();

	void setCallback(MySoapManagerCallback *callback = NULL) { _callback = callback; }

private slots:
	void onResponse(QByteArray body);
	void onError(QNetworkReply::NetworkError code);
	void onProgress(qint64 bytesSent, qint64 bytesTotal);

public slots:
	void quitUpload();//退出上传的阻塞，错误为：手动取消上传

private:
	MyNetManager *_manger;
	QUrl _url;

	QEventLoop *m_loop;

	QString m_resultXML;

	bool m_errorOccur;
	QString m_errorMsg;

	MySoapManagerCallback *_callback;
};
#endif // MYCXFSOAPMANAGER_H