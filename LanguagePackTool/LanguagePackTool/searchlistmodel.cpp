#include <QColor>
#include "searchlistmodel.h"

SearchListModel::SearchListModel(QObject *parent)
	: QStringListModel(parent)
{
	_cur_status_list.clear();
	_ori_status_list.clear();
	_original_list.clear();
}

SearchListModel::~SearchListModel()
{
	_cur_status_list.clear();
	_ori_status_list.clear();
	_original_list.clear();
}

void SearchListModel::setStatusList(const QList<ItemStatus> list)
{
	_original_list = this->stringList();
	_cur_status_list = list;
	_ori_status_list = list;
}

void SearchListModel::searchList(QString str)
{
	if (str.isEmpty() || str.isNull())
	{
		this->setStringList(_original_list);
		this->_cur_status_list = _ori_status_list;
		return;
	}
	QStringList templist;
	QList<ItemStatus> tempstatus;
	for (int i = 0; i < _original_list.size(); i++)
	{
		if (_original_list[i].contains(str, Qt::CaseInsensitive))
		{
			templist.push_back(_original_list[i]);
			tempstatus.push_back(_ori_status_list[i]);
		}
	}
	this->setStringList(templist);
	this->_cur_status_list = tempstatus;//此处不可调用setStatusList
}

QStringList SearchListModel::getOriginalList()
{
	return _original_list;
}

QVariant SearchListModel::data(const QModelIndex &index, int role) const
{
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
		int i = index.row();
		if (i >= 0 && i < _cur_status_list.size())
		{
			if (ItemStatus::Current == _cur_status_list[i])
			{
				return QColor(204, 204, 255);
			}
			else if (ItemStatus::Finished == _cur_status_list[i])
			{
				return QColor(255, 204, 204);
			}
			else if (ItemStatus::Unfinish == _cur_status_list[i])
			{
				return QColor(232, 232, 232);
			}
		}
	}
	return QVariant();
}
