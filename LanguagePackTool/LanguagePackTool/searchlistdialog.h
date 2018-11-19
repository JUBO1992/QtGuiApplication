#ifndef __SEARCHLISTDIALOG_H__
#define __SEARCHLISTDIALOG_H__

#include <QDialog>
#include "ui_searchlistdialog.h"
#include "searchlistmodel.h"
#include "languagepack.h"

class SearchListDialog : public QDialog
{
	Q_OBJECT

public:
	SearchListDialog(QWidget *parent = 0);
	~SearchListDialog();
	void setStringList(const QList<LanguageObj> &list);
	QString getCurrentChinese();
	void setCurrentIndex(int index);
	int getCurrentIndex();

private slots:
	void on_listview_doubleclicked(QModelIndex);
	void on_lineedit_textchanged(QString);

private:
	Ui::SearchListDialog ui;

	SearchListModel *_model;
	QString _select_chinese;
	int _select_index;
};
#endif
