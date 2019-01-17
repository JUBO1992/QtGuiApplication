#include "mynetmanager.h"
#include <QUrl>
#include <QByteArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QMimeDatabase>
#include <QFileInfo>
#include <QUuid>
#include <iostream>

using namespace std;

MyNetManager::MyNetManager(QNetworkAccessManager *parent) :
m_pManager(parent){
}

MyNetManager::~MyNetManager(){
}

bool MyNetManager::get(QUrl url){

	QNetworkRequest request;
	request.setUrl(url);

	QNetworkReply *reply = m_pManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(finished()));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(downloadProgress(qint64, qint64)));

	return true;
}

bool MyNetManager::post(QUrl url, QByteArray &bytes){
	QNetworkRequest request;
	request.setUrl(url);

	request.setRawHeader("Content-Type", "text/xml; charset=UTF-8");
	//request.setRawHeader("Content-Type", "application/json");
	request.setRawHeader("User-Agent", "iData/iDataMobile");
	request.setRawHeader("Connection", "keep-alive");

	QNetworkReply *reply = m_pManager->post(request, bytes);
	connect(reply, SIGNAL(finished()), this, SLOT(finished()));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(downloadProgress(qint64, qint64)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
		this, SLOT(slotError(QNetworkReply::NetworkError)));

	return true;
}

bool MyNetManager::getFile(QUrl url, const QString &saveFilename)
{
	QNetworkRequest request;
	request.setUrl(url);

	m_FileDownUp.file = new QFile(saveFilename);
	bool res = m_FileDownUp.file->open(QIODevice::WriteOnly);
	if (!res)
		return false;

	QNetworkReply *reply = m_pManager->get(request);

	m_FileDownUp.isDownUpFile = true;
	
	connect(reply, SIGNAL(finished()), this, SLOT(finished()));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
		this, SLOT(downloadProgress(qint64, qint64)));

	connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

	return true;
}

bool MyNetManager::postForm(QUrl url, const QVariantMap &formData, const QString &fileFormKey , const QString &filename )
{
	QNetworkRequest request;
	request.setUrl(url);

	//request.setRawHeader("Accept", "text/html, application/xhtml+xml, image/jxr, */*");
	request.setRawHeader("User-Agent", "iData/iDataMobile");
	//request.setRawHeader("Connection", "keep-alive");

	QMimeDatabase mineDB;

	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	for (auto it = formData.constBegin(); it != formData.constEnd(); it++)
	{
		QHttpPart formPart;
		formPart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(it.key()));
		formPart.setBody(it->toByteArray());

		multiPart->append(formPart);
	}

	if (!filename.isEmpty())
	{
		m_FileDownUp.file = new QFile(filename);
		bool res = m_FileDownUp.file->open(QIODevice::ReadOnly);
		if (!res)
			return false;

		m_FileDownUp.isDownUpFile = true;

		QFileInfo f(filename);
		QMimeType subFileMimeType = mineDB.mimeTypeForFile(filename);
		QString fileMimeType = subFileMimeType.name();

		QHttpPart filePart;
		filePart.setHeader(QNetworkRequest::ContentTypeHeader, fileMimeType);
		filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"; filename=\"%2\"").arg(fileFormKey).arg(f.fileName()));
		filePart.setBodyDevice(m_FileDownUp.file);

		multiPart->append(filePart);
	}

	//Qt官网上说不需要自己设置Boundary，Qt会自动生成，但不少人遇到Qt自动生成的Boundary服务器不识别的问题，因此还要自己设置一下
	QUuid uid = QUuid::createUuid();
	QString bd = uid.toString();
	bd = bd.mid(1, bd.length() - 2);
	bd = bd.remove('-');
	//QString bd = "da895d2b391244adaec2b00b5736dbc0";
	multiPart->setBoundary(bd.toLatin1());
	request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + bd);

	QNetworkReply *reply = m_pManager->post(request, multiPart);
	connect(reply, SIGNAL(finished()), this, SLOT(finished()));
	connect(reply, SIGNAL(uploadProgress(qint64, qint64)),
		this, SLOT(downloadProgress(qint64, qint64)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
		this, SLOT(slotError(QNetworkReply::NetworkError)));

	return true;
}

void MyNetManager::finished(){
	QNetworkReply* reply = (QNetworkReply*)sender();
	if (m_FileDownUp.isDownUpFile)
	{
		//下载或者上传文件结束
		//QString filename = m_FileDownUp.file->fileName();

		m_FileDownUp.file->close();
		delete m_FileDownUp.file;
		m_FileDownUp.file = NULL;
		m_FileDownUp.isDownUpFile = false;

		//emit requestDone(QString("{\"filename\":\"%1\"}").arg(filename).toUtf8());
		emit requestDone(reply->readAll());
	}
	else
	{
		emit requestDone(reply->readAll());
	}
	
	reply->deleteLater();
}

void MyNetManager::downloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	emit requestProgress(bytesSent, bytesTotal);
	//cout << "\ndownloadProgress done:\n";
	//cout << "bytesSent: " << bytesSent
	//	<< "  " << "bytesTocal: " << bytesTotal;
}

void	MyNetManager::slotError(QNetworkReply::NetworkError code)
{
	if (m_FileDownUp.isDownUpFile)
	{
		//下载或者上传文件出错
		m_FileDownUp.isDownUpFile = false;
		if (m_FileDownUp.file != NULL)
		{
			delete m_FileDownUp.file;
			m_FileDownUp.file = NULL;
		}
	}

	emit requestError(code);
}

void MyNetManager::onReadyRead()
{
	QNetworkReply* reply = (QNetworkReply*)sender();
	if (m_FileDownUp.isDownUpFile)
	{
		m_FileDownUp.file->write(reply->readAll());
	}
}

QNetworkAccessManager *MyNetManager::manger()
{
	return m_pManager;
}