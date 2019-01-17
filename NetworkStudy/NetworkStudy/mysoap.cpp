#include "mysoap.h"

#include <QNetworkProxy>
#include <QDomDocument>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

MySoapManager::MySoapManager(QObject *parent)
{
	_callback = NULL;

	QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
	_manger = new MyNetManager(netManager);
	connect(_manger, SIGNAL(requestDone(QByteArray)), this, SLOT(onResponse(QByteArray)));
	connect(_manger, SIGNAL(requestError(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
	connect(_manger, SIGNAL(requestProgress(qint64, qint64)), this, SLOT(onProgress(qint64, qint64)));

	m_loop = new QEventLoop(this);
}

MySoapManager::~MySoapManager()
{
	_manger->deleteLater();
}

void MySoapManager::setHost(const QString &host, int port)
{
	_url.setHost(host);
	_url.setPort(port);
	_url.setScheme("http");
}

void MySoapManager::setProxy(const QString &host, int port)
{
	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::HttpProxy);
	proxy.setHostName(host);
	proxy.setPort(port);

	_manger->manger()->setProxy(proxy);
}

bool MySoapManager::commonGet(const QString &fullUrl, bool blocking)
{
	m_resultXML.clear();
	m_errorMsg.clear();
	m_errorOccur = false;

	//_url.setPath("url"); //setPath会把？变成%3F，请求会失败
	_manger->get(QUrl(fullUrl));

	if (blocking)
	{
		m_loop->exec();

		if (m_errorOccur)
			return false;//发生了错误
	}

	return true;
}

bool MySoapManager::commonGetFile(const QString &fullUrl, const QString &saveFilename)
{
	m_resultXML.clear();
	m_errorMsg.clear();
	m_errorOccur = false;

	//_url.setPath("url"); //setPath会把？变成%3F，请求会失败
	return _manger->getFile(QUrl(fullUrl), saveFilename);
}

bool MySoapManager::commonPost(const QString &path, const QString &parameter, bool blocking)
{
	m_resultXML.clear();
	m_errorMsg.clear();
	m_errorOccur = false;

	_url.setPath(path);
	_manger->post(_url, parameter.toUtf8());

	if (blocking)
	{
		m_loop->exec();

		if (m_errorOccur)
			return false;//发生了错误
	}

	return true;
}

bool MySoapManager::uploadFileByForm(const QString &fullUrl, const QVariantMap &formData, const QString &fileFormKey, const QString &filename)
{
	m_resultXML.clear();
	m_errorMsg.clear();
	m_errorOccur = false;

	//_url.setPath("url"); //setPath会把？变成%3F，请求会失败
	return _manger->postForm(QUrl(fullUrl), formData, fileFormKey, filename);
}

QString MySoapManager::getLastError()
{
	return m_errorMsg;
}

bool MySoapManager::isErrorOccurred()
{
	return m_errorOccur;
}

void MySoapManager::onResponse(QByteArray body)
{
	m_resultXML = QString::fromUtf8(body);

	if (m_loop->isRunning())
		m_loop->exit();

	if (_callback != NULL)
		_callback->onNetworkResponse(m_resultXML);
}

void MySoapManager::onError(QNetworkReply::NetworkError code)
{
	m_errorOccur = true;

	switch (code)
	{
	case QNetworkReply::NoError:
		m_errorMsg.clear();
		break;
	case QNetworkReply::ConnectionRefusedError:
		m_errorMsg = "远程服务器拒绝连接";
		break;
	case QNetworkReply::RemoteHostClosedError:
		m_errorMsg = "远程服务器回复之前关闭了连接";
		break;
	case QNetworkReply::HostNotFoundError:
		m_errorMsg = "未找到远程服务器(地址无效)";
		break;
	case QNetworkReply::TimeoutError:
		m_errorMsg = "连接超时，服务器无法访问";
		break;
	case QNetworkReply::OperationCanceledError:
		m_errorMsg = "操作被取消";
		break;
	case QNetworkReply::SslHandshakeFailedError:
		m_errorMsg = "SSL握手失败，无法建立加密通道。";
		break;
	case QNetworkReply::TemporaryNetworkFailureError:
		m_errorMsg = "网络连接已重置。";
		break;
	case QNetworkReply::NetworkSessionFailedError:
		m_errorMsg = "由于网络断开或网络启动失败，连接断开。";
		break;
	case QNetworkReply::BackgroundRequestNotAllowedError:
		m_errorMsg = "由于平台策略，当前不允许后台请求。";
		break;
	case QNetworkReply::UnknownNetworkError:
		m_errorMsg = "未知的网络相关错误";
		break;
	case QNetworkReply::ProxyConnectionRefusedError:
		m_errorMsg = "与代理服务器的连接被拒绝（代理服务器不接受请求）";
		break;
	case QNetworkReply::ProxyConnectionClosedError:
		m_errorMsg = "代理服务器提前关闭了连接";
		break;
	case QNetworkReply::ProxyNotFoundError:
		m_errorMsg = "未找到代理服务器（代理服务器地址无效）";
		break;
	case QNetworkReply::ProxyTimeoutError:
		m_errorMsg = "代理服务器连接超时";
		break;
	case QNetworkReply::ProxyAuthenticationRequiredError:
		m_errorMsg = "代理服务器身份验证失败";
		break;
	case QNetworkReply::UnknownProxyError:
		m_errorMsg = "未知的代理相关错误";
		break;
	case QNetworkReply::ContentAccessDenied:
		m_errorMsg = "对远程内容的访问被拒绝（类似于HTTP错误401）";
		break;
	case QNetworkReply::ContentOperationNotPermittedError:
		m_errorMsg = "不允许的请求操作";
		break;
	case QNetworkReply::ContentNotFoundError:
		m_errorMsg = "在服务器上找不到指定内容（类似于HTTP错误404）";
		break;
	case QNetworkReply::AuthenticationRequiredError:
		m_errorMsg = "服务器身份验证失败";
		break;
	case QNetworkReply::ContentReSendError:
		m_errorMsg = "该请求需要再次发送。";
		break;
	case QNetworkReply::ContentConflictError:
		m_errorMsg = "由于与资源的当前状态的冲突，请求无法完成。";
		break;
	case QNetworkReply::ContentGoneError:
		m_errorMsg = "请求的资源在服务器上不再可用。";
		break;
	case QNetworkReply::UnknownContentError:
		m_errorMsg = "与远程内容相关的未知错误";
		break;
	case QNetworkReply::ProtocolUnknownError:
		m_errorMsg = "网络协议未知";
		break;
	case QNetworkReply::ProtocolInvalidOperationError:
		m_errorMsg = "所请求的操作对该网络协议无效";
		break;
	case QNetworkReply::ProtocolFailure:
		m_errorMsg = "检测到协议故障（解析错误，无效或意外的响应等）";
		break;
	case QNetworkReply::InternalServerError:
		m_errorMsg = "服务器发生意外的内部错误。";
		break;
	case QNetworkReply::OperationNotImplementedError:
		m_errorMsg = "服务器不支持完成请求所需的功能。";
		break;
	case QNetworkReply::ServiceUnavailableError:
		m_errorMsg = "服务器无法处理该请求。";
		break;
	case QNetworkReply::UnknownServerError:
		m_errorMsg = "与服务器响应相关的未知错误";
		break;
	default:
		m_errorMsg = "未知错误";
		break;
	}

	if (m_loop->isRunning())
		m_loop->exit();

	if (_callback != NULL)
		_callback->onNetworkError(code);
}

void MySoapManager::onProgress(qint64 bytesSent, qint64 bytesTotal)
{
	if (_callback != NULL)
		_callback->onNetworkdownloadProgress(bytesSent, bytesTotal);
}

void MySoapManager::quitUpload()
{
	m_errorOccur = true;
	m_errorMsg = "手动退出上传";
	if (m_loop->isRunning())
		m_loop->exit();
}
