#include "checkitemwnd.h"
#include <QMessageBox>

CheckItemWnd::CheckItemWnd(QWidget *parent, CheckItemStruct item)
: QDialog(parent)
, m_item(item)
{
	ui.setupUi(this);

	ui.comboBox_isCheck->addItems(QStringList() << "是" << "否");
	ui.comboBox_state->addItems(QStringList() << "必选" << "可选");
	if (item._isCheck)
	{
		ui.comboBox_isCheck->setCurrentIndex(0);
	}
	else
	{
		ui.comboBox_isCheck->setCurrentIndex(1);
	}
	if (item._state == CheckStateEnum::Required)
	{
		ui.comboBox_state->setCurrentIndex(0);
	}
	else
	{
		ui.comboBox_state->setCurrentIndex(1);
	}

	//QRegExp regExp("");
	//ui.lineEdit_condition->setValidator(new QRegExpValidator(regExp, this));
	ui.lineEdit_condition->setText(item._condition);
}

CheckItemWnd::~CheckItemWnd()
{

}

void CheckItemWnd::on_pushButton_ok_clicked()
{
	if (ui.lineEdit_condition->text().isEmpty())
	{
		QMessageBox::information(this, "提示", "检查项不能为空！", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	if (ui.comboBox_isCheck->currentIndex() == 0)
	{
		m_item._isCheck = true;;
	}
	else
	{
		m_item._isCheck = false;;
	}
	if (ui.comboBox_state->currentIndex() == 0)
	{
		m_item._state = CheckStateEnum::Required;
	}
	else
	{
		m_item._state = CheckStateEnum::Optional;
	}
	m_item._condition = ui.lineEdit_condition->text();
	accept();
}

void CheckItemWnd::on_pushButton_cancel_clicked()
{
	reject();
}
