#include "downloadprjdatadlg.h"
#include "ui_downloadprjdatadlg.h"
//#include "idatainterface.h"
#include "urlconfig.h"
//#include "compresstool.h"
#include <QDomDocument>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QShowEvent>
#include <QTextCodec>


//#include "irxMain.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

extern MySoapManager *g_NetworkInterface;
extern QString s_current_login_username;
extern QString g_current_packetpath;

DownloadPrjDataDlg::DownloadPrjDataDlg(QWidget *parent, DataDlgPage pageMode)
	: QDialog(parent)
{
	ui = new Ui::DownloadPrjDataDlg();
	ui->setupUi(this);

	_isbusy = false;
	m_requestType = YNSDNetWorkEnum::UnknownRequest;

	g_NetworkInterface->setCallback(this);

	ui->tabWidget->setCurrentIndex(0);
	m_pageMode = pageMode;
	updateWnd();
}

DownloadPrjDataDlg::~DownloadPrjDataDlg()
{
	g_NetworkInterface->setCallback(NULL);

	delete ui;
}

void DownloadPrjDataDlg::onNetworkResponse(QString body)
{
	QVariantMap map;
	if (body.startsWith("{") && body.endsWith("}"))
	{
		map = JsonStringToVariantMap(body);
	}
	qDebug() << map;
	if (m_requestType == YNSDNetWorkEnum::GetProjectList)
	{
		processPrjListRequest(map);
	}
	else if (m_requestType == YNSDNetWorkEnum::DownloadDataPacket)
	{
		processDownloadDataPacket(map);
	}
	else if (m_requestType == YNSDNetWorkEnum::UploadDataPacket)
	{
		processUploadDataPacket(map);
	}
	else if (m_requestType == YNSDNetWorkEnum::GetCkFileList)
	{
		processCkFileRequest(map);
	}
	else if (m_requestType == YNSDNetWorkEnum::DownloadCkFile)
	{
		processDownloadCkFile(map);
	}
	_isbusy = false;
	this->ui->progressBar->setMaximum(100);
	this->ui->progressBar->setValue(0);
}

void DownloadPrjDataDlg::onNetworkError(QNetworkReply::NetworkError code)
{
	//g_pInterface->iDataPrintf("Error");
	_isbusy = false;
	this->ui->progressBar->setMaximum(100);
	this->ui->progressBar->setValue(0);
}

void DownloadPrjDataDlg::onNetworkdownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	if (bytesTotal == 0)
		return;

	this->ui->progressBar->setMaximum(bytesTotal);
	this->ui->progressBar->setValue(bytesSent);
}

void DownloadPrjDataDlg::on_pushButton_close_clicked()
{
	this->close();
}

void DownloadPrjDataDlg::on_pushButton_download_clicked()
{
	if (_isbusy)
	{
		//g_pInterface->iDataPrintf("请求正在处理中，请稍后...");
		return;
	}

	//新建一个当前日期的目录
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString todaydirpath = YNSD_TEMPPATH + "/" + current_date_time.toString("yyyyMMdd");
	if (!QDir().exists(todaydirpath))
	{
		QDir().mkpath(todaydirpath);
	}

	QString name = s_current_login_username;
	if (m_pageMode == DataDlgPage::DloadPacketPage)
	{
		if (m_curPackInfo._xmid.isEmpty())
		{
			QMessageBox::information(this, "提示", "请先选择需要下载的项目！", QMessageBox::Ok, QMessageBox::Ok);
			return;
		}

		m_requestType = YNSDNetWorkEnum::DownloadDataPacket;
		QString xmid = m_curPackInfo._xmid;
		QString savefilename = todaydirpath + "/" + m_curPackInfo._wjmc;
		m_curDownloadFilepath = savefilename;
		g_NetworkInterface->commonGetFile(s_download_data_url.arg(name).arg(xmid), savefilename);
	}
	else if (m_pageMode == DataDlgPage::DloadCkFilePage)
	{
		if (m_curCkInfo._wjmc.isEmpty())
		{
			QMessageBox::information(this, "提示", "请先选择需要下载的文件！", QMessageBox::Ok, QMessageBox::Ok);
			return;
		}
		m_requestType = YNSDNetWorkEnum::DownloadCkFile;
		QString xmid = m_curPackInfo._xmid;
		QString wjmc = m_curCkInfo._wjmc;
		QString savefilename = todaydirpath + "/" + m_curCkInfo._wjmc;
		m_curDownloadFilepath = savefilename;
		g_NetworkInterface->commonGetFile(s_download_file_url.arg(name).arg(xmid).arg(wjmc), savefilename);
	}
	_isbusy = true;
}

