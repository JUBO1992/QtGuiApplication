#include "listviewdialog.h"
#include <QDir>
#include <QFileDialog>

ListViewDialog::ListViewDialog(ListModel model, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//this->setFixedSize(410, 445);
	//this->setWindowFlags(Qt::WindowCloseButtonHint);
	QObject::connect(ui.btn_AddItem, SIGNAL(clicked()), this, SLOT(on_btn_AddItem_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_DeleteItem, SIGNAL(clicked()), this, SLOT(on_btn_DeleteItem_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_ClearAll, SIGNAL(clicked()), this, SLOT(on_btn_ClearAll_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.btn_OK, SIGNAL(clicked()), this, SLOT(on_btn_OK_clicked()), Qt::UniqueConnection);
	//QObject::connect(ui.btn_Cancel, SIGNAL(clicked()), this, SLOT(on_btn_Cancel_clicked()), Qt::UniqueConnection);
	QObject::connect(ui.listView, SIGNAL(clicked(QModelIndex)), this, SLOT(on_listView_clicked(QModelIndex)), Qt::UniqueConnection);

	_listmodel = model;
	_model = new ListViewModel();
	ui.listView->setModel(_model);
	_confirmflag = false;
}

ListViewDialog::~ListViewDialog()
{
	delete _model;
	_model = NULL;
}

void ListViewDialog::setStringList(const QStringList &list)
{
	_model->setStringList(list);
}

void ListViewDialog::getStringList(QStringList &list)
{
	list.clear();
	QStringList slist = _model->stringList();
	for (int i = 0; i < slist.size(); i++)
	{
		if (!slist[i].isEmpty())
		{
			list.push_back(slist[i]);
		}
	}
}

void ListViewDialog::setStringList(const QList<QString> &list)
{
	QStringList slist;
	QListIterator<QString> i(list);
	while (i.hasNext())
	{
		slist.push_back(i.next());
	}
	_model->setStringList(slist);
}

void ListViewDialog::getStringList(QList<QString> &list)
{
	list.clear();
	QStringList slist = _model->stringList();
	for (int i = 0; i < slist.size(); i++)
	{
		if (!slist[i].isEmpty())
		{
			list.push_back(slist[i]);
		}
	}
}

bool ListViewDialog::getConfirmFlag()
{
	return _confirmflag;
}

void ListViewDialog::on_btn_AddItem_clicked()
{
	QString item = "";
	if (_listmodel == ListModel::Folder)
	{
		item = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, "Select Folder", QDir::currentPath()));
		if (!item.isEmpty())
		{
			_model->insertQString(item);
		}
	}
	else if (_listmodel == ListModel::File)
	{
		item = QFileDialog::getOpenFileName(this, "Select File", "", "All files(*.*)");
		if (!item.isEmpty())
		{
			_model->insertQString(item);
		}
	}
	else if (_listmodel == ListModel::Text)
	{
		QModelIndex index = _model->insertQString(item);
		ui.listView->setCurrentIndex(index);
		on_listView_clicked(index);
	}

	ui.btn_AddItem->setFocus();
}

void ListViewDialog::on_btn_DeleteItem_clicked()
{
	QModelIndex index = ui.listView->currentIndex();
	int i = index.row();
	_model->removeRow(i);
	ui.btn_DeleteItem->setFocus();
	if (_model->rowCount() <= 0)
	{
		ui.btn_DeleteItem->setEnabled(false);
		ui.listView->setFocus();
	}
}

void ListViewDialog::on_btn_ClearAll_clicked()
{
	_model->removeRows(0, _model->rowCount());
	ui.btn_ClearAll->setFocus();
	ui.btn_DeleteItem->setEnabled(false);
}

void ListViewDialog::on_btn_OK_clicked()
{
	_confirmflag = true;
	this->close();
}

void ListViewDialog::on_btn_Cancel_clicked()
{
	_confirmflag = false;
	this->close();
}

void ListViewDialog::on_listView_clicked(QModelIndex)
{
	QModelIndex index = ui.listView->currentIndex();
	int i = index.row();
	if (i >= 0 && i < _model->rowCount())
	{
		ui.btn_DeleteItem->setEnabled(true);
		return;
	}
	ui.btn_DeleteItem->setEnabled(false);
}
