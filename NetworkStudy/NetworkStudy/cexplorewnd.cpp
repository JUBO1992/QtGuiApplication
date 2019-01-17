#include "cexplorewnd.h"
#include <QUrl>

CExploreWnd::CExploreWnd(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	QColor qclr(129, 168, 211);
	QPalette palette;
	palette.setBrush(this->backgroundRole(), qclr);
	this->setPalette(palette);
	ui.lineEdit_url->setText("https://m.baidu.com");
}

CExploreWnd::~CExploreWnd()
{

}

void CExploreWnd::on_pushButton_go_clicked()
{
	QString url = ui.lineEdit_url->text();
	QUrl qurl(url);
	ui.webView->load(qurl);
}
