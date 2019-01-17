#include "networkstudy.h"
#include "cexplorewnd.h"
#include "mysoap.h"
#include "downloadprjdatadlg.h"
#include <QMessageBox>
#include "ynlogindlg.h"

#pragma execution_character_set("utf-8")

MySoapManager *g_NetworkInterface = NULL;
QString s_current_login_username;
QString g_current_packetpath;			//当前数据包路径（记录最近一次下载解压的数据包路径）
DownloadPrjDataDlg* g_uploadDlg;		//上传对话框
CExploreWnd* g_pWebWnd = NULL;

NetworkStudy::NetworkStudy(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QColor qclr(129, 168, 211);
	QPalette palette;
	palette.setBrush(this->backgroundRole(), qclr);
	this->setPalette(palette);

	g_NetworkInterface = new MySoapManager(this);
}

NetworkStudy::~NetworkStudy()
{
	if (g_NetworkInterface != NULL)
	{
		g_NetworkInterface->deleteLater();
	}
}

void NetworkStudy::on_pushButton_clicked()
{
	if (g_pWebWnd == NULL)
	{
		g_pWebWnd = new CExploreWnd(this);
	}
	if (g_pWebWnd)
	{
		g_pWebWnd->show();
	}
}

void NetworkStudy::on_pushButton_login_clicked()
{
	YNLoginDlg loginDlg(this);
	loginDlg.exec();
}

void NetworkStudy::on_pushButton_download_clicked()
{
	if (s_current_login_username.isEmpty())
	{
		QMessageBox::information(this, "错误", "必须先登录大数据平台，才可以下载数据！");
		return;
	}

	DownloadPrjDataDlg prjDlg(this);
	prjDlg.exec();
}

void NetworkStudy::on_pushButton_upload_clicked()
{
	if (s_current_login_username.isEmpty())
	{
		QMessageBox::information(this, "错误", "必须先登录大数据平台，才可以上传数据！");
		return;
	}
	if (g_uploadDlg == NULL)
	{
		g_uploadDlg = new DownloadPrjDataDlg(this, DownloadPrjDataDlg::UloadPacketPage);
	}
	g_uploadDlg->show();
}