void DownloadPrjDataDlg::on_pushButton_reflash_clicked()
{
	if (_isbusy)
	{
		//g_pInterface->iDataPrintf("请求正在处理中，请稍后...");
		return;
	}

	QString name = s_current_login_username;

	if (m_pageMode == DataDlgPage::DloadPacketPage)
	{
		m_requestType = YNSDNetWorkEnum::GetProjectList;
		g_NetworkInterface->commonGet(s_get_data_list_url.arg(name), false);
	}
	else if (m_pageMode == DataDlgPage::DloadCkFilePage)
	{
		QString xmid = m_curPackInfo._xmid;
		if (xmid.isEmpty())
		{
			QMessageBox::information(this, "提示", "请先选择项目！", QMessageBox::Ok, QMessageBox::Ok);
			return;
		}
		m_requestType = YNSDNetWorkEnum::GetCkFileList;
		g_NetworkInterface->commonGet(s_get_file_list_url.arg(name).arg(xmid), false);
	}
	_isbusy = true;
}

void DownloadPrjDataDlg::on_listWidget_itemClicked(QListWidgetItem *item)
{
	QModelIndex curIdx = ui->listWidget->currentIndex();
	int row = curIdx.row();
	if (row >= 0 && row < m_packInfoList.size())
	{
		DataPackInfo packinfo = m_packInfoList.at(row);
		m_curPackInfo = packinfo;
	}
	else
	{
		m_curPackInfo = DataPackInfo();
	}
	ui->lineEdit_name->setText(m_curPackInfo._xmmc);
	ui->lineEdit_create_time->setText(m_curPackInfo._cjsj);
	ui->lineEdit_data_time->setText(m_curPackInfo._sjbscsj);
	ui->lineEdit_size->setText(m_curPackInfo._sjbdx);
}

void DownloadPrjDataDlg::on_listWidget_2_itemClicked(QListWidgetItem *item)
{
	QModelIndex curIdx = ui->listWidget_2->currentIndex();
	int row = curIdx.row();
	if (row >= 0 && row < m_ckFileInfoList.size())
	{
		CheckFileInfo fileinfo = m_ckFileInfoList.at(row);
		m_curCkInfo = fileinfo;
	}
	else
	{
		m_curCkInfo = CheckFileInfo();
	}
	ui->lineEdit_wjmc->setText(m_curCkInfo._wjmc);
	ui->lineEdit_scsj->setText(m_curCkInfo._scsj);
	ui->lineEdit_sjbb->setText(m_curCkInfo._sjbb);
	ui->lineEdit_zyry->setText(m_curCkInfo._zyry);
}

void DownloadPrjDataDlg::on_tabWidget_currentChanged(int idx)
{
	int index = ui->tabWidget->currentIndex();
	if (index == 0)
	{
		m_pageMode = DataDlgPage::DloadPacketPage;
	}
	else if (index == 1)
	{
		m_pageMode = DataDlgPage::DloadCkFilePage;
	}
	updateWnd();
}

