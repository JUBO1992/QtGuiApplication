#include "ynlogindlg.h"
#include "ui_ynlogindlg.h"

#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QCryptographicHash>
#include <QStyledItemDelegate>

//#include "irxMain.h"

#include "urlconfig.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

//extern iDataInterface* g_pInterface;
extern QString s_current_login_username;

static const char* s_key_old_user_names = "YN_LoginNames";	//存储历史登录用户名的Key

YNLoginDlg::YNLoginDlg(QWidget *parent) :
QDialog(parent), _isSuccess(false),
ui(new Ui::YNLoginDlg)
{
	ui->setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);//无边框

	this->ui->pushButton_forget->setVisible(false);
	this->ui->pushButton_registration->setVisible(false);

	this->ui->label_close->installEventFilter(this);

	//不加这句貌似QAbstractItemView的stylesheet不起作用
	ui->comboBox_name->setItemDelegate(new QStyledItemDelegate());
	ui->comboBox_name->setEditable(true);
	QLineEdit* lineEdit = ui->comboBox_name->lineEdit();
	lineEdit->setPlaceholderText(QStringLiteral("云平台账号"));

	ui->lineEdit_passwd->setPlaceholderText(QStringLiteral("密码"));

	QStringList oldNames;// g_pInterface->property(s_key_old_user_names).toString().split(";");
	//if (!oldNames.contains("idata")) oldNames.append("idata");
	foreach(QString n, oldNames)
		if(!n.isEmpty())
			ui->comboBox_name->addItem(n);
	ui->comboBox_name->setCurrentText(QString());

	getServerInfo();
}

YNLoginDlg::~YNLoginDlg()
{
    delete ui;
}

void YNLoginDlg::showEvent(QShowEvent *)
{
	//QWidget *p = qobject_cast<QWidget *>(this->parent());
	//p->hide();
}

void YNLoginDlg::closeEvent(QCloseEvent *)
{
	if (!_isSuccess)
	{
		QWidget *p = qobject_cast<QWidget *>(this->parent());
		p->close();
		qApp->exit(0);
	}
}

void YNLoginDlg::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Escape)
		return;

	QDialog::keyPressEvent(event);
}

bool YNLoginDlg::eventFilter(QObject *sender, QEvent *event)
{
    QEvent::Type t = event->type();
    if(sender == this->ui->label_close)
    {
        if(t == QEvent::MouseButtonRelease)
        {
			if (QMessageBox::question(this, "询问", "是否退出？") == QMessageBox::Yes)
			{
				this->reject();
				closeEvent(nullptr);
				return true;
			}
        }
    }
    return QDialog::eventFilter(sender, event);
}

bool YNLoginDlg::isSccess()
{
	return _isSuccess;
}

void YNLoginDlg::getServerInfo()
{
	QSettings setting(qApp->applicationDirPath() + "/ynserverconfig.ini", QSettings::IniFormat);
	setting.beginGroup("config");

	bool ok;
	_ip = setting.value("IP").toString();

	quint16 temp = setting.value("Port").toUInt(&ok);
	if (ok)
		_port = temp;

	_username = setting.value("Username").toString();
	_passwd = setting.value("Passwd").toString();

	setting.endGroup();
}

bool YNLoginDlg::checkUser(const QString &name, const QString &passwd)
{
	//if (_ip.isEmpty() || _username.isEmpty())
	//	return false;

	//TODO  云南没有开放登录接口，所以这里全部通过
	return true;

	_isSuccess = false;
	return false;
}

void YNLoginDlg::on_pushButton_ok_clicked()
{
	QString name = this->ui->comboBox_name->currentText().trimmed();
	if (name.isEmpty())
	{
		this->ui->comboBox_name->setFocus();
		return;
	}

	QString passwd = this->ui->lineEdit_passwd->text().trimmed();
	if (passwd.isEmpty())
	{
		this->ui->lineEdit_passwd->setFocus();
		return;
	}

	if (!_isSuccess)
		_isSuccess = checkUser(name, QCryptographicHash::hash(passwd.toUtf8(), QCryptographicHash::Md5).toHex().toUpper());

	if (!_isSuccess)
	{
		static int errorCount = 0;
		QMessageBox::critical(this, "错误", "登录失败！");
		if (++errorCount >= 5)
		{
			QMessageBox::critical(this, "错误", "登录失败次数过多，已经锁定登录！");
			this->reject();
			closeEvent(nullptr);
			qApp->exit(0);
		}
		return;
	}

	//QWidget *p = qobject_cast<QWidget *>(this->parent());
	//p->show();

	//QMainWindow *pMain = NULL;
	//g_pInterface->GetMainWindow(pMain);
	QString appName = this->windowTitle();

	//g_pInterface->iDataPrintf(name + "，已登录大数据平台！");
	this->setWindowTitle(appName + " - " + name + "[已登录]");

	s_current_login_username = name;

	QStringList oldNames;// g_pInterface->property(s_key_old_user_names).toString().split(";");
	if (!oldNames.contains(name))
	{
		oldNames.append(name);
		oldNames.removeAll("");
		//g_pInterface->setProperty(s_key_old_user_names, oldNames.join(";"));
	}

	this->accept();
}

void YNLoginDlg::on_pushButton_registration_clicked()
{
	QMessageBox::information(this, "提醒", "系统暂不允许自建账户，请联系管理员！");
}

void YNLoginDlg::on_pushButton_forget_clicked()
{
	QMessageBox::information(this, "提醒", "系统暂不允许自行修改密码，请联系管理员！");
}
