#ifndef MYNETMANAGER_H
#define MYNETMANAGER_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include <QFile>

class MyDownUpFile
{
public:
	MyDownUpFile(){ reset(); }
	void reset(){ isDownUpFile = false; file = NULL; }
	bool isDownUpFile;
	QFile *file;
};

class MyNetManager : public QObject
{
	Q_OBJECT

public:
	explicit MyNetManager(QNetworkAccessManager *pManager);
	~MyNetManager();
	bool get(QUrl url);
	bool post(QUrl url, QByteArray& bytes);

	bool getFile(QUrl url, const QString &saveFilename);
	bool postForm(QUrl url, const QVariantMap &formData, const QString &fileFormKey = QString(), const QString &filename = QString());

	QNetworkAccessManager *manger();

signals:
	void requestDone(QByteArray body);
	void requestError(QNetworkReply::NetworkError code);
	void requestProgress(qint64 bytesSent, qint64 bytesTotal);

public slots :
	void downloadProgress(qint64 bytesSent, qint64 bytesTotal);
	void slotError(QNetworkReply::NetworkError code);
	void finished();
	void onReadyRead();


private:
	QNetworkAccessManager *m_pManager;

	MyDownUpFile m_FileDownUp;
};
#endif // MYNETMANAGER_H