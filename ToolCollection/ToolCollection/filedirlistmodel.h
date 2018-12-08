#ifndef FILEDIRLISTMODEL_H
#define FILEDIRLISTMODEL_H

#include <QStringListModel>

class FileDirListModel : public QStringListModel
{
	Q_OBJECT

public:
	FileDirListModel(QObject *parent = 0);
	~FileDirListModel();

	QString inputDir;

private:
	QVariant data(const QModelIndex &index, int role) const;
	int fileDirJudgeByName(QString fullpath) const;
};

#endif // FILEDIRLISTMODEL_H
