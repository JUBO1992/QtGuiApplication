#ifndef __SEARCHLISTMODEL_H__
#define __SEARCHLISTMODEL_H__

#include <QStringListModel>

enum ItemStatus
{
	Unknow = 0,
	Finished = 1,
	Unfinish = 2,
	Current = 3
};

class SearchListModel : public QStringListModel
{
	Q_OBJECT

public:
	SearchListModel(QObject *parent = 0);
	~SearchListModel();

	void setStatusList(const QList<ItemStatus> list);
	void searchList(QString);
	QStringList getOriginalList();

private:
	QVariant data(const QModelIndex &index, int role) const;

private:
	QList<ItemStatus> _cur_status_list;
	QList<ItemStatus> _ori_status_list;
	QStringList _original_list;
};
#endif
