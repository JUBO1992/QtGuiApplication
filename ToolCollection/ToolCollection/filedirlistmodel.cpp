#include "filedirlistmodel.h"
#include <QColor>
#include <QFileInfo>

FileDirListModel::FileDirListModel(QObject *parent)
	: QStringListModel(parent)
{

}

FileDirListModel::~FileDirListModel()
{

}

QVariant FileDirListModel::data(const QModelIndex &index, int role) const
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
		QString fullpath = inputDir + stringList().at(index.row());
		if (fileDirJudgeByName(fullpath) == 1)
		{
			return QColor(224, 224, 255);
		}
		else if (fileDirJudgeByName(fullpath) == 2)
		{
			return QColor(255, 224, 224);
		}
		else
		{
			return QColor(255, 255, 255);
		}

	}
	return QVariant();
}

int FileDirListModel::fileDirJudgeByName(QString fullpath) const
{
	QFileInfo info(fullpath);
	if (info.isFile())
	{
		return 1;
	}
	if (info.isDir())
	{
		return 2;
	}
	return 0;
}