void DownloadPrjDataDlg::on_pushButton_upload_clicked()
{
	if (_isbusy)
	{
		//g_pInterface->iDataPrintf("请求正在处理中，请稍后...");
		return;
	}

	//先打包数据，再进行上传
	QString name = s_current_login_username;
	QString xmid = m_curPrjInfo._xmid;
	if (xmid.isEmpty())
	{
		QMessageBox::information(this, "提示", "请选择有效的项目包路径！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	QString packpath = ui->lineEdit_packetpath->text();
	if (!createDataPacketXml(packpath))
	{
		QMessageBox::information(this, "提示", "datapacket.xml文件创建失败！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	//如果重命名成功说明说明文件夹没有被占用
	QString uploadfpath = packpath + ".zip";
	if (QDir().rename(packpath, packpath + "abcdefg"))
	{
		QDir().rename(packpath + "abcdefg", packpath);
		if (QFileInfo(uploadfpath).exists())
		{//如果包已经存在则删除旧包重新打包
			QFile::remove(uploadfpath);
		}
		bool isSuccess = false;// CompressTool::compress(uploadfpath, packpath, CompressTool::Level_5, CompressTool::Type_ZIP);
		if (!isSuccess)
		{
			QMessageBox::information(this, "提示", "文件打包失败！", QMessageBox::Ok, QMessageBox::Ok);
			return;
		}
	}
	else
	{
		QMessageBox::information(this, "提示", "请先关闭当前项目包内所有文件，否则会导致打包不成功！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
	//g_NetworkInterface->setProxy("127.0.0.1", 8888);
	m_requestType = YNSDNetWorkEnum::UploadDataPacket;
	g_NetworkInterface->uploadFileByForm(s_upload_data_url.arg(name).arg(xmid), QVariantMap(), "file", uploadfpath);

	_isbusy = true;
}

void DownloadPrjDataDlg::on_pushButton_scan_clicked()
{
	QString tempdir = "";// g_pInterface->property("YNSD_PRJPACK_PATH").toString();
	tempdir = g_current_packetpath.isEmpty() ? tempdir : g_current_packetpath;
	QString packpath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"),
		tempdir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (packpath.isEmpty())
	{
		return;
	}
	//g_pInterface->setProperty("YNSD_PRJPACK_PATH", packpath);
	updateUploadPage(packpath);
}

void DownloadPrjDataDlg::processPrjListRequest(const QVariantMap& map)
{
	auto it = map.find("status");
	if (it != map.end())
	{
		QString status = it.value().toString();
		if (status == "801")
		{//表示获取成功
			it = map.find("respData");
			if (it == map.end())
				it = map.find("respdata");
			if (it != map.end())
			{
				QList<QVariant> items = it.value().toList();
				m_packInfoList.clear();
				readPackList(items, m_packInfoList);
				updateListWidget();
			}
		}
		else
		{
			it = map.find("messages");
			QString errmsg;
			if (it != map.end())
			{
				errmsg = it.value().toString();
			}
			//g_pInterface->iDataPrintf(QString("状态码：%1；错误信息：%2").arg(status).arg(errmsg));
		}
	}
	else
	{
		//g_pInterface->iDataPrintf("Error");
	}
}

void DownloadPrjDataDlg::processDownloadDataPacket(const QVariantMap& map)
{
	QString zipfile = m_curDownloadFilepath;
	if (QFileInfo(zipfile).exists())
	{
		QString unzippath = zipfile.left(zipfile.lastIndexOf("."));
		bool flag = false;// CompressTool::uncompress(zipfile, unzippath);
		if (!flag)
		{
			//g_pInterface->iDataPrintf("数据包解压出错！");
		}
		else
		{
			g_current_packetpath = unzippath;
			//g_pInterface->setProperty("YNSD_PRJPACK_PATH", unzippath);
			//g_pInterface->iDataPrintf(QString("下载成功！文件路径：\n%1").arg(zipfile));
			QMessageBox::information(this, "提示", "数据包下载成功！", QMessageBox::Ok, QMessageBox::Ok);
		}
	}
	else
	{
		//g_pInterface->iDataPrintf("数据包下载失败！");
	}
}

void DownloadPrjDataDlg::processUploadDataPacket(const QVariantMap& map)
{
	auto it = map.find("status");
	if (it != map.end())
	{
		QString status = it.value().toString();
		it = map.find("messages");
		QString msg;
		if (it != map.end())
		{
			msg = it.value().toString();
		}
		//g_pInterface->iDataPrintf(QString("状态码：%1，返回信息：%2").arg(status).arg(msg));
		if (status == "801")
		{
			//表示上传成功
			QMessageBox::information(this, "提示", "文件上传成功", QMessageBox::Ok, QMessageBox::Ok);
		}
	}
	else
	{
		//g_pInterface->iDataPrintf("Error");
	}
}

void DownloadPrjDataDlg::processCkFileRequest(const QVariantMap& map)
{
	auto it = map.find("status");
	if (it != map.end())
	{
		QString status = it.value().toString();
		if (status == "801")
		{//表示获取成功
			it = map.find("respData");
			if (it == map.end())
				it = map.find("respdata");
			if (it != map.end())
			{
				QList<QVariant> items = it.value().toList();
				m_ckFileInfoList.clear();
				readCkFileList(items, m_ckFileInfoList);
				updateListWidget();
			}
		}
		else
		{
			it = map.find("messages");
			QString errmsg;
			if (it != map.end())
			{
				errmsg = it.value().toString();
			}
			//g_pInterface->iDataPrintf(QString("状态码：%1；错误信息：%2").arg(status).arg(errmsg));
		}
	}
	else
	{
		//g_pInterface->iDataPrintf("Error");
	}
}

void DownloadPrjDataDlg::processDownloadCkFile(const QVariantMap& map)
{
	QString ckfile = m_curDownloadFilepath;
	if (QFileInfo(ckfile).exists())
	{
		//g_pInterface->iDataPrintf(QString("下载成功！文件路径：\n%1").arg(ckfile));
		QDesktopServices::openUrl(QUrl::fromLocalFile(ckfile));
		//QMessageBox::information(this, "提示", "文件下载成功！", QMessageBox::Ok, QMessageBox::Ok);
	}
	else
	{
		//g_pInterface->iDataPrintf("文件下载失败！");
	}
}

void DownloadPrjDataDlg::updateWnd()
{
	if (m_pageMode == DataDlgPage::DloadPacketPage)
	{
		setWindowTitle("下载数据包");
		ui->stackedWidget->setCurrentIndex(0);
		ui->pushButton_reflash->setText("更新项目列表");
		ui->pushButton_download->setText("下载选中项目");
	}
	else if (m_pageMode == DataDlgPage::DloadCkFilePage)
	{
		setWindowTitle("下载数据包");
		ui->stackedWidget->setCurrentIndex(0);
		ui->pushButton_reflash->setText("更新文件列表");
		ui->pushButton_download->setText("下载选中文件");
	}
	else if (m_pageMode == DataDlgPage::UloadPacketPage)
	{
		setWindowTitle("上传数据包");
		ui->stackedWidget->setCurrentIndex(1);
		QString packpath = g_current_packetpath;
		updateUploadPage(packpath);
	}
}

void DownloadPrjDataDlg::updateListWidget()
{
	ui->listWidget->clear();
	for (int i = 0; i < m_packInfoList.size(); ++i)
	{
		QListWidgetItem* item = new QListWidgetItem;
		item->setText(m_packInfoList.at(i)._xmmc);
		ui->listWidget->addItem(item);
	}
	ui->listWidget_2->clear();
	for (int i = 0; i < m_ckFileInfoList.size(); ++i)
	{
		QListWidgetItem* item = new QListWidgetItem;
		item->setText(m_ckFileInfoList.at(i)._wjmc);
		ui->listWidget_2->addItem(item);
	}
}

void DownloadPrjDataDlg::updateUploadPage(const QString& packpath)
{
	ui->lineEdit_packetpath->setText(packpath);
	QString xmlpath = packpath + "/项目信息.xml";
	if (QFileInfo(xmlpath).exists())
	{
		if (readConfigFile(xmlpath, m_curPrjInfo))
		{
			ui->lineEdit_upload_xmmc->setText(m_curPrjInfo._xmmc);
			ui->lineEdit_upload_zyry->setText(m_curPrjInfo._cjry);
		}
		else
		{
			ui->lineEdit_upload_xmmc->setText(QString());
			ui->lineEdit_upload_zyry->setText(QString());
		}
	}
	QString packetxml = packpath + "/datapacket.xml";
	if (QFileInfo(packetxml).exists())
	{
		QVariantMap infoMap;
		if (readDataPakcetXml(packetxml, infoMap))
		{
			auto it = infoMap.find("sbbh");
			if (it != infoMap.end())
			{
				ui->lineEdit_upload_sbbh->setText(it.value().toString());
			}
			it = infoMap.find("zyry");
			if (it != infoMap.end())
			{
				ui->lineEdit_upload_zyry->setText(it.value().toString());
			}
			it = infoMap.find("bz");
			if (it != infoMap.end())
			{
				ui->lineEdit_upload_bz->setText(it.value().toString());
			}
		}
		else
		{
			ui->lineEdit_upload_sbbh->setText(QString());
			//ui->lineEdit_upload_zyry->setText(QString());
			ui->lineEdit_upload_bz->setText(QString());
		}
	}
}

void DownloadPrjDataDlg::showEvent(QShowEvent *event)
{
	if (event->type() == QEvent::Show)
	{//设置个延时去刷
		//on_pushButton_reflash_clicked();
		_isbusy = false;
	}
}

bool DownloadPrjDataDlg::createDataPacketXml(const QString& dirpath)
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString xmlpath = dirpath + "/datapacket.xml";
	QFile wfile(xmlpath);
	if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return false;
	}
	QTextStream out(&wfile);
	out.setCodec(QTextCodec::codecForName("utf-8")); //设置输出文件编码格式
	//out.setGenerateByteOrderMark(true); //with BOM
	QDomDocument document;
	QString strHeader("version=\"1.0\"");
	document.appendChild(document.createProcessingInstruction("xml", strHeader));

	// dataPacket标签
	QDomElement root_elem = document.createElement("dataPacket");
	document.appendChild(root_elem);
	// version标签
	QDomElement version_elem = document.createElement("version");
	QDomText text = document.createTextNode(current_date_time.toString("yyyyMMdd"));
	version_elem.appendChild(text);
	root_elem.appendChild(version_elem);
	// sbbh标签
	QDomElement sbbh_elem = document.createElement("sbbh");
	text = document.createTextNode(ui->lineEdit_upload_sbbh->text());
	sbbh_elem.appendChild(text);
	root_elem.appendChild(sbbh_elem);
	// scsj标签
	QDomElement scsj_elem = document.createElement("scsj");
	text = document.createTextNode(current_date_time.toString("yyyy-MM-dd hh:mm:ss"));
	scsj_elem.appendChild(text);
	root_elem.appendChild(scsj_elem);
	// zyry标签
	QDomElement zyry_elem = document.createElement("zyry");
	text = document.createTextNode(ui->lineEdit_upload_zyry->text());
	zyry_elem.appendChild(text);
	root_elem.appendChild(zyry_elem);
	// bz标签
	QDomElement bz_elem = document.createElement("bz");
	text = document.createTextNode(ui->lineEdit_upload_bz->text());
	bz_elem.appendChild(text);
	root_elem.appendChild(bz_elem);

	document.save(out, 4);
	wfile.close();
	return true;
}
