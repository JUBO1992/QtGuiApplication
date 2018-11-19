#include "searchlistdialog.h"

SearchListDialog::SearchListDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//this->setFixedSize(296, 480);
	//this->setWindowFlags(Qt::WindowCloseButtonHint);
	QObject::connect(ui.listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_listview_doubleclicked(QModelIndex)), Qt::UniqueConnection);
	QObject::connect(ui.lineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_lineedit_textchanged(QString)), Qt::UniqueConnection);

	_model = new SearchListModel();
	ui.listView->setModel(_model);
	_select_chinese = "";
	_select_index = -1;
}

SearchListDialog::~SearchListDialog()
{
}

void SearchListDialog::setStringList(const QList<LanguageObj> &list)
{
	QStringList stringlist;
	QList<ItemStatus> statuslist;
	for (int i = 0; i < list.size(); i++)
	{
		stringlist.push_back(list[i].chinese);
		ItemStatus istatus = ItemStatus::Unknow;
		if (i == _select_index)
		{
			istatus = ItemStatus::Current;
		}
		else if (!list[i].foreign.isEmpty())
		{
			istatus = ItemStatus::Finished;
		}
		else
		{
			istatus = ItemStatus::Unfinish;
		}
		statuslist.push_back(istatus);
	}
	_model->setStringList(stringlist);//在setStatusList之前调用
	_model->setStatusList(statuslist);
	QModelIndex index = _model->index(_select_index);
	ui.listView->setCurrentIndex(index);
}

QString SearchListDialog::getCurrentChinese()
{
	return _select_chinese;
}

void SearchListDialog::setCurrentIndex(int index)
{
	_select_index = index;
}

int SearchListDialog::getCurrentIndex()
{
	return _select_index;
}

void SearchListDialog::on_listview_doubleclicked(QModelIndex index)
{
	QStringList list = _model->stringList();
	int i = index.row();
	if (i < 0 || i >= list.size())
	{
		return;
	}
	_select_chinese = list[i];
	_select_index = _model->getOriginalList().indexOf(_select_chinese);
	this->close();
}

void SearchListDialog::on_lineedit_textchanged(QString)
{
	QString str = ui.lineEdit->text();
	_model->searchList(str);
}
