#ifndef MY_MODEL_H
#define MY_MODEL_H

#include <QAbstractTableModel>

class MyModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	MyModel(QObject *parent);
	~MyModel();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif //MY_MODEL_H