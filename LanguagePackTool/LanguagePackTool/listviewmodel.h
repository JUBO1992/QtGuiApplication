#ifndef __LISTVIEWMODEL_H__
#define __LISTVIEWMODEL_H__

#include <QStringListModel>

class ListViewModel : public QStringListModel
{
	Q_OBJECT

public:
	ListViewModel(QObject *parent = 0);
	~ListViewModel();
	QModelIndex insertQString(const QString &str);

private:
	QVariant data(const QModelIndex &index, int role) const;

};
#endif
