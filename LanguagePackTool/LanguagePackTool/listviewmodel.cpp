#include "listviewmodel.h"
#include <QColor>

ListViewModel::ListViewModel(QObject *parent)
	: QStringListModel(parent)
{
}

ListViewModel::~ListViewModel()
{
}

QModelIndex ListViewModel::insertQString(const QString &str)
{
	QStringList strlist = this->stringList();
	strlist.push_back(str);
	this->setStringList(strlist);
	return this->index(strlist.size() - 1);
}

QVariant ListViewModel::data(const QModelIndex &index, int role) const
{
	//QStringListModel::data(index, role);
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole)
	{
		return int(Qt::AlignLeft | Qt::AlignVCenter);
	}
	else if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		if (index.row() < 0 && index.row() >= stringList().size())
		{
			return QVariant();
		}
		else
		{
			return stringList().at(index.row());
		}
	}
	else if (role == Qt::BackgroundColorRole)
	{
		if (index.row() % 2 == 0)
		{
			return QColor(204, 204, 255);
		}
		else
		{
			return QColor(255, 204, 204);
		}
	}
	return QVariant();
}
