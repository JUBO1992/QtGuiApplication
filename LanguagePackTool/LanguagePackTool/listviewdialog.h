#ifndef __LISTVIEWDIALOG_H__
#define __LISTVIEWDIALOG_H__

#include <QDialog>
#include <QStringListModel>
#include "ui_listviewdialog.h"
#include "listviewmodel.h"

enum ListModel
{
	Folder = 0,
	File = 1,
	Text = 2
};

class ListViewDialog : public QDialog
{
	Q_OBJECT

public:
	ListViewDialog(ListModel model = ListModel::Folder, QWidget *parent = 0);
	~ListViewDialog();

	void setListModel(ListModel model = ListModel::Folder){ _listmodel = model; }
	void setStringList(const QStringList &list);
	void getStringList(QStringList &list);
	void setStringList(const QList<QString> &list);
	void getStringList(QList<QString> &list);
	bool getConfirmFlag();

public slots:
	void on_btn_AddItem_clicked();
	void on_btn_DeleteItem_clicked();
	void on_btn_ClearAll_clicked(); 
	void on_btn_OK_clicked();
	void on_btn_Cancel_clicked();
	void on_listView_clicked(QModelIndex);

private:
	Ui::ListViewDialog ui;
	ListModel _listmodel;
	ListViewModel *_model;
	bool _confirmflag;
};
#endif
