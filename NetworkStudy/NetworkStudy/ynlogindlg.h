#ifndef SD_LOGINWIDGET_H
#define SD_LOGINWIDGET_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class YNLoginDlg;
}

class YNLoginDlg : public QDialog
{
    Q_OBJECT

public:
	explicit YNLoginDlg(QWidget *parent);
    ~YNLoginDlg();

	bool isSccess();

private:
	void getServerInfo();
	bool checkUser(const QString &name, const QString &passwd);

protected:
	virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
	virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
	virtual void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *sender, QEvent *event) Q_DECL_OVERRIDE;

private slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_registration_clicked();
	void on_pushButton_forget_clicked();

private:
	Ui::YNLoginDlg *ui;
	bool _isSuccess;

	QString _ip;
	quint16 _port;
	QString _username;
	QString _passwd;

};

#endif // SD_LOGINWIDGET_H
